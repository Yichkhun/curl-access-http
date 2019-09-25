#include "stdafx.h"
#include "curl_syn_access.h"

curl_syn_access::curl_syn_access()
{
}

curl_syn_access::~curl_syn_access()
{
}

_call_back_func curl_syn_access::m_pfunc = nullptr;

bool curl_syn_access::open()
{
	//curl init env not thread safe
	curl_global_init(CURL_GLOBAL_ALL);

	return true;
}

void curl_syn_access::close()
{
	//curl clean not thread safe
	curl_global_cleanup();
}

//call back function
//param1 : points to the returned data
//param2 param3 : returned data size _size * _nmemb
//param3 : this parameter is related to CURLOPT_WRITEDATA - the requested and returned WRITE_DATA is same
size_t syn_call_back_function(char * _json_data, size_t _size, size_t _nmemb, void * _write_data)
{
	//perform call back func
	curl_syn_access::m_pfunc(_json_data, _size, _nmemb, _write_data);

	//important!!! must be return _size * _nmemb, if not will be show CURLE_WRITE_ERROR ******
	return _size * _nmemb;
}

bool curl_syn_access::syn_access_http_data(const std::string _http_url, void * _write_data, _call_back_func _call_func, INT64 _time_out)
{
	//set curl opt
	//you can adjust parameters according to your needs

	//save call back func ptr
	m_pfunc = _call_func ;

	//create curl ptr
	CURL* _curl = curl_easy_init();

	//check curl ptr
	if (nullptr ==_curl) {
		return false;
	}

	//set the request http url
	curl_easy_setopt(_curl, CURLOPT_URL, _http_url.c_str());
	//prevent libcurl make signal interrupt other thread wait or sleep operation
	curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1);	 
	//set time out 
	if (_time_out > 0){
		curl_easy_setopt(_curl, CURLOPT_TIMEOUT_MS, _time_out);
	}	
	//not output head info
	curl_easy_setopt(_curl, CURLOPT_HEADER, 0);
	//set callback func
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, syn_call_back_function);
	//set write data
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, _write_data);

	//curl perform
	CURLcode _res = curl_easy_perform(_curl);

	if (_res != CURLE_OK){
		//get error msg
		m_error_msg = curl_easy_strerror(_res);

		//clear curl
		curl_easy_cleanup(_curl);
		return false;
	}

	//clear curl
	curl_easy_cleanup(_curl);

	return true;
}

std::string & curl_syn_access::get_last_error_msg()
{
	return m_error_msg;
}
