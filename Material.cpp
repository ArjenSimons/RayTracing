#include "precomp.h"
#include "Material.h"

Material::Material(Color color, float specularity, Color secondColor, MatType type)
	: color(color), specularity(specularity), secondColor(secondColor), type(type)
{

}

Material::~Material()
{

}

Color Material::GetColor(float3 position)
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