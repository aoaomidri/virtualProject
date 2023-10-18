#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include"Texture.h"

class TextureManager {
public:
	TextureManager() = default;
	~TextureManager() = default;

	static void Initialize();

	static void Finalize();
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//	シングルトンインスタンスの取得
	static TextureManager* GetInstance();



private:
	/// <summary>
	/// Textureのコンテナ(キー値: ファイルネーム  コンテナデータ型: Texture*)
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;

	
};