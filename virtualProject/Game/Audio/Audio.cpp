#include "Audio.h"
#include <cassert>

Audio* Audio::GetInstance(){
	static Audio instance;

	return &instance;
}

void Audio::XAudio2VoiceCallback::OnBufferEnd(THIS_ void* pBufferContext) {

	Voice* voice = reinterpret_cast<Voice*>(pBufferContext);
	// 再生リストから除外
	Audio::GetInstance()->voices_.erase(voice);
}

void Audio::Initialize(){
	//Xaudioのエンジンのインスタンスを生成
	hr_ = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr_));
	hr_ = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr_));

	hr_ = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr_));

	hr_ = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	assert(SUCCEEDED(hr_));

	indexSoundData_ = 0u;
	indexVoice_ = 0u;
}

void Audio::Finalize(){
	xAudio2_.Reset();
	MediaFinalize();

	for (auto& soundData : soundDatas_){
		SoundUnload(&soundData);
	}

}

//uint32_t Audio::SoundLoadWave(const std::string fileName){
//	assert(indexSoundData_ < kMaxSoundData);
//	uint32_t handle = indexSoundData_;
//
//	// 読み込み済みサウンドデータを検索
//	auto it = std::find_if(soundDatas_.begin(), soundDatas_.end(), [&](const auto& soundData) {
//		return soundData.name_ == fileName;
//		});
//	if (it != soundDatas_.end()) {
//		// 読み込み済みサウンドデータの要素番号を取得
//		handle = static_cast<uint32_t>(std::distance(soundDatas_.begin(), it));
//		return handle;
//	}
//
//	//1：ファイルを開く
//	std::ifstream file;
//	file.open(ResourcesPath + "/" + fileName, std::ios_base::binary);
//
//	assert(file.is_open());
//
//	//2：.wavデータ読み込み
//	//RIFFヘッダー読み込み
//	RiffHeader riff{};
//	file.read((char*)&riff, sizeof(riff));
//	//ファイルがRIFFかチェック
//	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
//		assert(0);
//	}
//	//タイプがWAVEかチェック
//	if (strncmp(riff.type, "WAVE", 4) != 0) {
//		assert(0);
//	}
//
//	//Formatチャンクの読み込み
//	FormatChunk format = {};
//	//チャンクヘッダーの確認
//	file.read((char*)&format, sizeof(ChunkHeader));
//	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
//		assert(0);
//	}
//
//	//チャンク本体の読み込み
//	assert(format.chunk.size <= sizeof(format.fmt));
//	file.read((char*)&format.fmt, format.chunk.size);
//
//	//Dataチャンクの読み込み
//	ChunkHeader data{};
//	file.read((char*)&data, sizeof(data));
//	//JUNKチャンクを検出した場合
//	if (strncmp(data.id,"JUNK",4) == 0){
//		//読み込み位置をJUNKチャンクの終わりまで進める
//		file.seekg(data.size, std::ios_base::cur);
//		//再読み込み
//		file.read((char*)&data, sizeof(data));
//	}
//
//	while (strncmp(data.id,"data",4) != 0){
//		//読み込み位置をJUNKチャンクの終わりまで進める
//		file.seekg(data.size, std::ios_base::cur);
//		if (file.eof()){
//			assert(0);
//		}
//		//再読み込み
//		file.read((char*)&data, sizeof(data));
//	}
//
//	//Dataチャンクのデータ部(波形データ)の読み込み
//	char* pBuffer = new char[data.size];
//	file.read(pBuffer, data.size);
//	//Waveファイルを閉じる
//	file.close();
//
//	SoundData soundData = soundDatas_.at(handle);
//
//	soundData.wfex = format.fmt;
//	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
//	soundData.bufferSize = data.size;
//	soundData.name_ = fileName;
//
//	soundDatas_[indexSoundData_] = soundData;
//
//	indexSoundData_++;
//
//	return handle;
//}

