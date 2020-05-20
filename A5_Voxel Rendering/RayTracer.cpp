#include "RayTracer.h"
#include "_rayTree.h"

Vec3f RayTracer::traceRay(Ray& ray, float tmin, int bounces, float weight, Hit& hit) {
	//grid shading
	if (nx != 0) {
		if (_grid->intersect(ray, hit, tmin)) {
			Vec3f pos = hit.getIntersectionPoint();
			Vec3f color = _scene->getAmbientLight() * hit.getMaterial()->getDiffuseColor();
			for (int i = 0; i < _scene->getNumLights(); i++) 
			{
				Light* light = _scene->getLight(i);
				Vec3f lightDir, lightCol;
				float distance;
				light->getIllumination(pos, lightDir, lightCol, distance);
				color += hit.getMaterial()->Shade(ray, hit, lightDir, lightCol);
			}
			return color;
		}
		else 
		{
			return _scene->getBackgroundColor();
		}
	}

	// RayTrace shading
	Group* objects = _scene->getGroup();
	if (objects->intersect(ray, hit, tmin)) {
		if (bounces == 0) {
			RayTree::SetMainSegment(ray, tmin, hit.getT());
		}
		Vec3f hitPos = hit.getIntersectionPoint();
		Material* material = hit.getMaterial();
		Vec3f color = _scene->getAmbientLight() * material->getDiffuseColor();

		// SHADOWS
		for (int i = 0; i < _scene->getNumLights(); i++) {
			Light* curLight = _scene->getLight(i);

			// now we have point light, the distance are used to calculate the attention
			// and test the visibility
			Vec3f lightDir, lightCol;
			float distance;
			curLight->getIllumination(hitPos, lightDir, lightCol, distance);
			
			Ray shadowRay(hitPos, lightDir);
			Hit shadowHit(distance);
			if (shadows) {
				if (!objects->intersect(shadowRay, shadowHit, tmin))
					color += material->Shade(ray, hit, lightDir, lightCol);
			}
			else
				color += material->Shade(ray, hit, lightDir, lightCol);
			RayTree::AddShadowSegment(shadowRay, tmin, shadowHit.getT()); 
		}

		Ray scatteredRay;
		Vec3f scatteredCol;
		// REFLECTIVE
		if (bounces < max_bounces && weight > min_weight && material->reflect(ray, hit, scatteredCol, scatteredRay)) {
			Hit hit_ref(INFINITY);
			color += scatteredCol * traceRay(scatteredRay, tmin, bounces + 1, weight * scatteredCol.Length(), hit_ref);
			RayTree::AddReflectedSegment(scatteredRay, tmin, hit_ref.getT());
		}
		//REFREACTIVE
		if (bounces < max_bounces && weight > min_weight && material->refract(ray, hit, scatteredCol, scatteredRay)) {
			Hit hit_ref(INFINITY);
			color += scatteredCol * traceRay(scatteredRay, tmin, bounces + 1, weight * scatteredCol.Length(), hit_ref);
			RayTree::AddTransmittedSegment(scatteredRay, tmin, hit_ref.getT());
		}
		return color;
	}
	else {
		return _scene->getBackgroundColor();
	}
}
