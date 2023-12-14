#pragma once
#include"../../math/Vector2.h"
#include"../../math/Vector3.h"
#include"../../math/Vector4.h"
#include"../../math/Matrix4x4.h"
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

struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};
