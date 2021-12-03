#include "precomp.h"
#include "myapp.h"
#include "RayTracer.h"
#include "Intersectable.h"
#include "Texture.h"

TheApp* CreateApp() { return new MyApp(); }

RayTracer* rayTracer;

float lastDeltaTime = 0;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void MyApp::Init()
{
	// anything that happens only once at application start goes here
	std::cout << processor_count << std::endl;

	auto redTexture = make_shared<ColorTexture>(Color(1, 0, 0));
	auto whiteTexture = make_shared<ColorTexture>(Color(1, 1, 1));
	auto blackTexture = make_shared<ColorTexture>(Color(0, 0, 0));
	
	auto checkerTexture = make_shared<CheckerTexture>(whiteTexture, blackTexture);

	Scene scene = Scene();
	Plane* plane = new Plane(float3(0, -1, 0), float3(0, 1, 0), SOLID, Material(float3(1, 1, 1), checkerTexture, 0));// , float3(0, 0, 0), CHECKER));
	Plane* plane1 = new Plane(float3(0, 0, 4), float3(0, 0, -1), SOLID, Material(float3(1, 0, 0), checkerTexture, 0));
	Sphere* sphere = new Sphere(float3(0, .1, 2), 1, SOLID, Material(float3(1, 1, 1), redTexture, 0));
	Sphere* sphere1 = new Sphere(float3(0, .5, 4), .2, SOLID, Material(float3(1, 1, 1), redTexture, 0));

	LightSource* light = new LightSource(float3(1, 1, 3), 10, float3(1, 1, 1));
	LightSource* light1 = new LightSource(float3(-1, 3, -1), 10, float3(1, 1, 1));


	scene.AddObject(plane);
	scene.AddObject(plane1);
	scene.AddObject(sphere);
	scene.AddObject(sphere1);
	scene.AddLightSource(light);
	scene.AddLightSource(light1);


	//Scene teloscopeScene = GetTelescopeScene();

	rayTracer = new RayTracer(scene, 5, THREADING_ENABLED);

	std::cout << "end init" << std::endl;
}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick(float deltaTime)
{
	// clear the screen to black
	screen->Clear(0);

	rayTracer->Render();

	for (int i = 0; i <  SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		//Ray ray = rayTracer->GetUVRay(rayTracer->GetUV(i, j));
		//screen->Plot(i, j, rayTracer->Trace(ray).GetRGBValue());
		screen->Plot(i, j, rayTracer->GetBufferValue(i, j));
	}

	//for(int red = 0; red < 256; red++) for(int green = 0; green < 256; green++)
	//{
	//	int x = red, y = green;
	//	screen->Plot(x + 200, y + 100, (red << 16) + (green << 8));
	//}
	// plot a white pixel in the bottom right corner
	//screen->Plot(SCRWIDTH - 2, SCRHEIGHT - 2, 0xffffff);

	std::cout << deltaTime << "ms" << std::endl;
}

