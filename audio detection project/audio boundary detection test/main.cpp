#include "Audio.h"
#include "BoundsDetector.h"
#include "ErrorLog.h"
#include "UserLog.h"
#include <vector>
#include <fstream>
#include <windows.h>
#include <locale>
#include <codecvt>

using namespace std;

//返回边界点数组的长度，若出现异常则返回-1
int getBoundariesLength(string inputPath, string inputFile, string outputPath, string outputFile, int samplerate, int channels, int flags, char* errorInfo){
	//inputPath = "D:\\ServicePath\\ProjectFiles\\新闻频道\\nuotianwang\\";

	//inputFile = "2016092800000004.wav";
	//outputFile = "2016092800000004.wav";
	//outputPath = "F:\\audio\\2016092800000004\\";
	inputPath = "./audio/test/in/";
	outputPath = "./audio/test/out/";
	inputFile = "m5.wav";
	outputFile = "m5.wav";

	samplerate = 16000;
	channels = 1;
	flags = 0;

	try{
		Audio au(inputPath, inputFile, outputPath, outputFile, samplerate, channels, flags);
		au.Transcoding();
		au.Open();
		BoundsDetector bd(&au, flags);
		bd.DetectBounds();
		//bd.FilterBounds();
		//验证边界点序列的有效性
		bd.IsBoundsValid();

		return au.GetBoundaries().size() - 1;
	}
	catch (exception &e){
		strcpy_s(errorInfo, 255, e.what());
		return -1;
	}
}

//返回边界点数组，若出现异常则数组元素均为-1
void getBoundaries(string inputPath, string inputFile, string outputPath, string outputFile, int samplerate, int channels, int flags, char* errorInfo, int len, double bounds[]){
	try{
		Audio au(inputPath, inputFile, outputPath, outputFile, samplerate, channels, flags);
		au.Open();
		BoundsDetector bd(&au, flags);
		bd.DetectBounds();
		//bd.FilterBounds();
		//验证边界点序列的有效性
		bd.IsBoundsValid();
		au.WriteSegmentFiles();
		const vector<double> vec = au.GetBoundaries();
		//对数组进行赋值
		for (int i = 0; i < len; i++){
			bounds[i] = vec[i];
		}
	}
	catch (exception &e){
		memset(bounds, -1, len);
		strcpy_s(errorInfo, 255, e.what());
	}
}

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

string GBKToUTF8(const char* strGBK)
{
	int len = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, strGBK, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}

int main(void)
{	
	//char str[255] = { '\0' };
	//int len = getBoundariesLength("F:\\yuyinsucai\\split_test\\", "m5.wav", "F:\\yuyinsucai\\split_test\\output\\", "m5.wav", 16000, 1, 0, str);
	//double* buffer = new double[len];
	//getBoundaries("F:\\yuyinsucai\\split_test\\", "m5.wav", "F:\\yuyinsucai\\split_test\\output\\", "m5.wav", 16000, 1, 0, str, len, buffer);
	fstream fs("bit", ios::binary | ios::in);
	char *bytes = new char[20];
	memset(bytes, 0, 20);
	fs.read(bytes, 20);
	//for (int i = 0; i < 10; i++) {
	//	swap(bytes[2 * i], bytes[2 * i + 1]);
	//}
	unsigned short *wchars = new unsigned short[10];
	memset(wchars, 0, 20);
	memcpy(wchars, bytes, 20);
	
	wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> cvt;
	string res = cvt.to_bytes(wchars);
	string str = UTF8ToGBK(res.c_str());
	return 0;
}

