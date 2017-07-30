#include "BoundsDetector.h"
#include "Audio.h"
#include "Common.h"
#include "SimpleIni.h"
#include <list>
#include <stdexcept>

using std::list;
using std::runtime_error;

float BoundsDetector::sm_windowTimeLength = 0.2;

float BoundsDetector::sm_energyThreshold = 0.5;

int BoundsDetector::sm_frameOffsetLength = 1;

float BoundsDetector::sm_minSilenceLength = 1;

float BoundsDetector::sm_optimalAudioBlockLength = 20;

float BoundsDetector::sm_maxAudioBlockLength = 50;

string BoundsDetector::sm_configFileName = "configure.ini";

const int BoundsDetector::sm_isReadProfile = 1;

void BoundsDetector::DetectBounds(){
	//计算直流偏移
	CalculateDCOffset();
	//采样频率
	int samplerate = m_audio->GetSampleRate();
	//最短间隔，单位为帧
	sf_count_t minInterval = sm_minSilenceLength * samplerate;
	//窗长度，单位为帧
	sf_count_t windowSize = sm_windowTimeLength * samplerate;
	//已读取的帧数
	sf_count_t readedFrames = 0;
	//上一个边界点的位置
	sf_count_t lastBoundaryPosition = 0;
	//当前采样点在文件中的位置
	sf_count_t curFramePosition = windowSize / 2;
	//获取到缓冲区指针
	const float* buffer = m_audio->GetBuffer();
	//计算能量和
	double energySum = 0;
	//缓冲区中的有效帧数
	sf_count_t frames = 0;
	//将0点添加进边界点列表中
	m_audio->AddBoundary(0);
	do {
		//读取数据
		m_audio->Read();
		//每次读取数据只进行一次求和
		energySum = Sum(buffer, windowSize);
		//获取有效帧数
		frames = m_audio->GetFrameCount();
		//对每一个缓冲区中所有的数据，计算短时能量
		for (sf_count_t i = 0; i < frames - windowSize + 1; i++){
			if (i > 0){
				//每一帧减去直流偏移
				energySum -= calculateEnergy(buffer[i - 1] - m_dcOffset);
				energySum += calculateEnergy(buffer[i + windowSize - 1] - m_dcOffset);
			}
			curFramePosition++;
			if (energySum < sm_energyThreshold){
				//如果当前的位置大于上一个边界点的位置加上最小静音长度
				if (curFramePosition > lastBoundaryPosition + minInterval){
					//更新上一个边界点位置的值
					lastBoundaryPosition = curFramePosition;
					//将当前采样点添加至边界点序列中
					m_audio->AddBoundary(static_cast<double>(curFramePosition) / samplerate);
				}
			}
		}
		//将文件指针前移窗长度
		m_audio->Seek(-windowSize);
	} while (frames >= Audio::sm_maxBufferSize);
	//将音乐长度添加在边界点列表最后
	m_audio->AddBoundary(static_cast<double>(m_audio->GetAudioInfo().frames) / samplerate);
}

void BoundsDetector::FilterBounds(){
	//检测出的边界点
	const vector<double>& bounds = m_audio->GetBoundaries();
	//新的边界点链表
	list<double> newBounds;
	//将0点添加至边界点链表中
	newBounds.push_back(0);
	//上一个添加的边界点时间
	double lastBoundTime = 0;
	for (int i = 0; i < bounds.size() - 1; i++){
		//若下个边界与上次边界相减大于最大值（后面的大于最大值则直接添加前面的）
		if (bounds[i + 1] - lastBoundTime > sm_maxAudioBlockLength){
			//将当前边界添加至新的边界点链表中
			newBounds.push_back(bounds[i]);
			lastBoundTime = bounds[i];

		//若当前边界与上次边界相减大于最优值(前大则直接添加)
		}else if (bounds[i] - lastBoundTime > sm_optimalAudioBlockLength){
			//将当前边界添加至新的边界点链表中
			newBounds.push_back(bounds[i]);
			lastBoundTime = bounds[i];

		//若当前边界与上次边界相减小于最优值 且 下个边界与上次边界相减大于最优值（前小后大则添加后面的）
		}else if (bounds[i] - lastBoundTime < sm_optimalAudioBlockLength && bounds[i + 1] - lastBoundTime > sm_optimalAudioBlockLength){
			//将下个边界添加至边界点链表中
			newBounds.push_back(bounds[i + 1]);
			lastBoundTime = bounds[i + 1];

		//若当前边界与上次边界相减小于最优值 且 下个边界与上次边界相减小于最优值(两个都小于则继续)
		}else if (bounds[i] - lastBoundTime < sm_optimalAudioBlockLength && bounds[i + 1] - lastBoundTime < sm_optimalAudioBlockLength){
			continue;
		}
	}
	//若上一个添加的时间小于音频时长
	if (lastBoundTime < bounds[bounds.size() - 1]){
		//则将音频时长添加进新的边界点链表
		newBounds.push_back(bounds[bounds.size() - 1]);
	}
	//清空边界点数组
	m_audio->ClearBoundaries();
	//将新的边界点链表中的元素复制给边界点数组
	for (auto it = newBounds.begin(); it != newBounds.end(); it++){
		m_audio->AddBoundary(*it);
	}
}

