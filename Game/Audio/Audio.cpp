#include "Audio.h"

Audio* Audio::GetInstance(){
	static Audio instance;

	return &instance;
}

void Audio::Initialize(){
	//Xaudioのエンジンのインスタンスを生成
	hr_ = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);

	hr_ = xAudio2_->CreateMasteringVoice(&masterVoice_);

}

void Audio::Reset(){
	xAudio2_.Reset();
}

SoundData Audio::SoundLoadWave(const char* fileName){
	//1：ファイルを開く
	std::ifstream file;
	file.open(ResourcesPath + "/" + fileName, std::ios_base::binary);

	assert(file.is_open());

	//2：.wavデータ読み込み
	//RIFFヘッダー読み込み
	RiffHeader riff{};
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt", 3) != 0) {
		assert(0);
	}

	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	//Dataチャンクの読み込み
	ChunkHeader data{};
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id,"JUNK",4) == 0){
		//読み込み位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id,"data",4) != 0){
		assert(0);
	}

	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	//Waveファイルを閉じる
	file.close();

	SoundData soundData{};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

void Audio::SoundUnload(SoundData* soundData){
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Audio::SoundPlayWave(const SoundData& soundData){
	IXAudio2SourceVoice* pSourceVoice = nullptr;

	//波形フォーマットを基にSourceVoiceの生成
	hr_ = this->xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(hr_));
	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	//波形データの再生
	hr_ = pSourceVoice->SubmitSourceBuffer(&buf);
	hr_ = pSourceVoice->Start();

}

