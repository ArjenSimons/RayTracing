#include "precomp.h"
#include "RayTracer.h"

RayTracer::RayTracer()
{
}

RayTracer::~RayTracer()
{
}

void RayTracer::SetScene(Scene scene) {
	m_scene = scene;
}