#pragma once
#include <stdexcept>

//�ļ���ʧ���쳣
class file_open_failed :
	public std::runtime_error{
public:
	explicit file_open_failed(const std::string &s);
	~file_open_failed();
};

class file_close_failed :
	public std::runtime_error{
public:
	explicit file_close_failed(const std::string &s);
	~file_close_failed();
};

//��Ƶ�ļ���ȡ�쳣
class file_read_failed :
	public std::runtime_error{
public:
	explicit file_read_failed(const std::string &s);
	~file_read_failed();
};

//��Ƶ�ļ�д���쳣
class file_write_failed :
	public std::runtime_error{
public:
	explicit file_write_failed(const std::string &s);
	~file_write_failed();
};
