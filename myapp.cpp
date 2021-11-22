#include "precomp.h"
#include "myapp.h"
#include "RayTracer.h"
#include "Plane.h"

TheApp* CreateApp() { return new MyApp(); }

RayTracer* rayTracer;

Plane plane(float3(0, -3, 0), float3(0, 1, 0));

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void MyApp::Init()
{
	// anything that happens only once at application start goes here

	Scene scene = Scene();


	scene.AddObject(&plane);

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
	// print something to the console window
	//printf("hello world!\n");
	// plot some colors

	std::vector<std::vector<float3>> renderBuffer = rayTracer->Render();

	//std::cout << std::bitset<32>((255 << 24) + (255 << 16) + (255 << 8) + (255)) << std::endl;
	//std::cout << std::bitset<32>(INT_MAX) << std::endl;

	for (int i = 0; i <  SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		screen->Plot(i, j, ((int)(renderBuffer[i][j].x * 255) << 16) + ((int)(renderBuffer[i][j].y * 255) << 8) + ((int)(renderBuffer[i][j].z * 255)));
		//screen->Plot(i, j, (255 << 16) + (255 << 8) + (255));
	}

	/*for(int red = 0; red < 256; red++) for(int green = 0; green < 256; green++)
	{
		int x = red, y = green;
		screen->Plot(x + 200, y + 100, (red << 16) + (green << 8));
	}*/
	// plot a white pixel in the bottom right corner
	screen->Plot(SCRWIDTH - 2, SCRHEIGHT - 2, 0xffffff);

	std::cout << 1 / deltaTime << std::endl;
}