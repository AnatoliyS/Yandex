/* Анатолий Селезнёв, anatoliy.seleznyov@gmail.com, https://github.com/AnatoliyS/Yandex

CTimeStamp - класс Временной отметки. Например, можно создать разные временные 
отметки для разных часовых поясов.
CTimeStampFromString - класс раширяет понятие Временной отметки, позволяя создавать 
ее из строки. Возможное масштабирование: можно сделать много подобных констуркторов 
для временной отметки (например, из картинки, из атрибутов файла и т.д.), тогда
можно использовать фабрику.

СAnlge - класс Угол. Инкапсулирует необходимые операции. Внутри спрятаны типы вывода угла.

CClock - класс Часы, имеет ссылку на временную отметку. Таким образом, можем создать 
1 временную отметку и "настроить" на нее много часов. Достаточно модифицировать 1 
временную отметку, и все часы, настроенные на нее автоматически будут обновляться. 
Внутри спрятаны типы часов (так как их всего 2, и других типов стрелочных часов
нет, то отдельных классов для них создавать нет необходимости)
*/
#define _USE_MATH_DEFINES

#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include<exception>
#include<stdlib.h>
#include<iostream>


const double fullCircle = 360.0; // градусов
const double minutesPerDegree = 60.0;
const double secondsPerMinute = 60.0;

///
/// Класс Временная отметка. Время в 24-часовом формате
///
class CTimeStamp{
protected:
	int hour;
	int minute;
	int second;

public:	
	CTimeStamp() : hour(0), minute(0), second(0) {};
	CTimeStamp(int h, int m, int s = 0): hour(h), minute(m), second(s) {};
	~CTimeStamp() {}

	int getHour(){ return hour; }
	int setHour(int h){ hour = h; }
	int getMinute(){ return minute; }	
	int setMinute(int m){ minute = m; }
};

///
/// Класс для создания временной отметки из строки.
/// Строка должна строго соответствовать формату "HH:MM" или "HH:MM AM\PM"
/// Символы A,P,M - латинские большие буквы. Пробел допускается только для разделения 
/// времени и модификатора AM\PM
///
class CTimeStampFromString : public CTimeStamp{
private:
	// Последовательные состояния
	enum ScanerState{		
		scaningHours, 		// сканируем часы
		scaningMinutes, 	// сканируем минуты
		scaningFormat, 		// сканируем AM\PM (если нужно)
		scaningDone			// сканирование завершено
	} state;

	int scaned_symbols;     // Кол-во уже просканированных символов в данном состоянии 
	char* current_symbol;	// Текущий символ

	void scanHour();
	void scanMinute();
	void scanFormat();

public:
	CTimeStampFromString() {};
	CTimeStampFromString(char* time_str);
	~CTimeStampFromString() {};
	// Сканирует строку и получает из нее временную отметку
	void scan(char* time_str);
};


class WrongFormat24Exception: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Неверный формат времени. Должно быть HH:MM";
	}
};

class WrongFormat12Exception: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Неверный формат времени. Должно быть HH:MM AM\\PM";
	}
};

class WrongModifierException: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Неверный формат модификатора AM\\PM";
	}
};

class WrongFormat12RangeException: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Неверный формат времени. При указании AM\\PM часы указываются в диапазоне 0..11";
	}
};

class WrongFormat24RangeException: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Неверный формат времени. Число часов <= 23, минут <= 59";
	}
};

class MoreSymbolsException: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Неверный формат времени. Лишние символы в конце строки";
	}
};

class NoArgumentsException: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Недостаточно аргументов";
	}
};

class WrongAngleTypeException: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Неверно задан формат вывода угла. (deg\\rad\\dms)";
	}
};

class WrongClockTypeException: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Неверно задан тип часов. (mechanic\\quartz)";
	}
};


// Сканирует число часов
void CTimeStampFromString :: scanHour()
{
	// Первые 2 символа трактуем как цифры в числе часов
	if(isdigit(*current_symbol) && scaned_symbols < 2)
	{
		hour = hour*10 + *current_symbol - '0';
		scaned_symbols++;
	}
	else	
		// Если уже прочитали 2 символа и встретили ':' - начинаем сканировать минуты
		if(scaned_symbols == 2 && *current_symbol == ':') 
		{
			state = scaningMinutes;
			scaned_symbols = 0;
		}
		else 
			throw WrongFormat24Exception();
	
}

