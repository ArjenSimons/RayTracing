#include "precomp.h"
#include "SceneManager.h"

Scene * SceneManager::AnimationTest() {
	//Textures
	auto redTexture = make_shared<ColorTexture>(Color(.45, .12, .12));
	auto greenTexture = make_shared<ColorTexture>(Color(.12, .45, .15));

	shared_ptr<Mesh> dragonMesh = make_shared<Mesh>("res/dragon.obj");
	shared_ptr<Mesh> bunnyMesh = make_shared<Mesh>("res/bunny.obj");

	Model * model = new Model(float3(0, -1, 4), 2, dragonMesh, SOLID, Material(float3(1, 1, 1), redTexture));
	Model * bunnyModel = new Model(float3(-3, -1, 3), 5, bunnyMesh, SOLID, Material(float3(1, 1, 1), greenTexture));

	BVH * dragonBVH = new BVH(model->GetTriangles(), model->GetTriangles()->size(), model->GetTranslation(), true);
	BVH * bunnyBVH = new BVH(bunnyModel->GetTriangles(), bunnyModel->GetTriangles()->size(), bunnyModel->GetTranslation(), false);

	dragonBVH->ConstructBVH();
	bunnyBVH->ConstructBVH();
	
	BVHInstance * dragonInstance = new BVHInstance(dragonBVH);
	BVHInstance * dragonInstance1 = new BVHInstance(dragonBVH);
	BVHInstance * bunnyInstance = new BVHInstance(bunnyBVH);

	dragonInstance->RotateY(90);
	dragonInstance1->RotateY(-90);
	dragonInstance1->Translate(float3(-3, 0, 0));

	vector<BVHInstance*>* bvhs = new vector<BVHInstance*>;

	bvhs->push_back(dragonInstance);
	bvhs->push_back(dragonInstance1);
	bvhs->push_back(bunnyInstance);

	TopLevelBVH* topBVH = new TopLevelBVH(bvhs);

	vector<Intersectable*> objects;
	vector<LightSource*> lights;
	lights.push_back(new PointLight(float3(-1, 3, 0), 10, float3(1, 1, 1)));
	lights.push_back(new DirectionalLight(float3(0, 0, 14), float3(.2f, -.8f, -.1f), 1.0f, float3(1, 1, 1)));

	Scene * scene = new Scene(objects, lights, topBVH);

	// Add animation.
	shared_ptr<int> x = make_shared<int>(1);
	function<void(float)> animate = function<void(float)>([x, bunnyInstance, dragonInstance, dragonInstance1](float deltaTime) {
		*x += 1;
		float sinx = .5f * sin(0.1f * (*x));
		bunnyInstance->Translate(float3(sinx, 0, 0));
		bunnyInstance->RotateY(0.1f * (*x));
		float drakeSinx = .1f * sin(0.5f * (*x));
		dragonInstance->RotateX(drakeSinx);
		dragonInstance1->RotateX(drakeSinx);
	});

	scene->AddAnimations(animate);
	return scene;
}

Scene* SceneManager::BvhStressTest() {
	shared_ptr<Mesh> buddhaMesh = make_shared<Mesh>("res/buddha.obj");
	shared_ptr<ColorTexture> redTexture = make_shared<ColorTexture>(Color(.45, .12, .12));

	Model * model = new Model(float3(-15, -5, 9), 2, buddhaMesh, SOLID, Material(float3(1, 1, 1), redTexture));
	cout << "# of tris = %i\n" << model->GetTriangles()->size() * 31 * 30 << "\n";

	BVH * buddha = new BVH(model->GetTriangles(), model->GetTriangles()->size(), model->GetTranslation(), true);
	buddha->ConstructBVH();

	vector<BVHInstance*> buddhas;
	vector<BVHInstance*>* bvhs = new vector<BVHInstance*>;
	
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 31; j++)
		{
			buddhas.push_back(new BVHInstance(buddha));
			bvhs->push_back(buddhas.back());
			buddhas.back()->Translate(float3(i, 0, j));
		}
	}

	TopLevelBVH* topBVH = new TopLevelBVH(bvhs);
	vector<Intersectable*> objects;
	vector<LightSource*> lights;
	lights.push_back(new PointLight(float3(-1, 3, 0), 10, float3(1, 1, 1)));
	lights.push_back(new DirectionalLight(float3(0, 0, 14), float3(.2f, -.8f, -.1f), 1.0f, float3(1, 1, 1)));

	Scene* scene = new Scene(objects, lights, topBVH);
	return scene;
}