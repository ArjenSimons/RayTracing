#pragma once
//#include "precomp.h"

struct Controller
{
	bool forward, backward, leftward, rightward, rotate;
	float oldX, oldY, newX, newY;

	float2 DeltaPos() { return make_float2(newX - oldX, newY - oldY); }
};