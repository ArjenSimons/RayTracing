#pragma once

struct Controller
{
	bool forward, backward, leftward, rightward;
	float oldX, oldY, newX, newY;
};