#pragma once
#include<xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include<fstream>
#include<wrl.h>
#include <cassert>
#include<vector>

struct SoundData {
	//波形フォーマット
	WAVEFORMATEX wfex;
	//バッファの先頭アドレス
	BYTE* pBuffer;
	//バッファのサイズ
	unsigned int bufferSize;
};

class Audio{
public:
	// サウンドデータの最大数
	static const int kMaxSoundData = 128;

	//チャンクヘッダ
	struct ChunkHeader {
		char id[4];  //チャンク毎のID
		int32_t size;//チャンクサイズ
	};

	//RIFFヘッドチャンク
	struct RiffHeader {
		ChunkHeader chunk;//"RIFF"
		char type[4];	  //"WAVE"
	};

	//FNTチャンク
	struct FormatChunk {
		ChunkHeader chunk;//"fmt"
		WAVEFORMATEX fmt; //波形フォーマット
	};	

	static Audio* GetInstance();

	void Initialize();

	void Reset();

	SoundData SoundLoadWave(const char* fileName);

	void SoundUnload(SoundData* soundData);

	void SoundPlayWave(const SoundData& soundData);
	 

private:
	Audio() = default;
	~Audio() = default;
	Audio(const Audio&) = delete;
	const Audio& operator=(const Audio&) = delete;

	const std::string ResourcesPath = "resources/Audio";

	HRESULT hr_;

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;


};

