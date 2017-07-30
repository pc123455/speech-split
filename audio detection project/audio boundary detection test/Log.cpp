#include "Log.h"


Log::Log(string filename) : fileName(filename) {
}


Log::~Log() {
	ofs.close();
}

bool Log::putLogs(char* logs) {
	if (ofs.is_open()) {
		ofs << logs << std::endl;
		return true;
	}
	return false;
}

bool Log::openLogFile() {
	ofs.open(fileName, std::ios::app);
	if (ofs.is_open()){
		return true;
	}
	return false;
}