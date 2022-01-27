#include "precomp.h"

//TODO: Compute post processing effects on GPU
void PostProcessing::Vignette(Color renderBuffer[SCRWIDTH][SCRHEIGHT], float2 uv[SCRWIDTH][SCRHEIGHT], float outerRadius, float smoothness, float intensity)
{
	outerRadius = clamp(outerRadius, 0.0f, 1.0f);
	intensity = clamp(intensity, 0.0f, 1.0f);
	smoothness = max(0.0001f, smoothness);

	float innerRadius = outerRadius - smoothness;

	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		float2 p = uv[i][j] - .5f;
		float len = dot(p, p);
		float vignette = smoothstep(outerRadius, innerRadius, len) * intensity;

		renderBuffer[i][j] = renderBuffer[i][j] * vignette;
	}
}

void PostProcessing::GammaCorrection(Color renderBuffer[SCRWIDTH][SCRHEIGHT], float gamma)
{
	float invGamma = 1 / gamma;

	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		float x = pow(renderBuffer[i][j].value.x, invGamma);
		float y = pow(renderBuffer[i][j].value.y, invGamma);
		float z = pow(renderBuffer[i][j].value.z, invGamma);

		renderBuffer[i][j] = float3(x, y, z);
	}
}

Color tempBuffer[SCRWIDTH][SCRHEIGHT];

void PostProcessing::ChromaticAberration(Color renderBuffer[SCRWIDTH][SCRHEIGHT], int2 redOffset, int2 greenOffset, int2 blueOffset)
{
	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		float r = renderBuffer[i - redOffset.x][j - redOffset.y].value.x;
		float g = renderBuffer[i - greenOffset.x][j - greenOffset.y].value.y;
		float b = renderBuffer[i - blueOffset.x][j - blueOffset.y].value.z;

		tempBuffer[i][j] = float3(r, g, b);
	}

	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		renderBuffer[i][j] = tempBuffer[i][j];
	}
}