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
	//����ֱ��ƫ��
	CalculateDCOffset();
	//����Ƶ��
	int samplerate = m_audio->GetSampleRate();
	//��̼������λΪ֡
	sf_count_t minInterval = sm_minSilenceLength * samplerate;
	//�����ȣ���λΪ֡
	sf_count_t windowSize = sm_windowTimeLength * samplerate;
	//�Ѷ�ȡ��֡��
	sf_count_t readedFrames = 0;
	//��һ���߽���λ��
	sf_count_t lastBoundaryPosition = 0;
	//��ǰ���������ļ��е�λ��
	sf_count_t curFramePosition = windowSize / 2;
	//��ȡ��������ָ��
	const float* buffer = m_audio->GetBuffer();
	//����������
	double energySum = 0;
	//�������е���Ч֡��
	sf_count_t frames = 0;
	//��0����ӽ��߽���б���
	m_audio->AddBoundary(0);
	do {
		//��ȡ����
		m_audio->Read();
		//ÿ�ζ�ȡ����ֻ����һ�����
		energySum = Sum(buffer, windowSize);
		//��ȡ��Ч֡��
		frames = m_audio->GetFrameCount();
		//��ÿһ�������������е����ݣ������ʱ����
		for (sf_count_t i = 0; i < frames - windowSize + 1; i++){
			if (i > 0){
				//ÿһ֡��ȥֱ��ƫ��
				energySum -= calculateEnergy(buffer[i - 1] - m_dcOffset);
				energySum += calculateEnergy(buffer[i + windowSize - 1] - m_dcOffset);
			}
			curFramePosition++;
			if (energySum < sm_energyThreshold){
				//�����ǰ��λ�ô�����һ���߽���λ�ü�����С��������
				if (curFramePosition > lastBoundaryPosition + minInterval){
					//������һ���߽��λ�õ�ֵ
					lastBoundaryPosition = curFramePosition;
					//����ǰ������������߽��������
					m_audio->AddBoundary(static_cast<double>(curFramePosition) / samplerate);
				}
			}
		}
		//���ļ�ָ��ǰ�ƴ�����
		m_audio->Seek(-windowSize);
	} while (frames >= Audio::sm_maxBufferSize);
	//�����ֳ�������ڱ߽���б����
	m_audio->AddBoundary(static_cast<double>(m_audio->GetAudioInfo().frames) / samplerate);
}

void BoundsDetector::FilterBounds(){
	//�����ı߽��
	const vector<double>& bounds = m_audio->GetBoundaries();
	//�µı߽������
	list<double> newBounds;
	//��0��������߽��������
	newBounds.push_back(0);
	//��һ����ӵı߽��ʱ��
	double lastBoundTime = 0;
	for (int i = 0; i < bounds.size() - 1; i++){
		//���¸��߽����ϴα߽�����������ֵ������Ĵ������ֵ��ֱ�����ǰ��ģ�
		if (bounds[i + 1] - lastBoundTime > sm_maxAudioBlockLength){
			//����ǰ�߽�������µı߽��������
			newBounds.push_back(bounds[i]);
			lastBoundTime = bounds[i];

		//����ǰ�߽����ϴα߽������������ֵ(ǰ����ֱ�����)
		}else if (bounds[i] - lastBoundTime > sm_optimalAudioBlockLength){
			//����ǰ�߽�������µı߽��������
			newBounds.push_back(bounds[i]);
			lastBoundTime = bounds[i];

		//����ǰ�߽����ϴα߽����С������ֵ �� �¸��߽����ϴα߽������������ֵ��ǰС�������Ӻ���ģ�
		}else if (bounds[i] - lastBoundTime < sm_optimalAudioBlockLength && bounds[i + 1] - lastBoundTime > sm_optimalAudioBlockLength){
			//���¸��߽�������߽��������
			newBounds.push_back(bounds[i + 1]);
			lastBoundTime = bounds[i + 1];

		//����ǰ�߽����ϴα߽����С������ֵ �� �¸��߽����ϴα߽����С������ֵ(������С�������)
		}else if (bounds[i] - lastBoundTime < sm_optimalAudioBlockLength && bounds[i + 1] - lastBoundTime < sm_optimalAudioBlockLength){
			continue;
		}
	}
	//����һ����ӵ�ʱ��С����Ƶʱ��
	if (lastBoundTime < bounds[bounds.size() - 1]){
		//����Ƶʱ����ӽ��µı߽������
		newBounds.push_back(bounds[bounds.size() - 1]);
	}
	//��ձ߽������
	m_audio->ClearBoundaries();
	//���µı߽�������е�Ԫ�ظ��Ƹ��߽������
	for (auto it = newBounds.begin(); it != newBounds.end(); it++){
		m_audio->AddBoundary(*it);
	}
}

