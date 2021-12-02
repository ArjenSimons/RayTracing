#include "precomp.h"
#include "Camera.h"

Camera::Camera(float3 position, float3 direction, float distance)
{
	controller = Controller();
	pos = position;
	viewDir = direction;
	d = distance;
	CalculateScreen();
}

void Camera::CalculateScreen()
{
	C = pos + d * viewDir;
	float aspectRatio = (float)SCRWIDTH / (float)SCRHEIGHT;

	p0 = C + float3(-1 * aspectRatio, 1, 0);
	p1 = C + float3(1 * aspectRatio, 1, 0);
	p2 = C + float3(-1 * aspectRatio, -1, 0);
}

void Camera::Tick()
{
	if (controller.forward)
	{
		// These translations should be changed to take the viewDirection 
		// into account
		Translate(float3(0, 0, 1));
	}

	if (controller.backward)
	{
		Translate(float3(0, 0, -1));
	}

	if (controller.leftward)
	{
		Translate(float3(-1, 0, 0));
	}

	if (controller.rightward)
	{
		Translate(float3(1, 0, 0));
	}

	CalculateScreen();
}

void Camera::Rotate(std::vector<float3> matrix)
{
	viewDir.x = matrix[0].x * viewDir.x + matrix[0].y * viewDir.y + matrix[0].z * viewDir.z;
	viewDir.y = matrix[1].x * viewDir.x + matrix[1].y * viewDir.y + matrix[1].z * viewDir.z;
	viewDir.z = matrix[2].x * viewDir.x + matrix[2].y * viewDir.y + matrix[2].z * viewDir.z;
}

void Camera::Translate(float3 direction)
{
	pos.x += direction.x;
	pos.y += direction.y;
	pos.z += direction.z;
}