void Audio::SoundUnload(SoundData* soundData){
	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

//uint32_t Audio::SoundPlayWave(uint32_t soundHandle, float volume){
//	assert(soundHandle <= soundDatas_.size());
//
//	if (IsPlaying(soundHandle)){
//		return soundHandle;
//	}
//
//	//参照を取得
//	SoundData& soundData = soundDatas_.at(soundHandle);
//	//未ロードだった場合止める
//	assert(soundData.bufferSize != 0);
//
//	uint32_t playingHandle = indexVoice_;
//
//	IXAudio2SourceVoice* pSourceVoice = nullptr;
//
//	//波形フォーマットを基にSourceVoiceの生成
//	hr_ = this->xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
//	assert(SUCCEEDED(hr_));
//
//	//再生中データの生成
//	Voice* voice = new Voice();
//	voice->handle = playingHandle;
//	voice->sourceVoice = pSourceVoice;
//	//再生中データの登録
//	voices_.insert(voice);
//
//	//再生する波形データの設定
//	XAUDIO2_BUFFER buf{};
//	buf.pAudioData = soundData.pBuffer;
//	buf.pContext = voice;
//	buf.AudioBytes = soundData.bufferSize;
//	buf.Flags = XAUDIO2_END_OF_STREAM;
//	// 無限ループ
//	buf.LoopCount = XAUDIO2_LOOP_INFINITE;
//
//	//波形データの再生
//	hr_ = pSourceVoice->SubmitSourceBuffer(&buf);
//	pSourceVoice->SetVolume(volume);
//	hr_ = pSourceVoice->Start();
//
//	indexVoice_++;
//
//	return playingHandle;
//}

void Audio::RePlayWave(uint32_t soundHandle){
	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) { return voice->handle == soundHandle; });
	// 発見
	if (it != voices_.end()) {
		(*it)->sourceVoice->Stop(0);
		(*it)->sourceVoice->FlushSourceBuffers();
		/*if ((*it)->handle!=soundHandle){
			assert(true);
		}*/
		//参照を取得
		SoundData& soundData = soundDatas_.at(soundHandle);
		//未ロードだった場合止める
		assert(soundData.bufferSize != 0);
		//再生する波形データの設定
		XAUDIO2_BUFFER buf{};
		buf.pAudioData = soundData.pBuffer;
		buf.pContext = (*it);
		buf.AudioBytes = soundData.bufferSize;
		buf.Flags = XAUDIO2_END_OF_STREAM;

		hr_ = (*it)->sourceVoice->SubmitSourceBuffer(&buf);

		(*it)->sourceVoice->Start(0);
	}
}


void Audio::StopWave(uint32_t voiceHandle){
	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) { return voice->handle == voiceHandle; });
	// 発見
	if (it != voices_.end()) {
		(*it)->sourceVoice->DestroyVoice();
		MinusIndexVoice();
		voices_.erase(it);
	}
}

bool Audio::IsPlaying(uint32_t voiceHandle)
{
	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) { return voice->handle == voiceHandle; });
	// 発見。再生終わってるのかどうかを判断
	if (it != voices_.end()) {
		XAUDIO2_VOICE_STATE state{};
		(*it)->sourceVoice->GetState(&state);
		return state.SamplesPlayed != 0;
	}
	return false;
}

void Audio::PauseWave(uint32_t voiceHandle){
	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) { return voice->handle == voiceHandle; });
	// 発見
	if (it != voices_.end()) {
		(*it)->sourceVoice->Stop(0);
	}
}

void Audio::ResumeWave(uint32_t voiceHandle){
	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) { return voice->handle == voiceHandle; });
	// 発見
	if (it != voices_.end()) {

		// 再生位置を指定して再生を再開
		(*it)->sourceVoice->Start(0);

	}
}

void Audio::SetVolume(uint32_t voiceHandle, float volume){
	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) { return voice->handle == voiceHandle; });
	// 発見
	if (it != voices_.end()) {
		(*it)->sourceVoice->SetVolume(volume);
	}
}

uint32_t Audio::LoadAudio(const std::string fileName){
	MakeSourceReader(fileName);

	GetMediaType();
	MakeAudioData();	

	return LoadAudioData(fileName);;
}

