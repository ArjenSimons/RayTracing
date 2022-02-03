#pragma once

struct Camera
{
	Controller controller;

	float3 pos;
	float3 viewDir;
	float3 C;
	float d;
	float a;
	
	float3 p0;
	float3 p1;
	float3 p2;

	Camera(float3 position = float3(0, 0, 0), float3 direction = float3(0, 0, 1), float FOVDegree = 90, float aspectRatio = 0);
	void CalculateScreen();
	bool Tick();
	void Translate(float3);
	void FOVIncr(float);
};

