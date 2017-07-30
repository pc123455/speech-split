#include "function.h"
#include <fstream>
#include <codecvt>
#include <windows.h>
#include <locale>

using namespace std;

string UTF8ToGBK(const char* strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}


int __stdcall getlen(char* inputPath) {
	fstream fs("debug_test", std::ios::binary | std::ios::in | ios::app);
	int len = strlen(inputPath);
	//unsigned short* wchars = new unsigned short[len];
	//memset(wchars, 0, len * 2);
	////for (int i = 0; i < len; i++) {
	////	swap(inputPath[2 * i], inputPath[2 * i + 1]);
	////}
	//memcpy(wchars, inputPath, len * 2);
	//wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> cvt;
	//string res = cvt.to_bytes(wchars);
	//string str = UTF8ToGBK(res.c_str());
	 
	//fs.write(inputPath, len * 2);
	fs.write(inputPath, len);
	fs.close();
	return 1;
}

