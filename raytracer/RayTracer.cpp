#include "RayTracer.h"
#include "_rayTree.h"

Vec3f mirror(const Vec3f& normal, const Vec3f& incoming)
{
	Vec3f N = normal, I = incoming;
	N.Normalize();
	I.Normalize();
	float t = N.Dot3(I);
	return -2 * t * N + I;
}

bool transmit(const Vec3f& normal, const Vec3f& incoming,float index_i, float index_t, Vec3f& transmitted)
{
	Vec3f N = normal, I = incoming;
	N.Normalize();
	I.Normalize();
	float cosI = N.Dot3(I);

	Vec3f M_sinI = -cosI * N + I;
	float sinI = M_sinI.Length();
	
	// Snell-Descartes Law
	float sinO = sinI * index_i / index_t;

	if (abs(sinO) > 1)
		return false;

	float cosO = sqrt(1 - sinO * sinO);
	M_sinI.Normalize();
	if (cosI < 0)
		transmitted = -cosO * N + sinO * M_sinI;
	else
		transmitted = cosO * N + sinO * M_sinI;
	return true;
}

extern bool useTransparentShadows;
Vec3f RayTracer::traceRay(Ray& ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit& hit) const
{
	if (bounces > max_bounces)
		return Vec3f(0.0, 0.0, 0.0);

	Group* objects = _scene->getGroup();
	if (objects->intersect(ray, hit, tmin))
	{
		// the main ray is visualized in gray
		if (bounces == 0)
			RayTree::SetMainSegment(ray, 0.0, hit.getT());

		Vec3f baseColor = hit.getMaterial()->getDiffuseColor();

		Vec3f ambientTerm = _scene->getAmbientLight() * baseColor;
		Vec3f resultCol = ambientTerm;

		Vec3f N = hit.getNormal();

		// SHADOW
		const float EPSILON = 0.001;
		for (int lit = 0; lit < _scene->getNumLights(); lit++)
		{
			Light* curLight = _scene->getLight(lit);
			Vec3f pos = hit.getIntersectionPoint();
			Vec3f L, lightCol;
			float distance; 
			curLight->getIllumination(pos, L, lightCol, distance);

			Vec3f mask(1.0, 1.0, 1.0);
			if (_shadows)
			{
				Ray shadowRay(hit.getIntersectionPoint(), L);
				Hit shadowHit(INFINITY, nullptr, Vec3f(0.0, 0.0, 0.0));
				if (objects->intersect(shadowRay, shadowHit, EPSILON))
				{
					// in the shadow
					if (shadowHit.getT() < distance)
					{
						mask = Vec3f(0, 0, 0);
						RayTree::AddShadowSegment(shadowRay, 0.0, shadowHit.getT());
					}

					if (useTransparentShadows)
					{
						float curT = 0.0;
						float lastT = -1;
						Vec3f lastColor = Vec3f(-1, -1, -1);
						do {
							curT = shadowHit.getT();
							if (lastColor == shadowHit.getMaterial()->getTransparentColor())
							{
								Vec3f mask_temp = Vec3f(1, 1, 1) * (1 - (curT - lastT)) + shadowHit.getMaterial()->getTransparentColor() * (curT - lastT);
								Vec3f::Mult(mask, mask, mask_temp);
							}

							lastT = curT;
							lastColor = shadowHit.getMaterial()->getTransparentColor();
						} while (objects->intersect(shadowRay, shadowHit, curT + EPSILON));
					}
				}
				else // no intersection or intersectPoint is behind the light
					RayTree::AddShadowSegment(shadowRay, 0.0, 20.0);
			}
			resultCol += hit.getMaterial()->Shade(ray, hit, L, lightCol) * mask;
		}

		// REFLECTION
		if (bounces < max_bounces && hit.getMaterial()->getReflectiveColor().Length() > 0)
		{
			Ray reflRay(hit.getIntersectionPoint(), mirror(hit.getNormal(), ray.getDirection()));
			Hit reflHit(INFINITY, nullptr, Vec3f(0.0, 0.0, 0.0));
			resultCol += traceRay(reflRay, EPSILON, bounces + 1, weight, indexOfRefraction, reflHit) * hit.getMaterial()->getReflectiveColor();
		
			RayTree::AddReflectedSegment(reflRay, 0.0, reflHit.getT());
		}

		// REFRACTION
		if (bounces < max_bounces && hit.getMaterial()->getTransparentColor().Length() > 0)
		{
			Vec3f refrDir;
			bool inside = hit.getNormal().Dot3(ray.getDirection()) > 0;
			bool check = false;

			if (inside)
				check = transmit(hit.getNormal(), ray.getDirection(), indexOfRefraction, 1.0, refrDir);
			else
				check = transmit(hit.getNormal(), ray.getDirection(), 1.0, hit.getMaterial()->getIndexOfRefraction(), refrDir);

			Ray refrRay(hit.getIntersectionPoint(), refrDir);
			Hit refrHit(INFINITY, nullptr, Vec3f(0.0, 0.0, 0.0));
			
			if (check)
			{
				if (inside)
					resultCol += traceRay(refrRay, EPSILON, bounces + 1, weight, 1.0, refrHit) * hit.getMaterial()->getTransparentColor();
				else
					resultCol += traceRay(refrRay, EPSILON, bounces + 1, weight, hit.getMaterial()->getIndexOfRefraction(), refrHit) * hit.getMaterial()->getTransparentColor();

				RayTree::AddTransmittedSegment(refrRay, 0.0, refrHit.getT());
			}
		}
		return resultCol;
	}

	return _scene->getBackgroundColor();
}