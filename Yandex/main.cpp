/* �������� �������, anatoliy.seleznyov@gmail.com, ����������� �������:

CTimeStamp - ����� ��������� �������. ��������, ����� ������� ������ ��������� 
������� ��� ������ ������� ������.
CTimeStampFromString - ����� �������� ������� ��������� �������, �������� ��������� 
�� �� ������. ��������� ���������������: ����� ������� ����� �������� ������������� 
��� ��������� ������� (��������, �� ��������, �� ��������� ����� � �.�.), �����
����� ������������ �������.

�Anlge - ����� ����. ������������� ����������� ��������. ������ �������� ���� ������ ����.

CClock - ����� ����, ����� ������ �� ��������� �������. ����� �������, ����� ������� 
1 ��������� ������� � "���������" �� ��� ����� �����. ���������� �������������� 1 
��������� �������, � ��� ����, ����������� �� ��� ������������� ����� �����������. 
������ �������� ���� ����� (��� ��� �� ����� 2, � ������ ����� ���������� �����
���, �� ��������� ������� ��� ��� ��������� ��� �������������)
*/
#define _USE_MATH_DEFINES

#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include<exception>
#include<stdlib.h>
#include<iostream>


const double fullCircle = 360.0; // ��������
const double minutesPerDegree = 60.0;
const double secondsPerMinute = 60.0;

///
/// ����� ��������� �������. ����� � 24-������� �������
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
/// ����� ��� �������� ��������� ������� �� ������.
/// ������ ������ ������ ��������������� ������� "HH:MM" ��� "HH:MM AM\PM"
/// ������� A,P,M - ��������� ������� �����. ������ ����������� ������ ��� ���������� 
/// ������� � ������������ AM\PM
///
class CTimeStampFromString : public CTimeStamp{
private:
	// ���������������� ���������
	enum ScanerState{		
		scaningHours, 		// ��������� ����
		scaningMinutes, 	// ��������� ������
		scaningFormat, 		// ��������� AM\PM (���� �����)
		scaningDone			// ������������ ���������
	} state;

	int scaned_symbols;     // ���-�� ��� ���������������� �������� � ������ ��������� 
	char* current_symbol;	// ������� ������

	void scanHour();
	void scanMinute();
	void scanFormat();

public:
	CTimeStampFromString() {};
	CTimeStampFromString(char* time_str);
	~CTimeStampFromString() {};
	void scan(char* time_str);
};

// ��������� ����� �����
void CTimeStampFromString :: scanHour()
{
	// ������ 2 ������� �������� ��� ����� � ����� �����
	if(isdigit(*current_symbol) && scaned_symbols < 2)
	{
		hour = hour*10 + *current_symbol - '0';
		scaned_symbols++;
	}
	else	
		// ���� ��� ��������� 2 ������� � ��������� ':' - �������� ����������� ������
		if(scaned_symbols == 2 && *current_symbol == ':') 
		{
			state = scaningMinutes;
			scaned_symbols = 0;
		}
		else 
			throw std::exception("�������� ������ �������. ������ ���� HH:MM");
	
}

// ��������� ����� �����
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
				throw std::exception("�������� ������ ������������ AM\\PM");
		}
		else 
			throw std::exception("�������� ������ �������. ������ ���� HH:MM");						
	}
}

// ��������� ������ �������
void CTimeStampFromString :: scanFormat()
{		
	if(scaned_symbols == 0)
	{ 
		// ������� ��������� A ��� P
		if(*current_symbol == 'A' || *current_symbol == 'P')
		{
			if(hour > 11)
				throw std::exception("�������� ������. ��� �������� AM\\PM ���� ����������� � ��������� 0..11");

			if(*current_symbol == 'P')
				hour += 12;	

			scaned_symbols++;							
		}
		else
			throw std::exception("�������� ������ ������������ AM\\PM");	
		return;
	}

	if(scaned_symbols == 1)
		// ������� ��������� M
		if(*current_symbol == 'M')
			state = scaningDone;							
		else
			throw std::exception("�������� ������ ������������ AM\\PM");
}


// ������� ������������
void CTimeStampFromString :: scan(char* time_str)
{
	hour = minute = second = 0;

	state = scaningHours;

	current_symbol = time_str;
	scaned_symbols = 0;

	// ���� �� ����� ������ � �� ��������� ������������
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
			case scaningDone: // ���� ������������ ���������, �� �� ����� ������
				throw std::exception("�������� ������. ������ ������� � ����� ������");
		}
		current_symbol++;	
	}
		
	// ���� �������� ���� ���� ������������ ������, ���� ���� ����� ������ "HH:MM"
	if(state != scaningDone)
		// ���� ��������� ������������ � ����� ������ ������, ����� ��� ����� ������� ������ ������� - ������
		if(state != scaningMinutes || scaned_symbols != 2)
			throw std::exception("�������� ������. HH:MM AM\\PM");			
	

	if(hour > 23 || minute > 59)
		throw std::exception("�������� ������. ����� ����� <= 23, ����� <= 59");

}

