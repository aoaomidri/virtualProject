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
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
};

struct CameraForGPU{
	Vector3 worldPosition;
};
