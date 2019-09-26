//date   	2019/09/21
//author 	
//describe  include file
#pragma once

#include <stdio.h>
#include <functional>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include "curl.h"
#include "json.h"
#include "common.h"
#include "multi.h"
#include "file_folder_opt.h"

#ifdef WIN32
#include  <io.h>
#else
#include <unistd.h>
#endif