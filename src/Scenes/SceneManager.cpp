#include "precomp.h"
#include "SceneManager.h"

Scene * SceneManager::AnimationTest() {
	//Textures
	shared_ptr<ColorTexture> colorTexture = make_shared<ColorTexture>(Color(1, 1, 1));

	shared_ptr<Mesh> dragonMesh = make_shared<Mesh>("res/dragon.obj");
	shared_ptr<Mesh> bunnyMesh = make_shared<Mesh>("res/bunny.obj");

	Model * model = new Model(float3(0, -1, 4), 2, dragonMesh, SOLID, Material(Color(.45, .12, .12), colorTexture));
	Model * bunnyModel = new Model(float3(-3, -1, 3), 5, bunnyMesh, SOLID, Material(Color(.12, .45, .15), colorTexture));

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

	PointLight* pointLight = new PointLight(float3(-1, 3, 0), 10, float3(1, 1, 1));
	DirectionalLight* directionalLight = new DirectionalLight(float3(0, 0, 14), float3(.2f, -.8f, -.1f), 1.0f, float3(1, 1, 1));

	vector<Intersectable*> objects;
	vector<LightSource*> lights = { pointLight, directionalLight };

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

Scene* SceneManager::CornellBox() {
	// Colortextures
	shared_ptr<ColorTexture> colorTexture = make_shared<ColorTexture>(Color(1, 1, 1));

	// Specular material and diffuse material.
	Material mirrorMat = Material(Color(1, 0.9f, 0.9f), colorTexture, 1.f);
	Material glassMat = Material(Color(.95f, .95f, .95f), colorTexture);
	Material offWhiteMat = Material(Color(.85f, .85f, .85f), colorTexture);
	Material redMat = Material(Color(.95f, .1f, .2f), colorTexture);
	Material blueMat = Material(Color(.1f, .2f, .95f), colorTexture);
	Material blackMat = Material(Color(0, 0, 0), colorTexture);

	// Four walls, ceiling, floor.
	Plane* floor = new Plane(float3(0, -3, 0), float3(0, 1, 0), SOLID, offWhiteMat);
	Plane* ceiling = new Plane(float3(0, 5, 0), float3(0, -1, 0), SOLID, offWhiteMat);
	Plane* back_wall = new Plane(float3(0, 0, 7), float3(0, 0, -1), SOLID, offWhiteMat);
	Plane* left_wall = new Plane(float3(-4, 0, 0), float3(1, 0, 0), SOLID, redMat);
	Plane* right_wall = new Plane(float3(4, 0, 0), float3(-1, 0, 0), SOLID, blueMat);
	Plane* fourth_wall = new Plane(float3(0, 0, -1), float3(0, 0, 1), SOLID, blackMat);


	// Two spheres, one pure specular, one dielectric.
	Sphere* mirrorSphere = new Sphere(float3(-2, -2, 5), 1.f, SOLID, mirrorMat);
	Sphere* glassSphere = new Sphere(float3(2, -2, 4), 1.f, GLASS, glassMat);

	// TODO: For the path tracer, One area light.
	PointLight * pointLight = new PointLight(float3(0, 4.5f, 4), 20, float3(1, 1, 1));
	//DirectionalLight* directionalLight = new DirectionalLight(float3(0, 0, 14), float3(.2f, -.8f, -.1f), 1.0f, float3(1, 1, 1));

	// TODO: Add objects, lights, bvh
	vector<Intersectable*> objects = { floor, ceiling, back_wall, left_wall, right_wall, fourth_wall, mirrorSphere, glassSphere };
	vector<LightSource*> lights = { pointLight };

	Scene* scene = new Scene(objects, lights);

	return scene;
}

Scene * SceneManager::CornellBoxAreaLight() {
	Sphere* areaLight = new Sphere(float3(0, 5, 4), 2.f, LIGHT, Material(Color(1,1,1), 10));

	Scene* scene = CornellBox();
	scene->AddAreaLight(areaLight);
	return scene;
}

Scene* SceneManager::SibenikCathedral() {
	shared_ptr<ColorTexture> colorTexture = make_shared<ColorTexture>(Color(1, 1, 1));

	shared_ptr<Mesh> sibenikMesh = make_shared<Mesh>("res/sibenik.obj");
	Model* sibenikModel = new Model(float3(0, 0, 0), 1, sibenikMesh, SOLID, Material(Color(1, 1, 1), colorTexture));
	BVH* sibenikBVH = new BVH(sibenikModel->GetTriangles(), sibenikModel->GetTriangles()->size(), sibenikModel->GetTranslation(), true, .1f, 0.05f);

	sibenikBVH->ConstructBVH();

	BVHInstance* sibenikInstance = new BVHInstance(sibenikBVH);
	vector<BVHInstance*>* bvhs = new vector<BVHInstance*>;
	bvhs->push_back(sibenikInstance);

	TopLevelBVH* topBVH = new TopLevelBVH(bvhs);

	PointLight* pointLight = new PointLight(float3(0, 0, 0), 50, float3(1, 1, 1));
	DirectionalLight* directionalLight = new DirectionalLight(float3(0, 0, 14), float3(.2f, -.8f, -.1f), 1.0f, float3(1, 1, 1));


	vector<Intersectable*> objects = {};

	vector<LightSource*> lights = { pointLight };

	Scene* scene = new Scene(objects, lights, topBVH);

	return scene;
}

Scene * SceneManager::SpatialBvhTest() {
	shared_ptr<ColorTexture> colorTexture = make_shared<ColorTexture>(Color(1, 1, 1));
	shared_ptr<Mesh> mesh = make_shared<Mesh>("res/dragon.obj");
	Model* model = new Model(float3(0, 0, 3), 5, mesh, SOLID, Material(Color(.45, .12, .12), colorTexture));

	BVH* bvh = new BVH(model->GetTriangles(), model->GetTriangles()->size(), model->GetTranslation(), true, .1f, .05f);

	printf("\n triangle count: %i\n", model->GetTriangles()->size());

	bvh->ConstructBVH();

	BVHInstance* instance = new BVHInstance(bvh);
	instance->RotateY(90);

	vector<BVHInstance*>* bvhs = new vector<BVHInstance*>;

	bvhs->push_back(instance);

	TopLevelBVH* topBVH = new TopLevelBVH(bvhs);

	Sphere* redSphere = new Sphere(float3(-2, -2, 5), 1.f, SOLID, Material(Color(.45, .12, .12), colorTexture));

	// TODO: Add objects, lights, bvh
	vector<Intersectable*> objects = { };
	PointLight* pointLight = new PointLight(float3(0, 0, -4), 10, float3(1, 1, 1));

	vector<LightSource*> lights = { pointLight };

	Scene* scene = new Scene(objects, lights, topBVH);
	return scene;
}