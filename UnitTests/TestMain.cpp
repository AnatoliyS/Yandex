#include <cstdlib>
#include "gtest/gtest.h"

// ���, ����� ������������ main � ����������� ���������
#define main MainInTestedFile
#include "../../Yandex/main.cpp"
#define main main


class TimeStampFromStringTest : public ::testing::Test {
public:
	TimeStampFromStringTest() {};
protected:
	CTimeStampFromString tsfs;
};

TEST_F(TimeStampFromStringTest, format24_basicTest1) 
{
	tsfs.scan("12:00");
	ASSERT_TRUE(tsfs.getHour() == 12 && tsfs.getMinute() == 0);
}

TEST_F(TimeStampFromStringTest, format24_basicTest2) 
{
	tsfs.scan("15:15");
	ASSERT_TRUE(tsfs.getHour() == 15 && tsfs.getMinute() == 15);
}

TEST_F(TimeStampFromStringTest, format24_basicTest3) 
{
	tsfs.scan("23:59");
	ASSERT_TRUE(tsfs.getHour() == 23 && tsfs.getMinute() == 59);
}

TEST_F(TimeStampFromStringTest, format24_exception_empty) 
{
	ASSERT_THROW(tsfs.scan(""), std::exception);	
}

TEST_F(TimeStampFromStringTest, format24_exception_less_digit_in_hour) 
{	
	ASSERT_THROW(tsfs.scan("3:59"), std::exception);	
}

TEST_F(TimeStampFromStringTest, format24_exception_hour_more_23) 
{	
	ASSERT_THROW(tsfs.scan("24:59"), std::exception);	
}

TEST_F(TimeStampFromStringTest, format24_exception_minute_more_59) 
{	
	ASSERT_THROW(tsfs.scan("22:60"), std::exception);	
}

TEST_F(TimeStampFromStringTest, format24_exception_whitespace_1) 
{	
	ASSERT_THROW(tsfs.scan("13 :59"), std::exception);	
}

TEST_F(TimeStampFromStringTest, format24_exception_whitespace_2) 
{	
	ASSERT_THROW(tsfs.scan("13: 59"), std::exception);	
}

TEST_F(TimeStampFromStringTest, format24_exception_whitespace_3) 
{	
	ASSERT_THROW(tsfs.scan("1 3: 59"), std::exception);	
}

TEST_F(TimeStampFromStringTest, format12_exception_modifier_one_char) 
{	
	ASSERT_THROW(tsfs.scan("10:11 A"), std::exception);	
}

TEST_F(TimeStampFromStringTest, format12_exception_hour_more_than_11) 
{	
	ASSERT_THROW(tsfs.scan("13:59 AM"), std::exception);	
}

TEST_F(TimeStampFromStringTest, format12_exception_wrong_modifier)
{	
	ASSERT_THROW(tsfs.scan("13:59 AD"), std::exception);	
}

class ClockTest : public ::testing::Test {
public:
	ClockTest(): times(CTimeStamp(0,0)), c(CClock(&times,CClock::Mechanic)) {};
protected:
	CClock c;
	CTimeStamp times;
};



TEST_F(ClockTest, BasicCountAngleTest1) 
{
	times = CTimeStamp(12, 0);
	c.countAngle();
    ASSERT_TRUE(c.getAngle() == CAngle(0));
}

TEST_F(ClockTest, BasicCountAngleTest2) 
{
	times = CTimeStamp(15, 15);
	c.countAngle();
    ASSERT_TRUE(c.getAngle() == CAngle(7.5));
}

TEST_F(ClockTest, LimitCountAngleTest1) 
{
	times = CTimeStamp(23, 59);
	c.countAngle();
    ASSERT_TRUE(c.getAngle() == CAngle(5.5));
}

TEST_F(ClockTest, LimitCountAngleTest2) 
{
	times = CTimeStamp(0, 0);
	c.countAngle();
    ASSERT_TRUE(c.getAngle() == CAngle(0));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	system("pause");
    return 0;
	
}
