#pragma once
enum MatType {
	SOLID,
	CHECKER
};

class Material
{
protected:
	float3 color;
	float3 secondColor;
	MatType type;
public:
	Material(float3 color, float3 secondColor = float3(1, 0, 1), MatType type = SOLID);
	~Material();
	float3 GetColor(float3 position);
};