bool BoundsDetector::IsBoundsValid(){
	//检测出的边界点
	const vector<double>& bounds = m_audio->GetBoundaries();
	//判断边界点的间隔是否均小于最大音频块长度
	for (int i = 0; i < bounds.size() - 1; i++){
		if (bounds[i + 1] - bounds[i]>sm_maxAudioBlockLength){
			throw runtime_error("audio block length invalid!");
			return false;
		}
	}
	return true;
}

double BoundsDetector::Sum(const float* frames, sf_count_t windowSize){
	double sum = 0.0;
	for (int i = 0; i < windowSize; i++){
		//计算能量和
		sum += calculateEnergy(frames[i] - m_dcOffset);
	}
	return sum;
}

void BoundsDetector::CalculateDCOffset(){
	//将文件指针设置到起始位置
	m_audio->Seek(0, SEEK_SET);
	//缓冲区中的有效帧数
	sf_count_t frames = 0;
	//获取到缓冲区指针
	const float* buffer = m_audio->GetBuffer();
	do{
		m_audio->Read();
		//获取有效帧数
		frames = m_audio->GetFrameCount();
		//求和
		for (int i = 0; i < frames; i++){
			m_dcOffset += buffer[i];
		}
		
	} while (frames >= Audio::sm_maxBufferSize);
	//与总帧数相除计算出最终的直流偏移量
	m_dcOffset /= m_audio->GetAudioInfo().frames;
	//将文件指针设置到起始位置
	m_audio->Seek(0, SEEK_SET);
}

void BoundsDetector::ReadProfile(){
	CSimpleIni ini;
	ini.SetUnicode();
	ini.LoadFile(sm_configFileName.c_str());
	//读取窗长度
	sm_windowTimeLength = Common::wstr2Float(ini.GetValue(L"parameters", L"windowSize", L"0.2"));
	//读取静音阈值
	sm_energyThreshold = Common::wstr2Float(ini.GetValue(L"parameters", L"energyThreshold", L"0.5"));
	//读取帧移长度
	sm_frameOffsetLength = Common::wstr2Float(ini.GetValue(L"parameters", L"frameOffsetLength", L"1"));
	//读取最短静音长度
	sm_minSilenceLength = Common::wstr2Float(ini.GetValue(L"parameters", L"minSilienceLength", L"1"));
	//读取最优音频块长度
	sm_optimalAudioBlockLength = Common::wstr2Float(ini.GetValue(L"parameters", L"optimalAudioBlockLength", L"50"));
	//读取最大音频快长度
	sm_maxAudioBlockLength = Common::wstr2Float(ini.GetValue(L"parameters", L"maxAudioBlockLength", L"60"));
}

void BoundsDetector::WriteProfile(){
	CSimpleIni ini;
	ini.SetUnicode();
	ini.LoadFile(sm_configFileName.c_str());
	//设置窗长度
	ini.SetValue(L"parameters", L"windowSize", Common::num2Wstr(sm_windowTimeLength).c_str());
	//设置静音阈值
	ini.SetValue(L"parameters", L"energyThreshold", Common::num2Wstr(sm_energyThreshold).c_str());
	//设置帧移长度
	ini.SetValue(L"parameters", L"frameOffsetLength", Common::num2Wstr(sm_frameOffsetLength).c_str());
	//设置最短静音长度
	ini.SetValue(L"parameters", L"minSilienceLength", Common::num2Wstr(sm_minSilenceLength).c_str());
	//设置最优音频块长度
	ini.SetValue(L"parameters", L"optimalAudioBlockLength", Common::num2Wstr(sm_optimalAudioBlockLength).c_str());
	//设置最大音频快长度
	ini.SetValue(L"parameters", L"maxAudioBlockLength", Common::num2Wstr(sm_maxAudioBlockLength).c_str());
	//写入文件
	ini.SaveFile(sm_configFileName.c_str());
}

BoundsDetector::BoundsDetector(Audio* au, int flags, double(*cal)(const float &frame)) :
m_audio(au),
m_flags(flags),
calculateEnergy(cal),
m_dcOffset(0){
	if (sm_isReadProfile & m_flags != 0){
		ReadProfile();
	}
}

BoundsDetector::~BoundsDetector(){
}
