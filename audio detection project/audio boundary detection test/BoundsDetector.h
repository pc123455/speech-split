#pragma once
#include "Audio.h"

class BoundsDetector
{
	//Audio类指针
	Audio* m_audio;
	//直流偏移
	double m_dcOffset;
	//一些标识位
	int m_flags;
	//计算采样点能量的函数，为了通用性，使用函数指针进行存储
	double (*calculateEnergy)(const float &frame);
	//窗时间长度（单位为秒）
	static float sm_windowTimeLength;
	//能量阈值
	static float sm_energyThreshold;
	//帧移长度（单位为帧）
	static int sm_frameOffsetLength;
	//最小静音长度（单位为秒）
	static float sm_minSilenceLength;
	//最优音频块长度（单位为秒）
	static float sm_optimalAudioBlockLength;
	//最大音频快长度（单位为秒）
	static float sm_maxAudioBlockLength;
	//配置文件文件名
	static string sm_configFileName;

	static const int sm_isReadProfile;
public:
	//边界点检测函数
	void DetectBounds();
	//边界点过滤函数
	void FilterBounds();
	//检查音频块是否合法
	bool IsBoundsValid();
	//计算样本点能量和
	double Sum(const float* frames, sf_count_t windowSize);
	//计算直流偏移
	void CalculateDCOffset();
	//读取配置文件
	static void ReadProfile();
	//写配置文件
	static void WriteProfile();
	BoundsDetector(Audio* au, int flags, double(*cal)(const float &frame) = [](const float &frame){ return static_cast<double>(frame * frame); });
	~BoundsDetector();
};

