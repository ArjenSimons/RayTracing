#include "precomp.h"
#include "CheckerMaterial.h"

CheckerMaterial::CheckerMaterial(float3 color, float3 secondColor)
	: Material(color), secondColor(secondColor)
{

}

CheckerMaterial::~CheckerMaterial()
{

}

float3 CheckerMaterial::GetColor(float3 position)
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