#include "precomp.h"
#include "myapp.h"
#include "RayTracer.h"
#include "Intersectable.h"
#include "Texture.h"
#include "Mesh.h"

TheApp* CreateApp() { return new MyApp(); }

RayTracer* rayTracer;

float lastDeltaTime = 0;

std::vector<Intersectable*> objects;
std::vector<LightSource*> lights;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void MyApp::Init()
{
	// anything that happens only once at application start goes here
	std::cout << processor_count << std::endl;

	//Textures
	auto redTexture = make_shared<ColorTexture>(Color(1, 0, 0));
	auto whiteTexture = make_shared<ColorTexture>(Color(1, 1, 1));
	auto blackTexture = make_shared<ColorTexture>(Color(0, 0, 0));
	auto silverTexture = make_shared<ColorTexture>(Color(.8, .8, .8));
	
	auto checkerTexture = make_shared<CheckerTexture>(whiteTexture, blackTexture);
	auto earthTexture = make_shared<ImageTexture>("res/earthmap.jpg");
	auto marbleTexture = make_shared<ImageTexture>("res/marble.jpg");
	auto brickTexture = make_shared<ImageTexture>("res/bricks.jpg");

	//meshes
	auto tree = make_shared<Mesh>("res/tree.obj");
	auto cube = make_shared<Mesh>("res/cube.obj");

	Scene scene = Scene();
	objects.push_back(new Sphere(float3(-5, 1, 0), 2, SOLID, Material(float3(1, 1, 1), redTexture, 0)));
	//objects.push_back(new Torus(float3(-5, 0, -1), 1, 2, float3(0, 0, 0), SOLID, Material(float3(1, 1, 1), brickTexture, 0)));
	objects.push_back(new Plane(float3(0, -1, 0), float3(0, 1, 0), SOLID, Material(float3(1, 1, 1), checkerTexture, 0)));
	//objects.push_back(new Sphere(float3(0, 3, 11), 1, SOLID, Material(float3(1, 1, 1), redTexture, 0)));
	//objects.push_back(new Sphere(float3(0, .1, 2), 1, SOLID, Material(float3(1, 1, 1), earthTexture, 0)));
	//objects.push_back(new Sphere(float3(0, 0, 5.9), 5, WATER, Material(float3(1, 1, 1), whiteTexture, 0)));
	//objects.push_back(new Model(float3(0, 0, 10), 1, cube, SOLID, Material(float3(1, 1, 1), redTexture, 0)));
	//objects.push_back(new Model(float3(0, 0, 7), 1, tree, SOLID, Material(float3(1, 1, 1), silverTexture, 1)));
	
	//lights.push_back(new SpotLight(float3(-5, 1, 0), float3(1, 0, 0), 55, 10, float3(1, 1, 1)));
	//lights.push_back(new LightSource(float3(1, 1, 1), 5, float3(1, 1, 1)));
	//lights.push_back(new LightSource(float3(-1, 3, -1.5), 10, float3(1, 1, 1)));
	//lights.push_back(new PointLight(float3(0, 0, 0), 10, float3(1, 1, 1)));
	//lights.push_back(new LightSource(float3(0, 0, 5), 10, float3(1, 1, 1)));
	lights.push_back(new DirectionalLight(float3(0, 0, 0), float3(0, 1, 0), 1, float3(1, 1, 1)));
	//lights.push_back(new LightSource(float3(-5, 0, -2), 10, float3(1, 1, 1)));
	//lights.push_back(new LightSource(float3(5, 0, 0), 10, float3(1, 1, 1)));
	//lights.push_back(new LightSource(float3(0, 5, 0), 10, float3(1, 1, 1)));
	//lights.push_back(new LightSource(float3(0, -5, 0), 10, float3(1, 1, 1)));
	//objects.push_back(new Plane(float3(0, -1, 0), float3(0, 1, 0), SOLID, Material(float3(1, 1, 1), checkerTexture, 0)));
	//objects.push_back(new Plane(float3(0, 0, 1), float3(0, 0, -1), SOLID, Material(float3(1, 1, 1), redTexture, 0)));
	
	
	for (Intersectable* obj : objects)
	{
		scene.AddObject(obj);
	}

	for (LightSource* light : lights)
	{
		scene.AddLightSource(light);
	}

	//Scene teloscopeScene = GetTelescopeScene();

	rayTracer = new RayTracer(scene, 5, THREADING_ENABLED, MSAA::NONE);

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
	switch (key)
	{
	case 87: // W
		rayTracer->cam.controller.forward = false;
		break;

	case 83: // S
		rayTracer->cam.controller.backward = false;
		break;

	case 65: // A
		rayTracer->cam.controller.leftward = false;
		break;

	case 68: // D
		rayTracer->cam.controller.rightward = false;
		break;

	case 340: // SHIFT
		rayTracer->cam.controller.rotate = false;
		break;

	default:
		break;
	}
}
void MyApp::KeyDown(int key)
{
	switch (key)
	{
	case 87: // W
		rayTracer->cam.controller.forward = true;
		break;

	case 83: // S
		rayTracer->cam.controller.backward = true;
		break;

	case 65: // A
		rayTracer->cam.controller.leftward = true;
		break;

	case 68: // D
		rayTracer->cam.controller.rightward = true;
		break;

	case 340: // SHIFT
		rayTracer->cam.controller.rotate = true;
		break;

	case 265: // ^
		rayTracer->cam.FOVIncr(-0.25f);
		break;

	case 264: // v
		rayTracer->cam.FOVIncr(0.25f);
		break;

	default:
		break;
	}
}

void MyApp::Shutdown()
{
	delete rayTracer;

	for (Intersectable* obj : objects)
	{
		delete obj;
	}

	for (LightSource* light : lights)
	{
		delete light;
	}
}
