#pragma once
#include"nlohmann/json.hpp"
#include"Vector3.h"
#include"Matrix4x4.h"

class BrendLoader{
private:
	BrendLoader() = default;
	~BrendLoader() = default;
	BrendLoader(const BrendLoader& brend) = delete;
	BrendLoader& operator=(const BrendLoader&) = delete;

public:

	static BrendLoader* GetInstance() {
		static BrendLoader instance;
		return &instance;
	}

	void LoadFile();

public:
	using json = nlohmann::json;


};

