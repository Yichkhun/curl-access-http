#include "stdafx.h"
#include "file_folder_opt.h"

namespace file_folder_opt
{
	bool check_directory_is_exist(const std::string _file_dir)
	{
#ifdef WIN32
		WIN32_FIND_DATA  _find_data;
		bool _exist = false;
		//find
		HANDLE _handle = FindFirstFile(_file_dir.c_str(), &_find_data);
		//check
		if ((_handle != INVALID_HANDLE_VALUE) && (_find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			_exist = true;   
		}
		FindClose(_handle);
		return _exist;
#else
		if(access(_file_dir.c_str(), 0) == 0) {
			struct stat fd_buf;
			if(stat( _file_dir.c_str(), &fd_buf ) == 0) {
				if(S_IFDIR & fd_buf.st_mode) {
					return true;
				}
			}
		}
		return false;
#endif
	}

	void create_file_directory(const std::string _file_dir)
	{
		size_t		_index;
		size_t		_next_index;	
		std::string _tmp_file_dir;
		std::string _file_path = _file_dir;

		_index = _file_path.find("/");
		if (_index == std::string::npos){
			return;
		}

		_next_index = _index++;
		while(true) {
			_next_index = _file_path.find("/", _index);

			if(_next_index != std::string::npos) {

				_tmp_file_dir = _file_path.substr(0, _next_index);
				if (check_directory_is_exist(_tmp_file_dir) == false) {
#ifdef WIN32
					CreateDirectory(_tmp_file_dir.c_str(),NULL);
#else
					mkdir(_tmp_file_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
				}
				_next_index++;
				_index = _next_index;
			} else {
				break;
			}
		}

		if (check_directory_is_exist(_file_path) == false) {
#ifdef WIN32
			CreateDirectory(_file_path.c_str(),NULL);
#else
			mkdir(_file_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
		}
	}
}





