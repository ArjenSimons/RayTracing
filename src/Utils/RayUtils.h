#pragma once


float3 Reflect(const float3& dir, const float3& N) {
	return dir - 2 * dot(dir, N) * N;
}

struct DielectricTerms {
	float n1;
	float n2;
	float indexRatio;
	float k;
	float cosi;
	float energy;

	// Calculated values used for dielectrics.
	DielectricTerms(const Ray& ray, const Intersection& i) {
		n1 = RefractionIndex(ray.substance);
		n2 = RefractionIndex(i.sTo);

		indexRatio = n1 / n2;
		float3 D = ray.Dir * -1;
		cosi = dot(i.normal, D);

		k = 1 - ((indexRatio * indexRatio) * (1 - cosi * cosi));
		energy = ray.e * exp(-Absorption(ray.substance) * i.t);
	}
};

float3 Refract(const float3& dir, const float3& N, DielectricTerms terms) {

	float3 dir = terms.indexRatio * dir + N * (terms.indexRatio * terms.cosi - sqrt(terms.k));
	return dir;
}

// Calculates reflection and transmission coefficients
void Fresnel(DielectricTerms terms, float& Fr, float& Ft) {
	float sint = terms.indexRatio * sqrtf(max(0.f, 1 - terms.cosi * terms.cosi));
	float cost = sqrt(max(0.0f, 1 - sint * sint));
	float cosi = fabsf(terms.cosi);

	float n1TimesAngle1 = terms.n1 * cosi;
	float n1TimesAngle2 = terms.n1 * cost;
	float n2TimesAngle1 = terms.n2 * cosi;
	float n2TimesAngle2 = terms.n2 * cost;

	float sPolarizedSqrd = (n1TimesAngle1 - n2TimesAngle2) / (n1TimesAngle1 + n2TimesAngle2);
	float pPolarizedSqrd = (n1TimesAngle2 - n2TimesAngle1) / (n1TimesAngle2 + n2TimesAngle1);

	Fr = terms.energy * .5f * (sPolarizedSqrd * sPolarizedSqrd + pPolarizedSqrd * pPolarizedSqrd);
	Ft = terms.energy - Fr;
}