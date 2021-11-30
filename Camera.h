#pragma once
struct Camera
{
	float3 pos;
	float3 viewDir;
	float d;
	
	float3 p0;
	float3 p1;
	float3 p2;

	Camera(float3 position = float3(0, 0, 0), float3 direction = float3(0, 0, 1), float distance = 1)
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

	void Step();
};

