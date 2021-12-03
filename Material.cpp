#include "precomp.h"
#include "Material.h"

Material::Material(Color color, shared_ptr<Texture> texture, float specularity, Color secondColor, MatType type)
	: color(color), texture(texture), secondColor(secondColor), type(type)
{
	this->specularity = clamp(specularity, 0.0, 1.0);
}

Material::~Material()
{

}

Color Material::GetColor(float2 uv, float3 position) const 
{
	if (type == UNIFORM)
	{
		return color * texture->value(uv, position);
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