#include "Audio.h"
#include "Common.h"
#include "file_open_failed.h"

const sf_count_t  Audio::sm_maxBufferSize = 65535;

Audio::Audio(string inputPath, string inputFile, string outputPath, string outputFile, int sampleFrequency, int channels, int flags) :
m_inputFileName(inputFile), 
m_inputPath(inputPath), 
m_outputFileName(outputFile), 
m_outputPath(outputPath), 
m_outputSampleRate(sampleFrequency), 
m_channel(channels), 
m_flags(flags), 
m_file_ptr(NULL),
m_curPos(0),
m_buffers(NULL){
}

void Audio::Open(){
	try{
		string filePath = m_outputPath + m_outputFileName;
		m_file_ptr = sf_open(filePath.c_str(), SFM_READ, &m_sfinfo);
		if (m_file_ptr == NULL){
			throw file_open_failed(sf_strerror(m_file_ptr));
		}
		//�ڴ��ļ�ʱ��buffer�����ڴ�
		if (m_file_ptr != NULL && m_buffers == NULL){
			m_buffers = new float[sm_maxBufferSize];
		}
	}
	//�������ڴ�����쳣��ֱ���׳�
	catch (std::bad_alloc &e){
		throw e;
	}
}

void Audio::Transcoding(){
	string inputFilePath = m_inputPath + m_inputFileName;
	string outputFilePath = m_outputPath + m_outputFileName;
	string cmd("ffmpeg -i ");
	if (Common::GetSystemBits() == 32){
		cmd = "ffmpeg32 -i ";
	}
	else{
		cmd = "ffmpeg64 -i ";
	}
	
	cmd += inputFilePath;
	//����Ŀ�����Ƶ��
	cmd += " -ar ";
	cmd += Common::num2Str(m_outputSampleRate);
	//����Ŀ��������
	cmd += " -ac 1";
	cmd += " -f wav ";
	//��������ļ�·��
	cmd += outputFilePath;
	system(cmd.c_str());
}

void Audio::Seek(sf_count_t frames, int whence){
	m_curPos = sf_seek(m_file_ptr, frames, whence);
	//��seek����ʱ�׳��쳣
	if (m_curPos == -1){
		throw file_read_failed(sf_strerror(m_file_ptr));
	}
}

void Audio::Read(){
	m_frameCount = sf_read_float(m_file_ptr, m_buffers, sm_maxBufferSize);
	//�ļ���ȡʧ��ʱ�׳��쳣
	if (m_frameCount == 0){
		throw file_read_failed(sf_strerror(m_file_ptr));
	}
}

const vector<double>& Audio::GetBoundaries(){
	return m_boundaries;
}

void Audio::AddBoundary(double bound){
	m_boundaries.push_back(bound);
}

void Audio::ClearBoundaries(){
	m_boundaries.clear();
}

void Audio::WriteSegmentFiles(){
	//m_boundaries.push_back(static_cast<double>(m_sfinfo.frames) / m_sfinfo.samplerate);
	for (int i = 0; i < m_boundaries.size() - 1; i++){
		WriteSegmentFile(m_boundaries[i], m_boundaries[i + 1], i);
	}
}

void Audio::WriteSegmentFile(double start, double end, int id){
	//��������ļ���
	string filePath = m_outputPath + m_outputFileName;
	string idStr = Common::num2Str(id);
	filePath = filePath.substr(0, filePath.size() - 4);
	filePath += "_";
	filePath += idStr;
	filePath += ".pcm";

	//���㿪ʼʱ���ڲ������е�λ��
	sf_count_t posStart = start * m_outputSampleRate;
	//������Ҫд���֡����
	sf_count_t totalFrames = (end - start) * m_outputSampleRate;

	//���������Ϣ
	SF_INFO info;
	//��������ΪĿ��������
	info.channels = m_channel;
	//���ò�����ΪĿ�������
	info.samplerate = m_outputSampleRate;
	//���ø�ʽΪĿ��pcm������
	info.format = SF_FORMAT_RAW | SF_FORMAT_PCM_16;

	//���ļ�
	SNDFILE* file_ptr = sf_open(filePath.c_str(), SFM_WRITE, &info);
	//����ļ���ʧ�����׳��쳣
	if (file_ptr == NULL){
		throw file_open_failed(sf_strerror(file_ptr));
	}

	//�����ļ�ָ���λ�õ���ʼʱ��
	Seek(posStart, SEEK_SET);
	do{
		//��ȡ�ļ�����󻺳������ȸ�֡
		Read();
		//���ļ���д��min(totalFrames, m_frameCount)��֡
		int writeCount = sf_writef_float(file_ptr, m_buffers, min(totalFrames, m_frameCount));
		//���ļ�д��ʧ�����׳��쳣
		if (writeCount == 0){
			throw file_write_failed(sf_strerror(file_ptr));
		}
		//����ʣ��δ��ȡ��֡��
		totalFrames -= m_frameCount;
	} while (totalFrames > 0);
	//�ļ�д����ɺ�ر��ļ�ָ��
	sf_close(file_ptr);
}

int Audio::Close(){
	//�ļ��ر�ʧ��ʱ�׳��쳣
	if (sf_close(m_file_ptr) != 0){
		throw file_close_failed(sf_strerror(m_file_ptr));
	}
	return 0;
}

const float* Audio::GetBuffer() const{
	return m_buffers;
}

int Audio::GetSampleRate(){
	return m_sfinfo.samplerate;
}

sf_count_t Audio::GetFrameCount(){
	return m_frameCount;
}

const SF_INFO& Audio::GetAudioInfo(){
	return m_sfinfo;
}

Audio::~Audio(){
	Close();
	delete[] m_buffers;
}
