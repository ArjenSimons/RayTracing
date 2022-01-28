#include "precomp.h"
#include "myapp.h"

enum class Scenes
{
	ANIM_SETUP,
	BUDDHA_SETUP
};

TheApp* CreateApp() { return new MyApp(); }

RayTracer* rayTracer;
Scene* scene;
Scenes sceneType = Scenes::ANIM_SETUP;

Color renderBuffer[SCRWIDTH][SCRHEIGHT];

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void MyApp::Init()
{
	scene = SceneManager::AnimationTest();
	rayTracer = new RayTracer(scene, 5, THREADING_ENABLED, MSAA::NONE);
}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void MyApp::Tick(float deltaTime)
{
	// clear the screen to black
	// screen->Clear(0);

	scene->Animate(deltaTime);

	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	//RENDERING
	rayTracer->Render(renderBuffer);
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	cout << "Render time:" << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << "[ms]" << endl;

	chrono::steady_clock::time_point ppbegin = chrono::steady_clock::now();

	if (POSTPROCESSING) {
		if (VIGNETTING) {
			PostProcessing::Vignette(renderBuffer, rayTracer->uv, VIGNETTE_RADIUS, VIGNETTE_SMOOTHNESS, VIGNETTE_INTENSITY);
		}
		if (GAMMA_CORRECTION) {
			PostProcessing::GammaCorrection(renderBuffer, GAMMA);
		}
		if (CHROMATIC_ABERRATION) {
			PostProcessing::ChromaticAberration(renderBuffer, CHROM_ABB_R_OFFSET, CHROM_ABB_G_OFFSET, CHROM_ABB_B_OFFSET);
		}
	}

	chrono::steady_clock::time_point ppend = chrono::steady_clock::now();
	cout << "PP time:" << chrono::duration_cast<chrono::milliseconds>(ppend - ppbegin).count() << "[ms]" << endl;


	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		screen->Plot(i, j, renderBuffer[i][j].GetRGBValue());
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
	delete scene;
	delete rayTracer;
}