// Сканирует число минут
void CTimeStampFromString :: scanMinute()
{
	if(isdigit(*current_symbol) && scaned_symbols < 2)
	{
		minute = minute*10 + *current_symbol - '0';
		scaned_symbols++;
	}
	else
	{
		if(scaned_symbols == 2)
		{
			if(*current_symbol == ' ')
			{
				state = scaningFormat;
				scaned_symbols = 0;
			}
			else 
				throw WrongModifierException();
		}
		else 
			throw WrongFormat24Exception();						
	}
}

// Сканирует формат времени
void CTimeStampFromString :: scanFormat()
{		
	if(scaned_symbols == 0)
	{ 
		// Ожидаем встретить A или P
		if(*current_symbol == 'A' || *current_symbol == 'P')
		{
			if(hour > 11)
				throw WrongFormat12RangeException();

			if(*current_symbol == 'P')
				hour += 12;	

			scaned_symbols++;							
		}
		else
			throw WrongModifierException();	
		return;
	}

	if(scaned_symbols == 1)
		// Ожидаем встретить M
		if(*current_symbol == 'M')
			state = scaningDone;							
		else
			throw WrongModifierException();
}


// Функция сканирования
void CTimeStampFromString :: scan(char* time_str)
{
	hour = minute = second = 0;

	state = scaningHours;

	current_symbol = time_str;
	scaned_symbols = 0;

	// Пока не конец строки и не завершили сканирование
	while(*current_symbol != '\0' && state != scaningDone)
	{		
		switch(state)
		{
			case scaningHours:
				scanHour();					
				break;
			case scaningMinutes:
				scanMinute();
				break;
			case scaningFormat:
				scanFormat();
				break;
			case scaningDone: // Если сканирование завершено, но не конец строки
				throw MoreSymbolsException();
		}
		current_symbol++;	
	}
		
	// Сюда попадаем либо если некорректный формат, либо если имеем строку "HH:MM"
	if(state != scaningDone)
		// Если завершили сканирование в любой другой момент, кроме как перед началом чтения формата - ошибка
		if(state != scaningMinutes || scaned_symbols != 2)
			throw WrongFormat12Exception();			
	

	if(hour > 23 || minute > 59)
		throw WrongFormat24RangeException();

}

// Конструктор Временной отметки из строки
CTimeStampFromString :: CTimeStampFromString(char* time_str)
{
	scan(time_str);
}

///
/// Класс угол (для удобства в этой программе все вычисления в градусах)
///
class CAngle{
public:
	enum AngleOutputType { Deg, Rad, Dms }; // Тип вывода
	
private:
	double value;

public:
	CAngle(double _value) : value(_value) {};
	CAngle(const CAngle& other);
	CAngle() : value(0.0) {};
	~CAngle() {};

	CAngle& operator+=(const CAngle&);
	CAngle& operator+(const CAngle&);
	CAngle& operator-=(const CAngle&);
	CAngle& operator-(const CAngle&);
	bool operator==(const CAngle&);

	// Преобразует угол к меньшему из 2 возможных
	void toLower();
	void out(const AngleOutputType type);
	
};

CAngle :: CAngle(const CAngle& other){
	value = other.value;
}
CAngle& CAngle :: operator+=(const CAngle& other)
{
	value = value + other.value;
	// Фактически, вычисления углов по модулю fullCircle
	while(value >= fullCircle)
		value -= fullCircle;
	return *this;
}
bool CAngle :: operator==(const CAngle& other)
{
	double lowerAng1 = (value < fullCircle - value)? value : fullCircle - value;
	double lowerAng2 = (other.value < fullCircle - other.value)? other.value : fullCircle - other.value;
	return (lowerAng1 == lowerAng2);
}
CAngle& CAngle :: operator+(const CAngle& other)
{
	CAngle& res = *new CAngle(*this);
	res += other;
	return res;
}
CAngle& CAngle :: operator-=(const CAngle& other)
{
	value = value - other.value;
	// Фактически, вычисления углов по модулю fullCircle
	while(value < 0.0)
		value += fullCircle;
	return *this;
}
CAngle& CAngle :: operator-(const CAngle& other)
{
	CAngle& res = *new CAngle(*this);
	res -= other;
	return res;
}

