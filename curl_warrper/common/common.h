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