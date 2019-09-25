#include "stdafx.h"
#include "curl_syn_access.h"
#include "curl_asyn_access.h"

//get sys time
INT64 get_system_time()
{
	struct timeb _otime;
	ftime(&_otime);
	return ((INT64)_otime.time * 1000) + _otime.millitm;
}

//syn curl call back func
size_t syn_curl_call_back_func(char * _json_data, size_t _size, size_t _nmemb, void * _write_data)
{
	stru_write_data * _data = (stru_write_data *)_write_data;
	std::cout<<"write_data = "<<_data->m_index<<std::endl;

	//parse json string
	//format example {"ret":0,"msg":"OK","data":""}

	try {
		Json::Reader _reader;
		Json::Value _val;

		if (_reader.parse(_json_data, _val)) {

			if (!_val.isMember("ret") || !_val.isMember("msg") || !_val.isMember("data")) {
				return 1;
			}
			std::cout<<_val["msg"].asString().c_str()<<std::endl;
			return _val["ret"].asInt();
		}
	}
	catch(...) {
		std::cout<<"parse json string abnormal"<<std::endl;
	}

	return 1;
}

//asyn curl call back func
size_t asyn_curl_call_back_func(char * _json_data, size_t _size, size_t _nmemb, void * _write_data)
{
	stru_write_data * _data = (stru_write_data *)_write_data;
	std::cout<<"write_data = "<<_data->m_index<<std::endl;

	//remember to free your memory
	delete _data;

	try {
		Json::Reader _reader;
		Json::Value _val;

		if (_reader.parse(_json_data, _val)) {

			if (!_val.isMember("ret") || !_val.isMember("msg") || !_val.isMember("data")) {
				return 1;
			}
			std::cout<<_val["msg"].asString().c_str()<<std::endl;
			return _val["ret"].asInt();
		}
	}
	catch(...) {
		std::cout<<"parse json string abnormal"<<std::endl;
	}

	return 0;
}

int main()
{
	/************************************************
	*perform syn curl access http [9/24/2019 NashC]
	*************************************************/
	std::string _str_syn_http_curl_example = "http://xxx.xxx.xxx/v1/filter/msg?userid=123&token=1234&text=唱跳Rapper";

	curl_syn_access _syn_curl;	
	//syn curl open
	_syn_curl.open();	

	//you can define your struct
	stru_write_data _syn_write_data;
	_syn_write_data.m_index = 10;

	INT64 _syn_begin_time = get_system_time();

	//see 500 times time consuming
	for (int i = 0; i < 500; i++) {
		//syn curl request
		bool _ret = _syn_curl.syn_access_http_data(_str_syn_http_curl_example, &_syn_write_data, syn_curl_call_back_func, 5000);
		if (false == _ret) {
			std::string _error_msg = _syn_curl.get_last_error_msg();
			std::cout<<_error_msg.c_str()<<std::endl;
		}
	}

	INT64 _syn_end_time = get_system_time();
	//time consuming
	std::cout<<"syn get http data pay time = "<<_syn_end_time - _syn_begin_time<<"ms"<<std::endl;

	//syn curl close
	_syn_curl.close();

	std::cout<<"************************************************************"<<std::endl;

	/************************************************
	*perform asyn curn access http [9/24/2019 NashC]
	*************************************************/
	curl_asyn_access _asyn_curl;
	int _max_deal_curl_count = 10;

	//asyn curl open
	if (false == _asyn_curl.open(_max_deal_curl_count)) {
		return 1;
	}

	//add your url
	std::vector<std::string> _url_vec;
	for (int i = 0; i < 30; i++) {
		char _buffer[1024] = { 0 };
		sprintf(_buffer,"http://xxx.xxx.xxx/v1/filter/msg?userid=123&token=1234&text=你好我是数字%d", i);
		std::string _str_url = _buffer;
		_url_vec.push_back(_str_url);
	}	

	//add your define struct
	std::vector<void*> _data_vec;
	for (int j = 0; j < 30; j++) {
		
		stru_write_data * _data = new stru_write_data;
		_data->m_index = j;
		_data_vec.push_back(_data);
	}

	INT64 _asyn_begin_time = get_system_time();

	//concurrent request
	_asyn_curl.asyn_access_http_data(_url_vec, _data_vec, asyn_curl_call_back_func, 5000);

	INT64 _asyn_end_time = get_system_time();

	//time consuming
	std::cout<<"asyn get http data pay time = "<<_asyn_end_time - _asyn_begin_time<<"ms"<<std::endl;

	system("pause");
	return 0;
}