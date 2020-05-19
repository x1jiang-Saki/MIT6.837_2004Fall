#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_

#include "_scene_parser.h"
extern bool shadows;
extern bool shade_back;

class RayTracer {
public:
	RayTracer(SceneParser* s, int maxBounces, float minWeight)
	{
		_scene = s;
		max_bounces = maxBounces;
		min_weight = minWeight;
	}
	~RayTracer() {}

	Vec3f traceRay(Ray& ray, float tmin, int bounces, float weight, Hit& hit);

	SceneParser* _scene;
	int max_bounces;
	float min_weight;
};

#endif // !_RAYTRACER_H_
