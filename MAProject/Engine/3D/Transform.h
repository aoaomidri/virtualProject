#pragma once
#include"Matrix4x4.h"
#include"Quaternion.h"
#include<cstdint>

struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	bool operator==(const VertexData& other) const {
		return position.x == other.position.x && position.y == other.position.y && position.z == other.position.z && position.w == other.position.w &&
			normal.x == other.normal.x && normal.y == other.normal.y && normal.z == other.normal.z &&
			texcoord.x == other.texcoord.x && texcoord.y == other.texcoord.y;
	}
};
// VertexData 用のハッシュ関数を定義
namespace std {
	template <>
	struct hash<VertexData> {
		size_t operator()(const VertexData& data) const {
			size_t h1 = hash<float>()(data.position.x) ^ (hash<float>()(data.position.y) << 1);
			size_t h2 = hash<float>()(data.position.z) ^ (hash<float>()(data.position.w) << 1);
			size_t h3 = hash<float>()(data.normal.x) ^ (hash<float>()(data.normal.y) << 1);
			size_t h4 = hash<float>()(data.normal.z);
			size_t h5 = hash<float>()(data.texcoord.x) ^ (hash<float>()(data.texcoord.y) << 1);

			return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
		}
	};
}

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
};

struct CameraForGPU{
	Vector3 worldPosition;
};
