#pragma once
#include <sstream>
#include <Windows.h>

using std::string;
using std::wstring;
using std::stringstream;
using std::wstringstream;

class Common
{
public:
	//数字转字符串函数
	template <typename T>
	static string num2Str(T num){
		stringstream ss;
		ss << num;
		string str;
		ss >> str;
		return str;
	}

	//数字转宽字节字符串函数
	template <typename T>
	static wstring num2Wstr(T num){
		wstringstream ss;
		ss << num;
		wstring str;
		ss >> str;
		return str;
	}

	////取得两数之中较小的数
	//template <typename T>
	//static T min(const T &a, const T &b){
	//	return a < b ? a : b;
	//}

	//字符串转整形
	static int str2Int(string str){
		int num;
		stringstream ss;
		ss << str;
		ss >> num;
		return num;
	}

	//字符串转float
	static float str2Float(string str){
		float num;
		stringstream ss;
		ss << str;
		ss >> num;
		return num;
	}

	//宽字节字符串转float
	static float wstr2Float(wstring str){
		float num;
		wstringstream ss;
		ss << str;
		ss >> num;
		return num;
	}

	//宽字节字符串转整形
	static int wstr2Int(wstring str){
		int num;
		wstringstream ss;
		ss << str;
		ss >> num;
		return num;
	}

	// 安全的取得真实系统信息
	static VOID SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo);

	static int GetSystemBits();

	Common();
	~Common();
};

