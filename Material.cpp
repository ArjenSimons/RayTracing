#include "precomp.h"
#include "Material.h"

Material::Material(Color color, Color secondColor, MatType type)
	: color(color), secondColor(secondColor), type(type)
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
		Color retVal = color;

		if (((int)(position.x) + (int)(position.z)) & 1)
		{
			retVal = color;
		}
		else
		{
			retVal = secondColor;
		}

		if (position.x < 0)
		{
			if (retVal == color)
			{
				retVal = secondColor;
			}
			else
			{
				retVal = color;
			}
		}

		return retVal;
	}
	else return color;
}