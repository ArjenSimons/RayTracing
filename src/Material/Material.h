#pragma once

class Material
{
protected:
	Color color;
	shared_ptr<Texture> texture;
public:
	float specularity = 0;
	float emission = 0;
public:
	Material(Color color, shared_ptr<Texture> texture, float specularity = 0);
	Material(Color color, float emission) {
		texture = make_shared<ColorTexture>(Color(1, 1, 1));
		this->color = color;
		this->emission = emission;
	};
	~Material() {};
	Color GetEmission() { 
		return color * emission; 
	}
	Color GetColor(float2 uv, float3 position) const;
};