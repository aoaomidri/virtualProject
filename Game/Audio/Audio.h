#pragma once
#include<xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include<fstream>
#include<wrl.h>
#include <cassert>
#include<vector>
#include <array>
#include <cstdint>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>


class Audio{
public:
	// サウンドデータの最大数
	static const int kMaxSoundData = 256;

	struct SoundData {
		//波形フォーマット
		WAVEFORMATEX wfex;
		//バッファの先頭アドレス
		BYTE* pBuffer;
		//バッファのサイズ
		unsigned int bufferSize;
		//名前
		std::string name_;
	};

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

	// 再生データ
	struct Voice {
		uint32_t handle = 0u;
		IXAudio2SourceVoice* sourceVoice = nullptr;
	};

	/// <summary>
	/// オーディオコールバック
	/// </summary>
	class XAudio2VoiceCallback : public IXAudio2VoiceCallback {
	public:
		// ボイス処理パスの開始時
		STDMETHOD_(void, OnVoiceProcessingPassStart)
			([[maybe_unused]] THIS_ UINT32 BytesRequired) {};
		// ボイス処理パスの終了時
		STDMETHOD_(void, OnVoiceProcessingPassEnd)(THIS) {};
		// バッファストリームの再生が終了した時
		STDMETHOD_(void, OnStreamEnd)(THIS) {};
		// バッファの使用開始時
		STDMETHOD_(void, OnBufferStart)([[maybe_unused]] THIS_ void* pBufferContext) {};
		// バッファの末尾に達した時
		STDMETHOD_(void, OnBufferEnd)(THIS_ void* pBufferContext);
		// 再生がループ位置に達した時
		STDMETHOD_(void, OnLoopEnd)([[maybe_unused]] THIS_ void* pBufferContext) {};
		// ボイスの実行エラー時
		STDMETHOD_(void, OnVoiceError)
			([[maybe_unused]] THIS_ void* pBufferContext, [[maybe_unused]] HRESULT Error) {};
	};

	static Audio* GetInstance();

	void Initialize();

	void Finalize();


	/// <summary>
	/// 音声読み込み
	/// resouces/Audio/は省略
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	uint32_t SoundLoadWave(const char* fileName);

	void SoundUnload(SoundData* soundData);

	/// 音声再生
	/// 0が無音、1が原音
	uint32_t SoundPlayWave(uint32_t soundHandle, float volume);

	//最初から再生
	void RePlayWave(uint32_t soundHandle);
	 
	/// 音声停止
	void StopWave(uint32_t voiceHandle);

	/// 音声再生中かどうか
	bool IsPlaying(uint32_t voiceHandle);

	/// 音声一時停止
	void PauseWave(uint32_t voiceHandle);

	/// 音声一時停止からの再開
	void ResumeWave(uint32_t voiceHandle);

	/// <summary>
	/// 音量設定
	/// </summary>
	/// <param name="voiceHandle">再生ハンドル</param>
	/// <param name="volume">ボリューム
	/// 0で無音、1がデフォルト音量</param>
	void SetVolume(uint32_t voiceHandle, float volume);
private:
	Audio() = default;
	~Audio() = default;
	Audio(const Audio&) = delete;
	const Audio& operator=(const Audio&) = delete;

	//インクルードパス
	const std::string ResourcesPath = "resources/Audio";

	HRESULT hr_;
	//インスタンス
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	//マスターボイス
	IXAudio2MasteringVoice* masterVoice_;
	//サウンドデータコンテナ
	std::array<SoundData, kMaxSoundData> soundDatas_;
	std::set<Voice*> voices_;
	// サウンド格納ディレクトリ
	std::string directoryPath_;
	// 次に使うサウンドデータの番号
	uint32_t indexSoundData_ = 0u;
	// 次に使う再生中データの番号
	uint32_t indexVoice_ = 0u;
	// オーディオコールバック
	XAudio2VoiceCallback voiceCallback_;
	std::mutex voiceMutex_;


};

