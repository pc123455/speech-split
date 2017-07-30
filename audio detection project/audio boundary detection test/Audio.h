#pragma once
#include "sndfile.h"
#include <vector>

using std::string;
using std::vector;

class Audio
{
	//�ļ�ָ��
	SNDFILE* m_file_ptr;
	//�ļ���Ϣ
	SF_INFO m_sfinfo;
	//�������Ƶ��
	int m_outputSampleRate;
	//������
	int m_channel;
	//������
	float* m_buffers;
	//���������ݳ���
	sf_count_t  m_frameCount;
	//��ǰλ��
	sf_count_t  m_curPos;
	//�߽���б�
	vector<double> m_boundaries;
	//�����ļ�·��
	string m_inputPath;
	//�����ļ���
	string m_inputFileName;
	//����ļ�·��
	string m_outputPath;
	//����ļ���
	string m_outputFileName;
	//һЩ��־λ
	unsigned int m_flags;
public:
	//��󻺳�������
	static const sf_count_t  sm_maxBufferSize;

	//���캯��
	Audio(string inputPath, string inputFile, string outputPath, string outputFile, int sampleFrequency, int channels, int flags);
	//����Ƶ�ļ�
	void Open();
	//ת��
	void Transcoding();
	//Ѱ�ҵ�ָ��λ�ã� Ĭ�ϴӵ�ǰλ�ý���ƫ��
	void Seek(sf_count_t frames, int whence = SEEK_CUR);
	//��ȡ������������
	void Read();
	//�������б߽��
	const vector<double>& GetBoundaries();
	//���һ���߽����ĩβ
	void AddBoundary(double bound);
	//������б߽��
	void ClearBoundaries();
	//д���еı߽��ļ�
	void WriteSegmentFiles();
	//���ݿ�ʼ�����ʱ�佫һ������д��߽��ļ�
	void WriteSegmentFile(double start, double end, int id);
	/*�ر���Ƶ�ļ�
	   �رճɹ�����0�����򷵻�������ֵ*/
	int Close();
	//����buffer���׵�ַ
	const float* GetBuffer() const;
	//���ز���Ƶ��
	int GetSampleRate();
	//���ض�ȡ���ڴ��֡��
	sf_count_t GetFrameCount();
	//������Ƶ��Ϣ
	const SF_INFO& GetAudioInfo();
	~Audio();
};

