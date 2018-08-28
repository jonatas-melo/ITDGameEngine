#include "pch.h"
#include "GameObject.h"
#include <random>

using namespace DirectX;
using namespace DirectX::SimpleMath;

static const float maxHeight = 100.f;
static const float smooth = 0.01f;
static const int octaves = 4;
static const float persistence = 0.5f;

GameObject::GameObject(float scale, int divisions)
	: sizeQuad(scale), divisions(divisions)
{
}


GameObject::~GameObject()
{
}

void GameObject::createVertices()
{
	Vector2 uv00 = Vector2(0.f, 0.f);
	Vector2 uv10 = Vector2(1.f, 0.f);
	Vector2 uv01 = Vector2(0.f, 1.f);
	Vector2 uv11 = Vector2(1.f, 1.f);

	// int divisions = 1;
	// double scale = 1;
	// double max = scale * divisions;
	//
	// for (double z = -max; z < max; z += scale) {
	// 	for (double x = -max; x < max; x += scale) {
	// 		System.out.print("bottomLeft(" + x + "," + z + ")");
	// 		System.out.print("bottomRight(" + (x + scale) + "," + z + ")");
	// 		System.out.print("topRight(" + (x + scale) + "," + (z + scale) + ")");
	// 		System.out.println("topLeft(" + (x)+"," + (z + scale) + ")\n");
	//
	// 	}
	// }

	double max = sizeQuad * divisions;
	for (double z = -divisions; z < divisions; z++)
	{
		for (double x = -divisions; x < divisions; x++) {
			double tl_y = generateHeight(x, (z + 1));
			double tr_y = generateHeight((x + 1), (z + 1));
			double br_y = generateHeight((x + 1), z);
			double bl_y = generateHeight(x, z);

			Vector3 topLeft = Vector3(x, float(tl_y), (z + 1));
			Vector3 topRight = Vector3((x + 1), float(tr_y), (z + 1));
			Vector3 bottomRight = Vector3((x + 1), float(br_y), z);
			Vector3 bottomLeft = Vector3(x, float(bl_y), z);

			Matrix scale = Matrix::CreateScale(sizeQuad);

			vertexList.emplace_back(Vector3::Transform(bottomLeft, scale), Vector3::Up, uv11);
			vertexList.emplace_back(Vector3::Transform(bottomRight, scale), Vector3::Up, uv01);
			vertexList.emplace_back(Vector3::Transform(topRight, scale), Vector3::Up, uv00);
			vertexList.emplace_back(Vector3::Transform(topLeft, scale), Vector3::Up, uv10);
		}
	}

	for (int i = 0; i < std::pow(divisions, 2) * 4; i++)
	{
		triangles.push_back(4*i+3);
		triangles.push_back(4 * i + 1);
		triangles.push_back(4 * i + 0);
		triangles.push_back(4 * i + 3);
		triangles.push_back(4 * i + 2);
		triangles.push_back(4 * i + 1);
	}

	// Vector3 p0 = Vector3(-0.5f, -0.0f, 0.5f);
	// Vector3 p1 = Vector3(0.5f, -0.0f, 0.5f);
	// Vector3 p2 = Vector3(0.5f, -0.0f, -0.5f);
	// Vector3 p3 = Vector3(-0.5f, -0.0f, -0.5f);
	// Vector3 p4 = Vector3(0.f, 0.0f, -0.3f);
	// Vector3 p5 = Vector3(0.3f, 0.0f, -0.3f);
	// Vector3 p6 = Vector3(0.3f, 0.0f, 0.f);
	// Vector3 p7 = Vector3(0.f, 0.0f, 0.f);

	// case Cubeside.TOP:
	// 	vertices = new Vector3[]{ p7, p6, p5, p4 };
	// 	normals = new Vector3[]{ Vector3.up, Vector3.up,
	// 		Vector3.up, Vector3.up };
	// 	uvs = new Vector2[]{ uv11, uv01, uv00, uv10 };
	// 	triangles = new int[] {3, 1, 0, 3, 2, 1};

	// triangles = new int[] {3, 1, 0, 3, 2, 1};
}

int GameObject::generateHeight(float x, float z)
{
	float height = map(0.f, maxHeight, 0.f, 1.f, fBM(x*smooth, z*smooth, octaves, persistence));
	return int(height);
}

float GameObject::map(float newmin, float newmax, float origmin, float origmax, float value)
{
	return lerp(newmin, newmax, inverseLerp(origmin, origmax, value));
}

float GameObject::fBM(float x, float z, int oct, float pers)
{
	PerlinNoise pn(200);

	float total = 0.f;
	float frequency = 1.f;
	float amplitude = 1.f;
	float maxValue = 0.f;
	for (int i = 0; i < oct; i++)
	{
		total += pn.noise(x * frequency, z * frequency, 0.f) * amplitude;

		maxValue += amplitude;

		amplitude *= pers;
		frequency *= 2.f;
	}

	return total / maxValue;

}

float GameObject::lerp(float v0, float v1, float t)
{
	return (1.f - t) * v0 + t * v1;
}

float GameObject::inverseLerp(float v0, float v1, float t)
{
	return (t - v0) / (v1 - v0);
}
