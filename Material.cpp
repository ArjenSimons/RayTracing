#include "precomp.h"
#include "Material.h"

Material::Material(float3 color, float3 secondColor, MatType type)
	: color(color), secondColor(secondColor), type(type)
{

}

Material::~Material()
{

}

float3 Material::GetColor(float3 position) 
{
	if (type == SOLID)
	{
		return color;
	}
	else if (type == CHECKER)
	{
		if (((int)(position.x) + (int)(position.z)) & 1)
		{
			return color;
		}
		else
		{
			return secondColor;
		}
	}
	else return color;
}