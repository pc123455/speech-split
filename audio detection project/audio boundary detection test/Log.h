#pragma once
#include <fstream>

using std::string;
using std::fstream;

class Log
{
	//��־�ļ���
	string fileName;
protected:
	//��־�ļ���
	fstream ofs;
public:
	Log(string filename);
	virtual bool openLogFile();
	virtual bool putLogs(char* logs);
	virtual ~Log();
};

