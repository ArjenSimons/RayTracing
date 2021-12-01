#include "precomp.h"
#include "Camera.h"


Camera::Camera(float3 position, float3 direction, float distance)
{
	pos = position;
	viewDir = direction;
	d = distance;

	float aspectRatio = (float)SCRWIDTH / (float)SCRHEIGHT;
	float3 C = pos + d * viewDir;

	p0 = C + float3(-1 * aspectRatio, 1, 0);
	p1 = C + float3(1 * aspectRatio, 1, 0);
	p2 = C + float3(-1 * aspectRatio, -1, 0);
}

void Camera::HandleInput(int key)
{
	/*
	Q = 81
	E = 69
	W = 87
	A = 65
	S = 83
	D = 68
	*/
	std::vector<float4> matrix;
	
	if (key == 81)
	{
		matrix = GetRotationMatrix(0.125 * CL_M_PI);
		
	}

	if (key == 69)
	{
		matrix = GetRotationMatrix(-0.125 * CL_M_PI);
	}
	
	// use viewDir to have these matrices not fixed
	if (key == 87)
	{
		matrix = GetTranslationMatrix(float3(0, 0, 1));
	}

	if (key == 65)
	{
		matrix = GetTranslationMatrix(float3(-1, 0, 0));
	}

	if (key == 83)
	{
		matrix = GetTranslationMatrix(float3(0, 0, -1));
	}

	if (key == 68)
	{
		matrix = GetTranslationMatrix(float3(1, 0, 0));
	}

	Transform(matrix);
}

void Camera::Transform(std::vector<float4> matrix)
{
	if (matrix.size() != 4)
	{
		std::cout << "attempted to transform with unfit matrix" << std::endl;
		return;
	}

	LinearTransform(pos, matrix);
	LinearTransform(p0, matrix);
	LinearTransform(p1, matrix);
	LinearTransform(p2, matrix);
}

void Camera::LinearTransform(float3 &point, std::vector<float4> matrix)
{
	point.x = matrix[0].x * point.x + matrix[0].y * point.y + matrix[0].z * point.z + matrix[0].w;
	point.y = matrix[1].x * point.x + matrix[1].y * point.y + matrix[1].z * point.z + matrix[1].w;
	point.z = matrix[2].x * point.x + matrix[2].y * point.y + matrix[2].z * point.z + matrix[2].w;
}

std::vector<float4> Camera::GetRotationMatrix(float theta)
{
	// This should include something to make sure rotations always happen around
	// the camera position:
	// the camera should be translated to the origin, rotated, and translated back
	// these translation matrices are yet missing

	std::vector<float4> matrix;

	// For rotations around the y-axis
	matrix.push_back(make_float4(cos(theta), 0, -sin(theta), 0));
	matrix.push_back(make_float4(     0    , 1,      0     , 0));
	matrix.push_back(make_float4(sin(theta), 0, cos(theta) , 0));
	matrix.push_back(make_float4(     0    , 0,      0     , 1));

	return matrix;
}

std::vector<float4> Camera::GetTranslationMatrix(float3 direction)
{
	std::vector<float4> matrix;

	// For rotations around the y-axis
	matrix.push_back(make_float4(1, 0, 0, direction.x));
	matrix.push_back(make_float4(0, 1, 0, direction.y));
	matrix.push_back(make_float4(0, 0, 1, direction.z));
	matrix.push_back(make_float4(0, 0, 0, 1));

	return matrix;
}