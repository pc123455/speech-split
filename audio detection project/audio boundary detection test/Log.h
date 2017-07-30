#pragma once
#include <fstream>

using std::string;
using std::fstream;

class Log
{
	//日志文件名
	string fileName;
protected:
	//日志文件流
	fstream ofs;
public:
	Log(string filename);
	virtual bool openLogFile();
	virtual bool putLogs(char* logs);
	virtual ~Log();
};

