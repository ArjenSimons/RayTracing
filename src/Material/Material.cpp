#include "precomp.h"

Material::Material(Color color, shared_ptr<Texture> texture, float specularity)
	: color(color), texture(texture)
{
	this->specularity = clamp(specularity, 0.0, 1.0);
}

Material::~Material()
{

}

Color Material::GetColor(float2 uv, float3 position) const 
{
	return color * texture->value(uv, position);
}