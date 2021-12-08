#include "precomp.h"
#include "myapp.h"
#include "RayTracer.h"
#include "Intersectable.h"
#include "Texture.h"
#include "Mesh.h"

enum class Scenes
{
	GENERAL_SETUP,
	TEXTURE_SETUP,
	CORNELL_BOX_SETUP,
	GLASS_SPHERE_SETUP,
	MESH_SETUP,
	SPOT_LIGHT_SETUP,
	DONUT_SETUP
};

TheApp* CreateApp() { return new MyApp(); }

RayTracer* rayTracer;

std::vector<Intersectable*> objects;
std::vector<LightSource*> lights;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void MyApp::Init()
{
	//Textures
	auto redTexture = make_shared<ColorTexture>(Color(.45, .12, .12));
	auto greenTexture = make_shared<ColorTexture>(Color(.12, .45, .15));
	auto greyTexture = make_shared<ColorTexture>(Color(.73, .73, .73));
	auto whiteTexture = make_shared<ColorTexture>(Color(1, 1, 1));
	auto blackTexture = make_shared<ColorTexture>(Color(0, 0, 0));
	auto silverTexture = make_shared<ColorTexture>(Color(.8, .8, .8));

	auto checkerTexture = make_shared<CheckerTexture>(whiteTexture, blackTexture);

	Scene scene = Scene();

	shared_ptr<ImageTexture> earthTexture = nullptr;
	shared_ptr<ImageTexture> marbleTexture = nullptr;
	shared_ptr<ImageTexture> brickTexture = nullptr;
	shared_ptr<Mesh> treeMesh = nullptr;
	shared_ptr<Mesh> cubeMesh = nullptr;


	switch (Scenes::GENERAL_SETUP)
	{
	case(Scenes::GENERAL_SETUP):
		objects.push_back(new Plane(float3(0, -1, 0), float3(0, 1, 0), SOLID, Material(float3(1, 1, 1), checkerTexture, 0)));
		objects.push_back(new Sphere(float3(-1, -0.5, 2.5), .5, SOLID, Material(float3(1, 1, 1), silverTexture, 1)));
		objects.push_back(new Torus(float3(1.5, 0, 3), .2, .6, float3(-40, 0, 0), SOLID, Material(float3(1, 1, 1), redTexture, 0)));
		cubeMesh = make_shared<Mesh>("res/cube.obj");
		objects.push_back(new Model(float3(.2, -.5, 2), .3, cubeMesh, ABSORBING_GLASS, Material(float3(.1, .1, .9), whiteTexture)));

		lights.push_back(new PointLight(float3(1, 1, 1), 5, float3(1, 1, 1)));
		lights.push_back(new PointLight(float3(-1, 3, -1.5), 10, float3(1, 1, 1)));

		lights.push_back(new DirectionalLight(float3(0, 0, 0), float3(.2f, -.8f, .1f), .1f, float3(1, 1, 1)));
		break;
	case(Scenes::TEXTURE_SETUP):
		earthTexture = make_shared<ImageTexture>("res/earthmap.jpg");
		marbleTexture = make_shared<ImageTexture>("res/marble.jpg");
		brickTexture = make_shared<ImageTexture>("res/bricks.jpg");

		objects.push_back(new Plane(float3(0, -1, 0), float3(0, 1, 0), SOLID, Material(float3(1, 1, 1), marbleTexture, .4)));
		objects.push_back(new Plane(float3(0, 0, 4), float3(0, 0, -1), SOLID, Material(float3(1, 1, 1), brickTexture, 0)));
		objects.push_back(new Sphere(float3(0, 0, 2), 1, SOLID, Material(float3(1, 1, 1), earthTexture, 0)));

		lights.push_back(new PointLight(float3(1, 1, 1), 5, float3(1, 1, 1)));
		lights.push_back(new PointLight(float3(-1, 3, -1.5), 10, float3(1, 1, 1)));
		lights.push_back(new DirectionalLight(float3(0, 0, 0), float3(.2f, -.8f, .1f), .1f, float3(1, 1, 1)));
		break;
	case(Scenes::GLASS_SPHERE_SETUP):
		objects.push_back(new Sphere(float3(0, .5, 2), 1, GLASS, Material(float3(1, 1, 1), silverTexture, 0)));
		objects.push_back(new Sphere(float3(0, 1.8, 5), .5, SOLID, Material(float3(1, 1, 1), redTexture, 0)));

		objects.push_back(new Plane(float3(0, -1, 0), float3(0, 1, 0), SOLID, Material(float3(1, 1, 1), checkerTexture, 0)));

		lights.push_back(new PointLight(float3(1, 3, 3), 10, float3(1, 1, 1)));
		lights.push_back(new PointLight(float3(-1, 3, 1), 10, float3(1, 1, 1)));
		lights.push_back(new DirectionalLight(float3(0, 8, 0), float3(.2f, -.8f, .1f), .1f, float3(1, 1, 1)));
		break;
	case(Scenes::MESH_SETUP):
		treeMesh = make_shared<Mesh>("res/tree.obj");
		objects.push_back(new Model(float3(0, -1, 7), 1, treeMesh, SOLID, Material(float3(1, 1, 1), greenTexture)));

		objects.push_back(new Plane(float3(0, -1, 0), float3(0, 1, 0), SOLID, Material(float3(1, 1, 1), checkerTexture, 0)));

		lights.push_back(new PointLight(float3(-1, 3, 5), 10, float3(1, 1, 1)));
		lights.push_back(new PointLight(float3(1, 3, 5), 10, float3(1, 1, 1)));
		break;
	case(Scenes::SPOT_LIGHT_SETUP):
		objects.push_back(new Plane(float3(0, 0, 2), float3(0, 0, -1), SOLID, Material(float3(1, 1, 1), whiteTexture, 0)));

		lights.push_back(new SpotLight(float3(-.5, .5, 1), float3(0, 0, 1), 40, 10, float3(1, 0, 0)));
		lights.push_back(new SpotLight(float3(.5, .5, 1), float3(0, 0, 1), 40, 10, float3(0, 1, 0)));
		lights.push_back(new SpotLight(float3(0, -.5, 1), float3(0, 0, 1), 40, 10, float3(0, 0, 1)));
		break;
	}

	for (Intersectable* obj : objects)
	{
		scene.AddObject(obj);
	}

	for (LightSource* light : lights)
	{
		scene.AddLightSource(light);
	}

	rayTracer = new RayTracer(scene, 0, 5, THREADING_ENABLED, MSAA::NONE);

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
	rayTracer->AddVignette(.6f, .3f, 1);
	rayTracer->AddGammaCorrection(.6f);
	//rayTracer->AddChromaticAberration(int2(10, 0), int2(0, 0), int2(0, 0));

	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		screen->Plot(i, j, rayTracer->GetBufferValue(i, j).GetRGBValue());
	}

	rayTracer->cam.Tick();

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
