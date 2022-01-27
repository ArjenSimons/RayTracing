#pragma once
static class PostProcessing
{
public:
	static void Vignette(Color renderBuffer[SCRWIDTH][SCRHEIGHT], float2 uv[SCRWIDTH][SCRHEIGHT], float outerRadius, float smoothness, float intensity);
	static void GammaCorrection(Color renderBuffer[SCRWIDTH][SCRHEIGHT], float gamma);
	static void ChromaticAberration(Color renderBuffer[SCRWIDTH][SCRHEIGHT], int2 redOffset, int2 greenOffset, int2 blueOffset);
};

