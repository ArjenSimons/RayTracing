#pragma once
static class PostProcessing
{
public:
	static void Vignette(float outerRadius, float smoothness, float intensity);
	static void GammaCorrection(float gamma);
	static void ChromaticAberration(int2 redOffset, int2 greenOffset, int2 blueOffset);
};

