#include "precomp.h"
#include "myapp.h"
#include "RayTracer.h"
#include "Intersectable.h"

#define M_PI 3.14159265358979323846 /* pi */

TheApp* CreateApp() { return new MyApp(); }

RayTracer* rayTracer;

float lastDeltaTime = 0;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void MyApp::Init()
{
	// anything that happens only once at application start goes here

	Scene scene = Scene();
	Plane* plane = new Plane(float3(0, -1, 0), float3(0, 1, 0), Material(float3(1, 1, 1), float3(0, 0, 0), CHECKER));
	//Plane* plane1 = new Plane(float3(0, 0, 5), float3(0, 0, -1), Material(float3(1, 1, 1), float3(0, 0, 0)));
	Sphere* sphere = new Sphere(float3(0, 1, 3), 1, Material(float3(1, 0, 0)));
	Sphere* sphere2 = new Sphere(float3(0, 1, -3), 1, Material(float3(1, 1, 0)));
	Sphere* sphere3 = new Sphere(float3(3, 1, 0), 1, Material(float3(0, 1, 0)));
	Sphere* sphere4 = new Sphere(float3(-3, 1, 0), 1, Material(float3(0, 1, 1)));

	LightSource* light = new LightSource(float3(1, 3, 1), 1, float3(1, 1, 1));
	LightSource* light1 = new LightSource(float3(-1, 3, -1), 1, float3(1, 1, 1));

	scene.AddObject(plane);
	scene.AddObject(sphere);
	scene.AddObject(sphere2);
	scene.AddObject(sphere3);
	scene.AddObject(sphere4);
	scene.AddLightSource(light);
	scene.AddLightSource(light1);

	rayTracer = new RayTracer(scene);

	std::cout << "end init" << std::endl;
}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick(float deltaTime)
{
	// clear the screen to black
	screen->Clear(0);

	for (int i = 0; i <  SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		Ray ray = rayTracer->GetUVRay(rayTracer->GetUV(i, j));
		screen->Plot(i, j, rayTracer->Trace(ray).GetRGBValue());

	}

	rayTracer->cam.Tick();

	//for(int red = 0; red < 256; red++) for(int green = 0; green < 256; green++)
	//{
	//	int x = red, y = green;
	//	screen->Plot(x + 200, y + 100, (red << 16) + (green << 8));
	//}
	// plot a white pixel in the bottom right corner
	//screen->Plot(SCRWIDTH - 2, SCRHEIGHT - 2, 0xffffff);

	std::cout << deltaTime << "ms" << std::endl;
}

void MyApp::MouseMove(int x, int y)
{
	rayTracer->cam.controller.newX = x;
	rayTracer->cam.controller.newY = y;
}
void MyApp::KeyUp(int key)
{
	/*
	W = 87
	A = 65
	S = 83
	D = 68
	*/

	switch (key)
	{
	case 87:
		rayTracer->cam.controller.forward = false;
		break;

	case 83:
		rayTracer->cam.controller.backward = false;
		break;

	case 65:
		rayTracer->cam.controller.leftward = false;
		break;

	case 68:
		rayTracer->cam.controller.rightward = false;
		break;

	default:
		break;
	}
}
void MyApp::KeyDown(int key)
{
	std::cout << "Key down" << std::endl;

	/*
	W = 87
	A = 65
	S = 83
	D = 68
	*/

	switch (key)
	{
	case 87:
		std::cout << "forward" << std::endl;
		rayTracer->cam.controller.forward = true;
		break;

	case 83:
		rayTracer->cam.controller.backward = true;
		break;

	case 65:
		rayTracer->cam.controller.leftward = true;
		break;

	case 68:
		rayTracer->cam.controller.rightward = true;
		break;

	default:
		break;
	}
}