bool BoundsDetector::IsBoundsValid(){
	//�����ı߽��
	const vector<double>& bounds = m_audio->GetBoundaries();
	//�жϱ߽��ļ���Ƿ��С�������Ƶ�鳤��
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
		//����������
		sum += calculateEnergy(frames[i] - m_dcOffset);
	}
	return sum;
}

void BoundsDetector::CalculateDCOffset(){
	//���ļ�ָ�����õ���ʼλ��
	m_audio->Seek(0, SEEK_SET);
	//�������е���Ч֡��
	sf_count_t frames = 0;
	//��ȡ��������ָ��
	const float* buffer = m_audio->GetBuffer();
	do{
		m_audio->Read();
		//��ȡ��Ч֡��
		frames = m_audio->GetFrameCount();
		//���
		for (int i = 0; i < frames; i++){
			m_dcOffset += buffer[i];
		}
		
	} while (frames >= Audio::sm_maxBufferSize);
	//����֡�������������յ�ֱ��ƫ����
	m_dcOffset /= m_audio->GetAudioInfo().frames;
	//���ļ�ָ�����õ���ʼλ��
	m_audio->Seek(0, SEEK_SET);
}

void BoundsDetector::ReadProfile(){
	CSimpleIni ini;
	ini.SetUnicode();
	ini.LoadFile(sm_configFileName.c_str());
	//��ȡ������
	sm_windowTimeLength = Common::wstr2Float(ini.GetValue(L"parameters", L"windowSize", L"0.2"));
	//��ȡ������ֵ
	sm_energyThreshold = Common::wstr2Float(ini.GetValue(L"parameters", L"energyThreshold", L"0.5"));
	//��ȡ֡�Ƴ���
	sm_frameOffsetLength = Common::wstr2Float(ini.GetValue(L"parameters", L"frameOffsetLength", L"1"));
	//��ȡ��̾�������
	sm_minSilenceLength = Common::wstr2Float(ini.GetValue(L"parameters", L"minSilienceLength", L"1"));
	//��ȡ������Ƶ�鳤��
	sm_optimalAudioBlockLength = Common::wstr2Float(ini.GetValue(L"parameters", L"optimalAudioBlockLength", L"50"));
	//��ȡ�����Ƶ�쳤��
	sm_maxAudioBlockLength = Common::wstr2Float(ini.GetValue(L"parameters", L"maxAudioBlockLength", L"60"));
}

void BoundsDetector::WriteProfile(){
	CSimpleIni ini;
	ini.SetUnicode();
	ini.LoadFile(sm_configFileName.c_str());
	//���ô�����
	ini.SetValue(L"parameters", L"windowSize", Common::num2Wstr(sm_windowTimeLength).c_str());
	//���þ�����ֵ
	ini.SetValue(L"parameters", L"energyThreshold", Common::num2Wstr(sm_energyThreshold).c_str());
	//����֡�Ƴ���
	ini.SetValue(L"parameters", L"frameOffsetLength", Common::num2Wstr(sm_frameOffsetLength).c_str());
	//������̾�������
	ini.SetValue(L"parameters", L"minSilienceLength", Common::num2Wstr(sm_minSilenceLength).c_str());
	//����������Ƶ�鳤��
	ini.SetValue(L"parameters", L"optimalAudioBlockLength", Common::num2Wstr(sm_optimalAudioBlockLength).c_str());
	//���������Ƶ�쳤��
	ini.SetValue(L"parameters", L"maxAudioBlockLength", Common::num2Wstr(sm_maxAudioBlockLength).c_str());
	//д���ļ�
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