// Возвращает меньший из 2 возможных углов
void CAngle :: toLower(){
	if(2.0*value > fullCircle)
		value = fullCircle - value;
}

// Вывести угол в соответствии с типом
void CAngle :: out(const CAngle::AngleOutputType type){
	switch(type){
		case Deg:
			printf("%.4lf\n", value);
			break;
		case Rad:
			printf("%.4lf\n", value*2.0*M_PI/fullCircle);
			break;
		case Dms:
			printf("%d.%02.0lf'%02.0lf\"\n", int(value), 
												(value - int(value))*minutesPerDegree, 
												((value - int(value))*minutesPerDegree 
												- int((value - int(value))*minutesPerDegree))*secondsPerMinute);
	}
}


///
/// Класс Часы
///
class CClock{
public:
	enum clockType { Mechanic, Quartz };

private:
	// Указатель на временную отметку
	CTimeStamp* time;
	clockType type;
	// Углы стрелок от 0 градусов (00:00)
	CAngle hourAngle;
	CAngle minuteAngle;	

public:	
	CClock() {};
	CClock(CTimeStamp* time_stamp, const clockType tp = Mechanic);
	// Мы не очищаем данные по ссылке на TimeStamp, т.к. в этой архитектуре подразумевается,
	// что мы можем использовать несколько часов, ссылающихся на 1 TimeStamp.
	~CClock() {};
	
	void setClockType(clockType tp){ type = tp; }
	clockType getClockType(){ return type; }
	
	void countAngle();
	CAngle getAngle();
	void printAngle(const CAngle::AngleOutputType type);
	
};

// Расчитать угол между стрелками
void CClock :: countAngle(){
	hourAngle = CAngle(double(time->getHour()-12)*30.0); // 1 час = 30 градусов
	minuteAngle = CAngle(double(time->getMinute())*6.0); // 1 минута = 360/60 = 6 градусов

	if(type == Mechanic)
		hourAngle += double(time->getMinute())/2.0; // Прибавка к часам = minute * (30 град/60 минут)
}

// Конструктор Часов по временной отметке
CClock :: CClock(CTimeStamp* time_stamp, const clockType tp) : time(time_stamp), type(tp) {
	countAngle();
}

CAngle CClock :: getAngle(){
	countAngle();
	CAngle res = hourAngle - minuteAngle;
	res.toLower();
	return res;
}

void CClock :: printAngle(const CAngle::AngleOutputType tp){
	countAngle();
	CAngle res = hourAngle - minuteAngle;
	res.toLower();
	res.out(tp);
}

///
/// Вспомагательные функции для получения типов:
///
CAngle::AngleOutputType getAngleType(char* str){
	if(strcmp(str, "deg") == 0)
		return CAngle::Deg;
	
	if(strcmp(str, "rad") == 0)
		return CAngle::Rad;
	
	if(strcmp(str, "dms") == 0)
		return CAngle::Dms;

	throw WrongAngleTypeException();
} 

CClock::clockType getClockType(char* str){
	if(strcmp(str, "mechanic") == 0)
		return CClock::Mechanic;
	
	if(strcmp(str, "quartz") == 0)
		return CClock::Quartz;

	throw WrongClockTypeException();
} 


int main(int argc, char** argv){
	setlocale( LC_ALL,"Russian" );
	
	CTimeStamp* ts = NULL;
	CClock* clock = NULL;

	try{
		switch(argc){
		case 1:
			throw NoArgumentsException();
			break;
		case 2:
			ts = new CTimeStampFromString(argv[1]);
			clock = new CClock(ts, CClock::Mechanic); // Часы по умолчанию
			clock->printAngle(CAngle::Deg);			
			break;
		case 3:
			ts = new CTimeStampFromString(argv[1]);
			clock = new CClock(ts, CClock::Mechanic);
			clock->printAngle(getAngleType(argv[2]));			
			break;
		default:
			ts = new CTimeStampFromString(argv[1]);
			clock = new CClock(ts, getClockType(argv[3]));
			clock->printAngle(getAngleType(argv[2]));
		}
	}catch(std::exception& e){
		printf("%s\n", e.what());
	}


	delete ts;
	delete clock;

	std::cin.get();

	return EXIT_SUCCESS;
}