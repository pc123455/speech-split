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
		//在打开文件时对buffer分配内存
		if (m_file_ptr != NULL && m_buffers == NULL){
			m_buffers = new float[sm_maxBufferSize];
		}
	}
	//若捕获到内存分配异常则直接抛出
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
	//设置目标采样频率
	cmd += " -ar ";
	cmd += Common::num2Str(m_outputSampleRate);
	//设置目标声道数
	cmd += " -ac 1";
	cmd += " -f wav ";
	//设置输出文件路径
	cmd += outputFilePath;
	system(cmd.c_str());
}

void Audio::Seek(sf_count_t frames, int whence){
	m_curPos = sf_seek(m_file_ptr, frames, whence);
	//当seek出错时抛出异常
	if (m_curPos == -1){
		throw file_read_failed(sf_strerror(m_file_ptr));
	}
}

void Audio::Read(){
	m_frameCount = sf_read_float(m_file_ptr, m_buffers, sm_maxBufferSize);
	//文件读取失败时抛出异常
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
	//设置输出文件名
	string filePath = m_outputPath + m_outputFileName;
	string idStr = Common::num2Str(id);
	filePath = filePath.substr(0, filePath.size() - 4);
	filePath += "_";
	filePath += idStr;
	filePath += ".pcm";

	//计算开始时间在采样点中的位置
	sf_count_t posStart = start * m_outputSampleRate;
	//计算需要写入的帧总数
	sf_count_t totalFrames = (end - start) * m_outputSampleRate;

	//设置输出信息
	SF_INFO info;
	//设置声道为目标声道数
	info.channels = m_channel;
	//设置采样率为目标采样率
	info.samplerate = m_outputSampleRate;
	//设置格式为目标pcm裸数据
	info.format = SF_FORMAT_RAW | SF_FORMAT_PCM_16;

	//打开文件
	SNDFILE* file_ptr = sf_open(filePath.c_str(), SFM_WRITE, &info);
	//如果文件打开失败则抛出异常
	if (file_ptr == NULL){
		throw file_open_failed(sf_strerror(file_ptr));
	}

	//设置文件指针的位置到开始时间
	Seek(posStart, SEEK_SET);
	do{
		//读取文件中最大缓冲区长度个帧
		Read();
		//在文件中写入min(totalFrames, m_frameCount)个帧
		int writeCount = sf_writef_float(file_ptr, m_buffers, min(totalFrames, m_frameCount));
		//若文件写入失败则抛出异常
		if (writeCount == 0){
			throw file_write_failed(sf_strerror(file_ptr));
		}
		//计算剩余未读取的帧数
		totalFrames -= m_frameCount;
	} while (totalFrames > 0);
	//文件写入完成后关闭文件指针
	sf_close(file_ptr);
}

int Audio::Close(){
	//文件关闭失败时抛出异常
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
