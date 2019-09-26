//date   	2019/09/21
//author 	
//describe  file folder operation
#pragma once

#include <string>

namespace file_folder_opt
{
	//check dir is exist
	bool check_directory_is_exist(const std::string _file_dir);
	//create a complete dir 
	void create_file_directory(const std::string _file_dir);

};