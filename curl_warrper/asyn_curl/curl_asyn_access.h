//date   	2019/09/21
//author 	
//describe  curl asyn access http 
#pragma once
#include <vector>
#include <string>

class curl_asyn_access
{
public:
	curl_asyn_access();
	~curl_asyn_access();

public:
	//open
	bool open(int _max_deal_url_count);
	//close
	void close();
	//init create esay curl
	void init(const std::string _http_url, void * _write_data, INT64 _time_out);

public:
	//asyn get http data
	bool asyn_access_http_data(const std::vector<std::string> _http_url_vec, const std::vector<void *> _write_data_vec, _asyn_call_back_func _call_func, INT64 _time_out = 0);

private:
	//perform get http data
	bool perform_get_http_data();

public:
	static _asyn_call_back_func						m_asyn_call_func;

private:
	CURLM*											m_curlm;
	
};