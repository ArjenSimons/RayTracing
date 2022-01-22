#include "precomp.h"

Camera::Camera(float3 position, float3 direction, float FOVDegree, float aspectRatio)
{
	controller = Controller();
	pos = position;
	viewDir = normalize(direction);

	a = clamp(aspectRatio, 0.0f, 2.0f);
	if (aspectRatio == 0)
	{
		a = (float)SCRWIDTH / (float)SCRHEIGHT;
	}
	float FOVRads = FOVDegree * PI / 180;
	float distance = (0.5 * a / tan(0.5 * FOVRads));
	d = clamp(distance, 0.0f, 2.0f);

	CalculateScreen();
}

void Camera::CalculateScreen()
{
	C = pos + d * viewDir;

	float3 upEstim = float3(0, 1, 0);
	float3 right = normalize(cross(upEstim, viewDir));
	float3 up = normalize(cross(viewDir, right));

	p0 = C - right * a + up;
	p1 = C + right * a + up;
	p2 = C - right * a - up;
}

void Camera::Tick()
{
	float2 deltaMouse = controller.DeltaPos();
	controller.oldX = controller.newX;
	controller.oldY = controller.newY;

	if (controller.forward) 
	{ 
		float3 forward = viewDir; 
		Translate(forward);
	}
	if (controller.backward) 
	{ 
		float3 backward = -viewDir;
		Translate(backward); 
	}
	if (controller.leftward) 
	{
		float3 upEstim = float3(0, 1, 0);
		float3 left = normalize(cross(viewDir, upEstim));
		Translate(left);
	}
	if (controller.rightward) 
	{ 
		float3 upEstim = float3(0, 1, 0);
		float3 right = normalize(cross(upEstim, viewDir));
		Translate(right);
	}

	// Because the mouse is not 'fixed' such as in most freeform viewing experiences,
	// it is possible to reach the top of your screen before you have looked as far up as you want
	// thus it is desireable to move the mouse around without actually changing the camera orientation.
	if (controller.rotate)
	{
		// Check if delta position is greater than some margin
	// to make sure we are not proccing on minute movements
		float angleX = 4 * deltaMouse.x / SCRWIDTH;
		if (angleX >= 0.125 || angleX <= -0.125)
		{
			// the delta-X dictates rotations around the Y axis
			// around this axis, we allow 360 degrees of rotation

			float3 upEstim = float3(0, 1, 0);
			float3 right = normalize(cross(upEstim, viewDir));

			viewDir = normalize(viewDir + right * angleX);
		}

		// Multiplying with -4 instead of 4 because the origin of the screen (0,0)
		// is in the top left instead of bottom left
		// ie. moving up results in a negative delta position, and vice versa
		float angleY = -4 * deltaMouse.y / SCRHEIGHT;
		if (angleY >= 0.125 || angleY <= -0.125)
		{
			// Here we rotate around the Z axis
			// Here, we only allow <180 degrees of rotation
			// to prevent the screen from flipping over.
			// Additionally, if we allow the viewDir to become (0, 1, 0)
			// we can no longer infer upDirection and rightDirection from cross products.

			float3 upEstim = float3(0, 1, 0);
			float3 right = normalize(cross(upEstim, viewDir));
			float3 up = normalize(cross(viewDir, right));

			float3 newDir = normalize(viewDir + up * angleY);
			// check if the sign of x and z have changed
			// they should only change if the new direction has 'crossed over'
			// which we do not allow
			if (!(newDir.x * viewDir.x < 0 || newDir.z * viewDir.z < 0))
			{
				viewDir = newDir;
			}
		}
	}
	
	CalculateScreen();
}

void Camera::Translate(float3 direction)
{
	pos.x += direction.x;
	pos.y += direction.y;
	pos.z += direction.z;
}

void Camera::FOVIncr(float amount)
{
	// Changes will take effect the next cameraTick
	// we do not want d to be <= 0 for obvious reasons
	// we also do not want d to be > 2 for arbitrary reasons

	if (d + amount > 0 && d + amount <= 2)
	{
		d += amount;
	}
}