#pragma once
#include "controller.h"

struct Camera
{
	Controller controller;

	float3 pos;
	float3 viewDir;
	float3 C;
	float d;
	
	float3 p0;
	float3 p1;
	float3 p2;

	Camera(float3 position = float3(0, 0, 0), float3 direction = float3(0, 0, 1), float distance = 1);
	void CalculateScreen();
	void Tick();
	void Translate(float3);
	void Rotate(std::vector<float3>);
};

