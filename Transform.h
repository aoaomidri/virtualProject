#pragma once
#include"math/Matrix.h"
#include<cstdint>
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct Material{
	Vector4 color;
	std::int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};
