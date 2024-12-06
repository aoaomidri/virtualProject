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
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <codecvt>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
/*音源の読み込み再生を行う「マネージャーを作り、音源の再生のみにする予定」*/

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


	//音源の手動での再生や停止を行って確認をする
	void AudioDebug();


	/// <summary>
	/// 音声読み込み
	/// resouces/Audio/は省略
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	//uint32_t SoundLoadWave(const std::string fileName);

	void SoundUnload(SoundData* soundData);

	/*/// 音声再生
	/// 0が無音、1が原音
	uint32_t SoundPlayWave(uint32_t soundHandle, float volume);*/

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

	uint32_t GetIndexVoice()const {
		return indexVoice_;
	}

	void AddIndexVoice() {
		indexVoice_++;
	}

	void MinusIndexVoice() {
		indexVoice_--;
	}

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
	const std::string ResourcesPath_ = "resources/Audio";

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

public:
	//MediaFoundationでmp3を再生する用の関数、変数
	//関数軍
	

	/// <summary>
	/// ソースリーダーの取得
	/// </summary>
	void MakeSourceReader(const std::string fileName);

	/// <summary>
	/// メディアタイプの作成
	/// </summary>
	void GetMediaType();

	/// <summary>
	/// オーディオデータ形式の作成
	/// </summary>
	void MakeAudioData();

	/// <summary>
	/// データの読み込み
	/// </summary>
	uint32_t LoadAudioData(const std::string fileName);

	/// <summary>
	/// 音楽ファイルを読み込む
	/// </summary>
	/// <param name="fileName">ファイルの名前	○○.mp3もしくは.wav</param>
	/// <returns></returns>
	uint32_t LoadAudio(const std::string fileName);
	
	/// <summary>
	/// 音楽を再生させる
	/// </summary>
	/// <param name="soundHandle">：再生させたいハンドルを指定</param>
	/// <param name="volume">：再生する音楽の音のボリュームを設定</param>
	/// <param name="isLoop">：この曲をループさせるかどうか</param>
	/// <returns></returns>
	uint32_t PlayAudio(uint32_t soundHandle, float volume, bool isLoop);

	/// <summary>
	/// 終了処理
	/// </summary>
	void MediaFinalize();
private:
	//変数群

	IMFSourceReader* pMFSourceReader_{ nullptr };

	IMFMediaType* pMFMediaType_{ nullptr };

	WAVEFORMATEX* waveFormat_{ nullptr };

	int32_t debugNumber_ = 0;
};

