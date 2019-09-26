#include "stdafx.h"

namespace common_methods
{
	//get sys time
	INT64 get_system_time()
	{
		struct timeb _otime;
		ftime(&_otime);
		return ((INT64)_otime.time * 1000) + _otime.millitm;
	}

	void char_to_wchar(std::string _src , std::wstring & _dst)
	{
		size_t _len = _src.length();
		int _mlen = MultiByteToWideChar(CP_ACP, 0, _src.c_str(), static_cast<int>(_len + 1), NULL, 0);

		wchar_t * _wbuffer = new wchar_t[_mlen];
		MultiByteToWideChar(CP_ACP, 0, _src.c_str(), static_cast<int>(_len + 1), _wbuffer, _mlen);

		_dst = _wbuffer;
	}
};

