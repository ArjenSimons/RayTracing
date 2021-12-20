#include "precomp.h"
#include "myapp.h"
#include "RayTracer.h"
#include "Intersectable.h"
#include "Texture.h"
#include "Mesh.h"
#include "BVH.h"
#include <chrono>

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
Model* model;
// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void MyApp::Init()
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	//Textures
	auto redTexture = make_shared<ColorTexture>(Color(.45, .12, .12));
	auto greenTexture = make_shared<ColorTexture>(Color(.12, .45, .15));
	auto greyTexture = make_shared<ColorTexture>(Color(.73, .73, .73));
	auto whiteTexture = make_shared<ColorTexture>(Color(1, 1, 1));
	auto blackTexture = make_shared<ColorTexture>(Color(0, 0, 0));
	auto silverTexture = make_shared<ColorTexture>(Color(.8, .8, .8));

	//auto checkerTexture = make_shared<CheckerTexture>(whiteTexture, blackTexture);

	Scene scene = Scene();

	//shared_ptr<ImageTexture> earthTexture = nullptr;
	//shared_ptr<ImageTexture> marbleTexture = nullptr;
	//shared_ptr<ImageTexture> brickTexture = nullptr;


	shared_ptr<Mesh> mesh = make_shared<Mesh>("res/buddha.obj");

	model = new Model(float3(0, -1, 2), 10, mesh, SOLID, Material(float3(1, 1, 1), redTexture));
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Load Model Time:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

	int count = 5;

	printf("# of tris = %i\n", model->GetTriangles().size());

	BVH bvh = BVH(model->GetTriangles(), model->GetTriangles().size(), true);
	begin = std::chrono::steady_clock::now();
	bvh.ConstructBVH();
	end = std::chrono::steady_clock::now();
	std::cout << "BVH Construction time:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

	//objects.push_back(new Model(float3(0, -1, 2), 10, mesh, SOLID, Material(float3(1, 1, 1), redTexture)));

	//objects.push_back(new Plane(float3(0, -1, 0), float3(0, 1, 0), SOLID, Material(float3(1, 1, 1), checkerTexture, 0)));

	//lights.push_back(new PointLight(float3(-1, 5, 5), 100, float3(1, 1, 1)));
	lights.push_back(new DirectionalLight(float3(0, 0, 14), float3(.2f, -.8f, .1f), .1f, float3(1, 1, 1)));

	for (Intersectable* obj : objects)
	{
		scene.AddObject(obj);
	}

	for (LightSource* light : lights)
	{
		scene.AddLightSource(light);
	}

	rayTracer = new RayTracer(scene, 0, 5, THREADING_ENABLED, MSAA::NONE);


}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick(float deltaTime)
{
	// clear the screen to black
	screen->Clear(0);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	rayTracer->Render();

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	std::cout << "Render time:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;


	//rayTracer->AddVignette(.6f, .3f, 1);
	//rayTracer->AddGammaCorrection(.6f);
	//rayTracer->AddChromaticAberration(int2(10, 0), int2(0, 0), int2(0, 0));

	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		screen->Plot(i, j, rayTracer->GetBufferValue(i, j).GetRGBValue());
	}

	//rayTracer->cam.Tick();

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
