#pragma once
#include "Log.h"
class UserLog :
	public Log
{
public:
	UserLog(string filename);
	~UserLog();
};

