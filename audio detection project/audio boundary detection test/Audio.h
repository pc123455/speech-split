#pragma once
#include "sndfile.h"
#include <vector>

using std::string;
using std::vector;

class Audio
{
	//文件指针
	SNDFILE* m_file_ptr;
	//文件信息
	SF_INFO m_sfinfo;
	//输出采样频率
	int m_outputSampleRate;
	//声道数
	int m_channel;
	//缓冲区
	float* m_buffers;
	//缓冲区数据长度
	sf_count_t  m_frameCount;
	//当前位置
	sf_count_t  m_curPos;
	//边界点列表
	vector<double> m_boundaries;
	//输入文件路径
	string m_inputPath;
	//输入文件名
	string m_inputFileName;
	//输出文件路径
	string m_outputPath;
	//输出文件名
	string m_outputFileName;
	//一些标志位
	unsigned int m_flags;
public:
	//最大缓冲区长度
	static const sf_count_t  sm_maxBufferSize;

	//构造函数
	Audio(string inputPath, string inputFile, string outputPath, string outputFile, int sampleFrequency, int channels, int flags);
	//打开音频文件
	void Open();
	//转码
	void Transcoding();
	//寻找到指定位置， 默认从当前位置进行偏移
	void Seek(sf_count_t frames, int whence = SEEK_CUR);
	//读取数据至缓冲区
	void Read();
	//返回所有边界点
	const vector<double>& GetBoundaries();
	//添加一个边界点至末尾
	void AddBoundary(double bound);
	//清除所有边界点
	void ClearBoundaries();
	//写所有的边界文件
	void WriteSegmentFiles();
	//根据开始与结束时间将一段数据写入边界文件
	void WriteSegmentFile(double start, double end, int id);
	/*关闭音频文件
	   关闭成功返回0，否则返回其他数值*/
	int Close();
	//返回buffer的首地址
	const float* GetBuffer() const;
	//返回采样频率
	int GetSampleRate();
	//返回读取至内存的帧数
	sf_count_t GetFrameCount();
	//返回音频信息
	const SF_INFO& GetAudioInfo();
	~Audio();
};

