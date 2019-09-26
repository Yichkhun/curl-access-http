//date   	2019/09/21
//author 	
//describe  common file
#pragma once
#include <sys/types.h> 
#include <sys/timeb.h>

//syn call back
typedef std::function<size_t(char *, size_t, size_t, void *)> _call_back_func;

//asyn call back
typedef std::function<size_t(char *, size_t, size_t, void *)> _asyn_call_back_func;

struct stru_write_data
{
	int			m_index;

public:
	stru_write_data()
	{
		memset(this, 0, sizeof(stru_write_data));
	}
};

struct stru_date_data
{
	std::string m_year;
	std::string m_month;
	std::string m_day;
	std::string m_hour;
	std::string m_min;
	std::string m_seconds;

public:
	stru_date_data()
	{
		m_year = "";
		m_month = "";
		m_day = "";
		m_hour = "";
		m_min = "";
		m_seconds = "";
	}
};


namespace common_methods
{
	//get sys time
	INT64 get_system_time();

	//character conversion
	void char_to_wchar(std::string _src , std::wstring & _dst);
};