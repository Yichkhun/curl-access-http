#include "stdafx.h"
#include "curl_asyn_access.h"

curl_asyn_access::curl_asyn_access()
	:m_curlm(nullptr)
{
}

curl_asyn_access::~curl_asyn_access()
{
}

_asyn_call_back_func curl_asyn_access::m_asyn_call_func = nullptr;

bool curl_asyn_access::open(int _max_deal_url_count)
{
	//init global env
	curl_global_init(CURL_GLOBAL_ALL);

	//create curlm
	m_curlm = curl_multi_init();

	//check
	if (nullptr == m_curlm) {
		std::cout<<"create curlm failed"<<std::endl;
		return false;
	}

	//set deal url count
	if (CURLM_OK != curl_multi_setopt(m_curlm, CURLMOPT_MAXCONNECTS, _max_deal_url_count)) {
		return false;
	}

	return true;
}

void curl_asyn_access::close()
{
	//clear multi curl
	curl_multi_cleanup(m_curlm);

	//curl clean not thread safe
	curl_global_cleanup();
}

bool curl_asyn_access::asyn_access_http_data(const std::vector<std::string> _http_url_vec, const std::vector<void *> _write_data_vec, _asyn_call_back_func _call_func, INT64 _time_out)
{
	m_asyn_call_func = _call_func;

	int _size = _http_url_vec.size();

	//create easy curl and add to curlmulti
	for (int i = 0; i < _size; i++) {
		//create easy curl
		init(_http_url_vec[i], _write_data_vec[i], _time_out);
	}

	return perform_get_http_data();
}

bool curl_asyn_access::perform_get_http_data()
{
	//identify have curl needs to be processed
	int _still_running = 0;

	//deal
	do 
	{
		CURLMcode _mc;
		int		  _numfds;

		//param1 CURLM
		//param2 the rest to be processed curl
		_mc = curl_multi_perform(m_curlm, &_still_running);

		if (_mc == CURLM_OK) {
			// wait for activity
			_mc = curl_multi_wait(m_curlm, NULL, 0, 1000, &_numfds);
		}

		if (_mc != CURLM_OK) {
			std::cout<<curl_multi_strerror(_mc)<<std::endl;
			return false;
		}

		//if there is no _numfds please sleep 100ms 
		if (!_numfds) {
			Sleep(100);
		}

	} while (_still_running);

	//deal read
	CURLMsg * _curl_msg = nullptr;
	do 
	{
		int _msg_queue = 0;

		_curl_msg = curl_multi_info_read(m_curlm, &_msg_queue);
		//check
		if (_curl_msg && _curl_msg->msg == CURLMSG_DONE) {
			//
			if (_curl_msg->data.result != CURLE_OK) {
				//query error info
				char *_url = NULL;
				curl_easy_getinfo(_curl_msg->easy_handle,  CURLINFO_EFFECTIVE_URL, &_url);				
				std::cout<<"http :"<<_url<<"- error info: "<<curl_easy_strerror(_curl_msg->data.result)<<std::endl;
			}

			//remove the relation  mcurl and easy curl
			curl_multi_remove_handle(m_curlm, _curl_msg->easy_handle);
			//clear easy curl
			curl_easy_cleanup(_curl_msg->easy_handle);
		}

	} while (_curl_msg);

	return true;
}

//call back function
//param1 : points to the returned data
//param2 param3 : returned data size _size * _nmemb
//param3 : this parameter is related to CURLOPT_WRITEDATA - the requested and returned WRITE_DATA is same
size_t asyn_call_back_function(char * _json_data, size_t _size, size_t _nmemb, void * _write_data)
{
	//call back the upper level
	curl_asyn_access::m_asyn_call_func(_json_data, _size, _nmemb, _write_data);

	//important!!! must be return _size * _nmemb, if not will be show CURLE_WRITE_ERROR ******
	return _size * _nmemb;
}

void curl_asyn_access::init(const std::string _http_url, void * _write_data, INT64 _time_out)
{
	CURL * _curl = curl_easy_init();

	//check curl ptr
	if (nullptr ==_curl) {
		return ;
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
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, asyn_call_back_function);
	//set write data
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, _write_data);

	//add every easy curl to curlm
	CURLMcode _mc = curl_multi_add_handle(m_curlm, _curl);

	//check
	if (_mc != CURLM_OK) {
		std::cout<<"multi add easy curl failed http url :"<<_http_url.c_str()<<std::endl;
		//clear _curl
		curl_easy_cleanup(_curl);
	}
}