#pragma once
#include "PerlinNoise.h"

class GameObject
{
public:
	GameObject(float scale, int divisions);
	~GameObject();

	float sizeQuad = 1.f;
	int divisions = 20;
	DirectX::SimpleMath::Vector3 initPos = DirectX::SimpleMath::Vector3::Zero;

	std::vector<DirectX::VertexPositionNormalTexture> vertexList;
	std::vector<DirectX::SimpleMath::Vector3> vertices;
	std::vector<DirectX::SimpleMath::Vector3> normals;
	std::vector<uint16_t> triangles;
	std::vector<DirectX::SimpleMath::Vector2> uvs;

	void createVertices();
	int generateHeight(float x, float y);
	float map(float newmin, float newmax, float origmin, float origmax, float value);
	float fBM(float x, float z, int oct, float pers);
	float lerp(float v0, float v1, float t);
	float inverseLerp(float v0, float v1, float t);
};

