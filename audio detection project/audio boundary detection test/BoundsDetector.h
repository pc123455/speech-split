#pragma once
#include "Audio.h"

class BoundsDetector
{
	//Audio��ָ��
	Audio* m_audio;
	//ֱ��ƫ��
	double m_dcOffset;
	//һЩ��ʶλ
	int m_flags;
	//��������������ĺ�����Ϊ��ͨ���ԣ�ʹ�ú���ָ����д洢
	double (*calculateEnergy)(const float &frame);
	//��ʱ�䳤�ȣ���λΪ�룩
	static float sm_windowTimeLength;
	//������ֵ
	static float sm_energyThreshold;
	//֡�Ƴ��ȣ���λΪ֡��
	static int sm_frameOffsetLength;
	//��С�������ȣ���λΪ�룩
	static float sm_minSilenceLength;
	//������Ƶ�鳤�ȣ���λΪ�룩
	static float sm_optimalAudioBlockLength;
	//�����Ƶ�쳤�ȣ���λΪ�룩
	static float sm_maxAudioBlockLength;
	//�����ļ��ļ���
	static string sm_configFileName;

	static const int sm_isReadProfile;
public:
	//�߽���⺯��
	void DetectBounds();
	//�߽����˺���
	void FilterBounds();
	//�����Ƶ���Ƿ�Ϸ�
	bool IsBoundsValid();
	//����������������
	double Sum(const float* frames, sf_count_t windowSize);
	//����ֱ��ƫ��
	void CalculateDCOffset();
	//��ȡ�����ļ�
	static void ReadProfile();
	//д�����ļ�
	static void WriteProfile();
	BoundsDetector(Audio* au, int flags, double(*cal)(const float &frame) = [](const float &frame){ return static_cast<double>(frame * frame); });
	~BoundsDetector();
};

