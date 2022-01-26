#include "precomp.h"
#include "myapp.h"

enum class Scenes
{
	ANIM_SETUP,
	BUDDHA_SETUP,
	BVH_TEST,
	SBVH_TEST
};

TheApp* CreateApp() { return new MyApp(); }

RayTracer* rayTracer;

std::vector<Intersectable*> objects;
std::vector<LightSource*> lights;
Model* model;
Model* bunnyModel;

std::vector<BVHInstance*> bvhs;
TopLevelBVH* topBVH;
vector<BVHInstance*> buddhas;
BVH* buddha;
BVH* dragonBVH;
BVH* bunnyBVH;
BVH* planeBVH;
BVHInstance* dragonInstance;
BVHInstance* dragonInstance1;
BVHInstance* bunnyInstance;
BVHInstance* planeInstance;
Scene* scene;

int x = 0;
Scenes sceneType = Scenes::SBVH_TEST;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void MyApp::Init()
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	//Textures
	auto redTexture = make_shared<ColorTexture>(Color(.45, .12, .12));
	auto greenTexture = make_shared<ColorTexture>(Color(.12, .45, .15));
	//auto greyTexture = make_shared<ColorTexture>(Color(.73, .73, .73));
	//auto whiteTexture = make_shared<ColorTexture>(Color(1, 1, 1));
	//auto blackTexture = make_shared<ColorTexture>(Color(0, 0, 0));
	//auto silverTexture = make_shared<ColorTexture>(Color(.8, .8, .8));
	//auto checkerTexture = make_shared<CheckerTexture>(whiteTexture, blackTexture);

	//shared_ptr<ImageTexture> earthTexture = nullptr;
	//shared_ptr<ImageTexture> marbleTexture = nullptr;
	//shared_ptr<ImageTexture> brickTexture = nullptr;
	shared_ptr<Mesh> dragonMesh;
	shared_ptr<Mesh> bunnyMesh;
	shared_ptr<Mesh> longPlaneMesh;

	switch (sceneType)
	{
	case(Scenes::ANIM_SETUP):
		dragonMesh = make_shared<Mesh>("res/dragon.obj");
		bunnyMesh = make_shared<Mesh>("res/bunny.obj");

		model = new Model(float3(0, -1, 4), 2, dragonMesh, SOLID, Material(float3(1, 1, 1), redTexture));
		bunnyModel = new Model(float3(-3, -1, 3), 5, bunnyMesh, SOLID, Material(float3(1, 1, 1), greenTexture));
		end = std::chrono::steady_clock::now();
		std::cout << "Load Model Time:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

		printf("# of tris = %i\n", model->GetTriangles()->size() + bunnyModel->GetTriangles()->size());

		dragonBVH = new BVH(model->GetTriangles(), model->GetTriangles()->size(), model->GetTranslation(), true);
		bunnyBVH = new BVH(bunnyModel->GetTriangles(), bunnyModel->GetTriangles()->size(), bunnyModel->GetTranslation(), false);

		begin = std::chrono::steady_clock::now();
		dragonBVH->ConstructBVH();
		bunnyBVH->ConstructBVH();
		dragonInstance = new BVHInstance(dragonBVH);
		dragonInstance->RotateY(90);
		dragonInstance1 = new BVHInstance(dragonBVH);
		dragonInstance1->Translate(float3(-3, 0, 0));
		dragonInstance1->RotateY(-90);
		bunnyInstance = new BVHInstance(bunnyBVH);
		end = std::chrono::steady_clock::now();
		std::cout << "BVH Construction time:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

		objects.push_back(bunnyModel);

		bvhs.push_back(dragonInstance);
		bvhs.push_back(dragonInstance1);
		bvhs.push_back(bunnyInstance);
		break;
	case(Scenes::BVH_TEST):
		dragonMesh = make_shared<Mesh>("res/dragon.obj");
		model = new Model(float3(0, 0, 1), 2, dragonMesh, SOLID, Material(float3(1, 1, 1), redTexture));
		dragonBVH = new BVH(model->GetTriangles(), model->GetTriangles()->size(), model->GetTranslation(), true);

		dragonBVH->ConstructBVH();
		dragonInstance = new BVHInstance(dragonBVH);
		dragonInstance->RotateY(90);
		//dragonInstance->Translate(float3(0, 0, 0));
		bvhs.push_back(dragonInstance);
		break;
	case(Scenes::SBVH_TEST):
		longPlaneMesh = make_shared<Mesh>("res/sbvhTest.obj");
		model = new Model(float3(0, 0, 1), .1f, longPlaneMesh, SOLID, Material(float3(1, 1, 1), redTexture));
		planeBVH = new BVH(model->GetTriangles(), model->GetTriangles()->size(), model->GetTranslation(), true);

		planeBVH->ConstructBVH();
		planeInstance = new BVHInstance(planeBVH);
		//planeInstance->RotateZ(45);
		printf("# of tris = %i\n", model->GetTriangles()->size());

		bvhs.push_back(planeInstance);
		break;
	case(Scenes::BUDDHA_SETUP):
		shared_ptr<Mesh> buddhaMesh = make_shared<Mesh>("res/buddha.obj");

		model = new Model(float3(-15, -5, 9), 2, buddhaMesh, SOLID, Material(float3(1, 1, 1), redTexture));
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		std::cout << "Load Model Time:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

		printf("# of tris = %i\n", model->GetTriangles()->size() * 31 * 30);

		begin = std::chrono::steady_clock::now();

		buddha = new BVH(model->GetTriangles(), model->GetTriangles()->size(), model->GetTranslation(), true);
		buddha->ConstructBVH();

		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 31; j++)
			{
				buddhas.push_back(new BVHInstance(buddha));
				bvhs.push_back(buddhas.back());
				buddhas.back()->Translate(float3(i, 0, j));
			}
		}

		end = std::chrono::steady_clock::now();
		std::cout << "BVH Construction time:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
		break;
	}
	
	topBVH = new TopLevelBVH(&bvhs);
	//objects.push_back(new Plane(float3(0, -1, 0), float3(0, 1, 0), SOLID, Material(float3(1, 1, 1), checkerTexture, 0)));
	
	lights.push_back(new PointLight(float3(-1, 3, 0), 10, float3(1, 1, 1)));
	lights.push_back(new DirectionalLight(float3(0, 0, 14), float3(.2f, -.8f, -.1f), 1.0f, float3(1, 1, 1)));

	scene = new Scene();
	scene->AddBVH(topBVH);

	for (Intersectable* obj : objects)
	{
		scene->AddObject(obj);
	}

	for (LightSource* light : lights)
	{
		scene->AddLightSource(light);
	}

	rayTracer = new RayTracer(scene, 0, 5, THREADING_DISABLED, MSAA::NONE);
}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick(float deltaTime)
{
	// clear the screen to black
	//screen->Clear(0);

	x += 1;
	float sinx = 0;
	float drakeSinx = 0;
	switch (sceneType)
	{
	case(Scenes::ANIM_SETUP):
		sinx = .5f * sin(.1f * x);
		bunnyInstance->Translate(float3(sinx, 0, 0));
		bunnyInstance->RotateY(x * .1f);
		drakeSinx = .1f * sin(.5f * x);
		dragonInstance->RotateX(drakeSinx);
		dragonInstance1->RotateX(drakeSinx);
		break;
	case(Scenes::BUDDHA_SETUP):
		break;
	}

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	//RENDERING
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

	rayTracer->cam.Tick();
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
