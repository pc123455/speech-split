#pragma once
#include "Log.h"
class ErrorLog :
	public Log
{
public:
	ErrorLog(string filename);
	~ErrorLog();
};

