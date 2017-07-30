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
	//����ת�ַ�������
	template <typename T>
	static string num2Str(T num){
		stringstream ss;
		ss << num;
		string str;
		ss >> str;
		return str;
	}

	//����ת���ֽ��ַ�������
	template <typename T>
	static wstring num2Wstr(T num){
		wstringstream ss;
		ss << num;
		wstring str;
		ss >> str;
		return str;
	}

	////ȡ������֮�н�С����
	//template <typename T>
	//static T min(const T &a, const T &b){
	//	return a < b ? a : b;
	//}

	//�ַ���ת����
	static int str2Int(string str){
		int num;
		stringstream ss;
		ss << str;
		ss >> num;
		return num;
	}

	//�ַ���תfloat
	static float str2Float(string str){
		float num;
		stringstream ss;
		ss << str;
		ss >> num;
		return num;
	}

	//���ֽ��ַ���תfloat
	static float wstr2Float(wstring str){
		float num;
		wstringstream ss;
		ss << str;
		ss >> num;
		return num;
	}

	//���ֽ��ַ���ת����
	static int wstr2Int(wstring str){
		int num;
		wstringstream ss;
		ss << str;
		ss >> num;
		return num;
	}

	// ��ȫ��ȡ����ʵϵͳ��Ϣ
	static VOID SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo);

	static int GetSystemBits();

	Common();
	~Common();
};