void Audio::MakeSourceReader(const std::string fileName){
	std::string st = (ResourcesPath + "/" + fileName);
	std::wstring wst;
	wst.assign(st.begin(), st.end());

	MFCreateSourceReaderFromURL(wst.c_str(), NULL, &pMFSourceReader_);

}

void Audio::GetMediaType(){
	MFCreateMediaType(&pMFMediaType_);

	pMFMediaType_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType_->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	pMFSourceReader_->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, pMFMediaType_);

	pMFMediaType_->Release();
	pMFMediaType_ = nullptr;
	pMFSourceReader_->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &pMFMediaType_);
}

void Audio::MakeAudioData(){
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType_, &waveFormat_, nullptr);
}

uint32_t Audio::LoadAudioData(const std::string fileName){
	assert(indexSoundData_ < kMaxSoundData);
	uint32_t handle = indexSoundData_;

	// 読み込み済みサウンドデータを検索
	auto it = std::find_if(soundDatas_.begin(), soundDatas_.end(), [&](const auto& soundData) {
		return soundData.name_ == (fileName);
		});
	if (it != soundDatas_.end()) {
		// 読み込み済みサウンドデータの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(soundDatas_.begin(), it));
		return handle;
	}

	BYTE* pBuffer_{ nullptr };

	std::vector<BYTE> mediaData_;
	while (true){
		IMFSample* pMFSample_{ nullptr };
		DWORD dwStreamFlags_{ 0 };
		pMFSourceReader_->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &dwStreamFlags_, nullptr, &pMFSample_);

		if (dwStreamFlags_ & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			break;
		}
		IMFMediaBuffer* pMFMediaBuffer_{ nullptr };

		pMFSample_->ConvertToContiguousBuffer(&pMFMediaBuffer_);

		DWORD cbCurrentLength_{ 0 };
		pMFMediaBuffer_->Lock(&pBuffer_, nullptr, &cbCurrentLength_);

		mediaData_.resize(mediaData_.size() + cbCurrentLength_);
		memcpy(mediaData_.data() + mediaData_.size() - cbCurrentLength_, pBuffer_, cbCurrentLength_);

		pMFMediaBuffer_->Unlock();

		pMFMediaBuffer_->Release();
		pMFSample_->Release();
	}
	pBuffer_ = new BYTE[mediaData_.size()];
	std::memcpy(pBuffer_, mediaData_.data(), mediaData_.size());

	SoundData soundData = soundDatas_.at(handle);
	
	soundData.wfex = (*waveFormat_);
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer_);
	soundData.bufferSize = sizeof(BYTE) * static_cast<UINT32>(mediaData_.size());
	soundData.name_ = (fileName);

	soundDatas_[indexSoundData_] = soundData;

	indexSoundData_++;

	return handle;
}

uint32_t Audio::PlayAudio(uint32_t soundHandle, float volume, bool isLoop) {
	assert(soundHandle <= soundDatas_.size());

	if (IsPlaying(soundHandle)) {
		return soundHandle;
	}

	//参照を取得
	SoundData& soundData = soundDatas_.at(soundHandle);
	//未ロードだった場合止める
	assert(soundData.bufferSize != 0);

	uint32_t playingHandle = GetIndexVoice();

	IXAudio2SourceVoice* pSourceVoice = nullptr;

	//波形フォーマットを基にSourceVoiceの生成
	hr_ = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(hr_));

	//再生中データの生成
	Voice* voice = new Voice();
	voice->handle = playingHandle;
	voice->sourceVoice = pSourceVoice;
	//再生中データの登録
	voices_.insert(voice);

	XAUDIO2_BUFFER buffer{ 0 };
	buffer.pAudioData = soundData.pBuffer;
	buffer.pContext = voice;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = soundData.bufferSize;
	if (isLoop){
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	else {
		buffer.LoopCount = 0;
	}

	hr_ = pSourceVoice->SubmitSourceBuffer(&buffer);
	pSourceVoice->SetVolume(volume);
	hr_ = pSourceVoice->Start();

	AddIndexVoice();

	return playingHandle;
}

void Audio::MediaFinalize(){
	CoTaskMemFree(waveFormat_);
	pMFMediaType_->Release();
	pMFSourceReader_->Release();
	MFShutdown();
}
