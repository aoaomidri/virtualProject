#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"
#include"Quaternion.h"
#include<cstdint>

struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

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

struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

struct CameraForGPU{
	Vector3 worldPosition;
};