// ����������� ��������� ������� �� ������
CTimeStampFromString :: CTimeStampFromString(char* time_str)
{
	scan(time_str);
}

///
/// ����� ���� (��� �������� � ���� ��������� ��� ���������� � ��������)
///
class CAngle{
public:
	static enum AngleOutputType { Deg, Rad, Dms }; // ��� ������
	
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

	// ����������� ���� � �������� �� 2 ���������
	void toLower();
	void out(const AngleOutputType type);
	char* toString(const AngleOutputType type);
	
};

CAngle :: CAngle(const CAngle& other){
	value = other.value;
}
CAngle& CAngle :: operator+=(const CAngle& other)
{
	value = value + other.value;
	// ����������, ���������� ����� �� ������ fullCircle
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
	// ����������, ���������� ����� �� ������ fullCircle
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

// ���������� ������� �� 2 ��������� �����
void CAngle :: toLower(){
	if(2.0*value > fullCircle)
		value = fullCircle - value;
}

char* CAngle :: toString(const AngleOutputType type){
	char* temp;
	switch(type){
		case Deg:
			temp = (char*) malloc(sizeof(char)*7);
			sprintf(temp, "%.4lf\n", value);
			break;
		case Rad:
			temp = (char*) malloc(sizeof(char)*7);
			sprintf(temp, "%.4lf\n", value*2.0*M_PI/fullCircle);
			break;
		case Dms:
			temp = (char*) malloc(sizeof(char)*15);
			sprintf(temp, "%d.%02.0lf'%02.0lf\"\n", int(value), 
												(value - int(value))*minutesPerDegree, 
												((value - int(value))*minutesPerDegree 
												- int((value - int(value))*minutesPerDegree))*secondsPerMinute);
	}
	return temp;
}

// ������� ���� � ������������ � �����
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
/// ����� ����
///
class CClock{
public:
	static enum clockType { Mechanic, Quartz };

private:
	// ��������� �� ��������� �������
	CTimeStamp* time;
	clockType type;
	// ���� ������� �� 0 �������� (00:00)
	CAngle hourAngle;
	CAngle minuteAngle;	

public:	
	CClock() {};
	CClock(CTimeStamp* time_stamp, const clockType tp = Mechanic);
	// �� �� ������� ������ �� ������ �� TimeStamp, �.�. � ���� ����������� ���������������,
	// ��� �� ����� ������������ ��������� �����, ����������� �� 1 TimeStamp.
	~CClock() {};
	
	void setClockType(clockType tp){ type = tp; }
	clockType getClockType(){ return type; }
	
	void countAngle();
	CAngle getAngle();
	void printAngle(const CAngle::AngleOutputType type);
	
};

// ��������� ���� ����� ���������
void CClock :: countAngle(){
	hourAngle = CAngle(double(time->getHour()-12)*30.0); // 1 ��� = 30 ��������
	minuteAngle = CAngle(double(time->getMinute())*6.0); // 1 ������ = 360/60 = 6 ��������

	if(type == Mechanic)
		hourAngle += double(time->getMinute())/2.0; // �������� � ����� = minute * (30 ����/60 �����)
}

// ����������� ����� �� ��������� �������
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
/// ��������������� ������� ��� ��������� �����:
///
CAngle::AngleOutputType getAngleType(char* str){
	if(strcmp(str, "deg") == 0)
		return CAngle::Deg;
	
	if(strcmp(str, "rad") == 0)
		return CAngle::Rad;
	
	if(strcmp(str, "dms") == 0)
		return CAngle::Dms;

	throw std::exception("������� ����� ������ ������ ����. (deg\\rad\\dms)");
} 

CClock::clockType getClockType(char* str){
	if(strcmp(str, "mechanic") == 0)
		return CClock::Mechanic;
	
	if(strcmp(str, "quartz") == 0)
		return CClock::Quartz;

	throw std::exception("������� ����� ��� �����. (mechanic\\quartz)");
} 


int main(int argc, char** argv){
	setlocale( LC_ALL,"Russian" );
	
	CTimeStamp* ts = new CTimeStamp();
	CClock* clock = new CClock(ts, CClock::Mechanic); // ���� �� ���������

	try{
		switch(argc){
		case 1:
			throw std::exception("������������ ����������");
			break;
		case 2:
			*ts = CTimeStampFromString(argv[1]);			
			clock->printAngle(CAngle::Deg);			
			break;
		case 3:
			*ts = CTimeStampFromString(argv[1]);			
			clock->printAngle(getAngleType(argv[2]));			
			break;
		default:
			*ts = CTimeStampFromString(argv[1]);
			clock->setClockType(getClockType(argv[3]));
			clock->printAngle(getAngleType(argv[2]));
		}
	}catch(std::exception& e){
		printf("%s\n", e.what());
	}


	delete ts;
	delete clock;

	system("pause");
	return EXIT_SUCCESS;
}