#include "file_open_failed.h"


file_open_failed::file_open_failed(const std::string &s) :std::runtime_error(s){}


file_open_failed::~file_open_failed(){}

file_close_failed::file_close_failed(const std::string &s) : std::runtime_error(s){}

file_close_failed::~file_close_failed(){}

file_read_failed::file_read_failed(const std::string &s) :std::runtime_error(s){}

file_read_failed::~file_read_failed(){}

file_write_failed::file_write_failed(const std::string &s) :std::runtime_error(s){}

file_write_failed::~file_write_failed(){}
