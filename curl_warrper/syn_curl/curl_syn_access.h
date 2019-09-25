//date   	2019/09/21
//author 	
//describe  curl syn access http
#pragma once

#include <string>
#include "common.h"

class curl_syn_access
{
public:
	curl_syn_access();
	~curl_syn_access();

public:
	//init
	bool open();
	//uninit
	void close();

public:
	//syn get http data
	bool syn_access_http_data(const std::string _http_url, void * _write_data, _call_back_func _call_func, INT64 _time_out = 0);
	//get curl last error message
	std::string & get_last_error_msg();

public:
	static _call_back_func							m_pfunc;

private:
	std::string										m_error_msg;
};

