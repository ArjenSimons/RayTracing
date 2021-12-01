#pragma once
struct Camera
{
	float3 pos;
	float3 viewDir;
	float d;
	
	float3 p0;
	float3 p1;
	float3 p2;

	Camera(float3 position = float3(0, 0, 0), float3 direction = float3(0, 0, 1), float distance = 1);

	void HandleInput(int key);
	void Transform(std::vector<float4>);
	void LinearTransform(float3&, std::vector<float4>);
	std::vector<float4> GetRotationMatrix(float);
	std::vector<float4> GetTranslationMatrix(float3);
};

