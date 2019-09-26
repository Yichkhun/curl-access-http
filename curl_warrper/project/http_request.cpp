#include "stdafx.h"
#include "curl_syn_access.h"
#include "curl_asyn_access.h"
#include <urlmon.h>
#include <tchar.h>

#pragma comment(lib,"urlmon.lib")


std::string make_file_folder(INT64 _room_id)
{
	stru_date_data _date;
	time_t t = time(NULL);

	//init data
	strftime((char *)_date.m_year.c_str(), sizeof(_date.m_day) - 1,  "%Y", localtime(&t));
	strftime((char *)_date.m_month.c_str(), sizeof(_date.m_month) - 1,  "%m", localtime(&t));
	strftime((char *)_date.m_day.c_str(), sizeof(_date.m_day) - 1,  "%d", localtime(&t));

	char _buffer[256] = {0};
	sprintf(_buffer, "D:/watch_wall/%s/%s/%s/room_id_%d", _date.m_year.c_str(), _date.m_month.c_str(), _date.m_day.c_str(), _room_id);

	std::string _result = _buffer;

	return _result;
}

std::string make_jpg_file(const std::string  _down_dir, INT64 _room_id)
{
	stru_date_data _date;
	time_t t = time(NULL);

	strftime((char *)_date.m_hour.c_str(), sizeof(_date.m_hour) - 1,  "%H", localtime(&t));
	strftime((char *)_date.m_min.c_str(), sizeof(_date.m_min) - 1,  "%M", localtime(&t));
	strftime((char *)_date.m_seconds.c_str(), sizeof(_date.m_seconds) - 1, "%S", localtime(&t));

	char _jpg_path[256] = {0};
	sprintf(_jpg_path, "%s/roomid_%d_%s-%s-%s.jpg", _down_dir.c_str(), _room_id, _date.m_hour.c_str(), _date.m_min.c_str(), _date.m_seconds.c_str());

	std::string _jpg_file = _jpg_path;

	return _jpg_file;
}


//syn curl call back func
size_t syn_curl_call_back_func(char * _json_data, size_t _size, size_t _nmemb, void * _write_data)
{
	//key roomid , value pic url 
	std::unordered_map<INT64, std::string> _pic_url;
	std::unordered_map<std::string, std::string> _fail_url_map;

	//stru_write_data * _data = (stru_write_data *)_write_data;
	//std::cout<<"write_data = "<<_data->m_index<<std::endl;

	//json exp:[{"roomId":"1451040","pics":["http://xxx.xxx.xxx/qxiu/90161380.jpg"]}]

	//parse json string
	try {
		Json::Reader _readers;
		Json::Value _vals;

		std::cout<<_json_data<<std::endl;
		_readers.parse(_json_data, _vals);
		int _json_size = _vals.size();

		//traverse array
		for (int i = 0; i < _json_size; i++) {
			
			Json::Value _val = _vals[i];	
			int _room_id = atoi(_val["roomId"].asString().c_str());
			//maybe array, if struct is array you should traverse array, this is a example
			std::string _http_url = _val["pics"][0].asString();
			//save 
			_pic_url[_room_id] = _http_url;
		}
	}
	catch(...) {
		std::cout<<"parse json string abnormal"<<std::endl;
	}

	//down pic
	if (!_pic_url.empty())
	{
		//traverse unordered_map
		auto pfn = [&](std::unordered_map<INT64, std::string>::const_reference  pair)
		{
			std::wstring _wurl;
			common_methods::char_to_wchar(pair.second, _wurl);

			std::string _down_dir =  make_file_folder(pair.first);

			if (false == file_folder_opt::check_directory_is_exist(_down_dir)) {
				file_folder_opt::create_file_directory(_down_dir);
			}

			std::string _jpg_path = make_jpg_file(_down_dir, pair.first);

			//syn down pic, maybe slow
			HRESULT hr = URLDownloadToFile(NULL, pair.second.c_str(), _jpg_path.c_str(), 0, NULL);
			if (hr != S_OK) {
				std::cout<<"down  pic failed first time jpg_path = "<<_jpg_path.c_str()<<std::endl;
				std::cout<<"down  pic failed first time url ="<<pair.second.c_str()<<std::endl;

				_fail_url_map[_jpg_path] = pair.second;
			}
		};
		std::for_each(_pic_url.begin(), _pic_url.end(), pfn);
	}

	//repeat get pic
	if (!_fail_url_map.empty()) {
		for (auto iter = _fail_url_map.begin(); iter != _fail_url_map.end(); iter++) {

			//repeat down pic
			HRESULT hr = URLDownloadToFile(NULL, iter->second.c_str(), iter->first.c_str(), 0, NULL);
			if (hr != S_OK) {
				std::cout <<"down  pic failed second time jpg_path = "<<iter->first.c_str()<<std::endl;
				std::cout <<"down  pic failed second time url = "<<iter->second.c_str()<<std::endl;
			}
		}
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
	std::string _str_syn_http_curl_example = "http://xxx.xxx.xxx/xxx/list";

	curl_syn_access _syn_curl;	
	//syn curl open
	_syn_curl.open();	

	//you can define your struct
	stru_write_data _syn_write_data;
	_syn_write_data.m_index = 10;

	INT64 _syn_begin_time = common_methods::get_system_time();

	//see 500 times time consuming
	for (; ;) {
		//syn curl request
		bool _ret = _syn_curl.syn_access_http_data(_str_syn_http_curl_example, NULL /*_syn_write_data*/, syn_curl_call_back_func, 5000);
		if (false == _ret) {
			std::string _error_msg = _syn_curl.get_last_error_msg();
			std::cout<<_error_msg.c_str()<<std::endl;
		}

		//sleep 20s
		Sleep(20 * 1000);
		std::cout<<"The program is still running !!!!!!!!!!"<<std::endl;
	}

	INT64 _syn_end_time = common_methods::get_system_time();
	//time consuming
	std::cout<<"syn get http data pay time = "<<_syn_end_time - _syn_begin_time<<"ms"<<std::endl;

	//syn curl close
	_syn_curl.close();

	std::cout<<"************************************************************"<<std::endl;

	system("pause");
	return 0;

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

	INT64 _asyn_begin_time = common_methods::get_system_time();

	//concurrent request
	_asyn_curl.asyn_access_http_data(_url_vec, _data_vec, asyn_curl_call_back_func, 5000);

	INT64 _asyn_end_time = common_methods::get_system_time();

	//time consuming
	std::cout<<"asyn get http data pay time = "<<_asyn_end_time - _asyn_begin_time<<"ms"<<std::endl;
}