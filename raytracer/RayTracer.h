#ifndef _RAY_TRACER_H_
#define _RAY_TRACER_H_

#include "_scene_parser.h"
#include "Object3D.h"

extern int nx, ny, nz;

// computes the radiance (color) along a ray
class RayTracer
{
public:
	SceneParser* _scene;
	// a stopping criterion to prevent infinite recursion.
	int max_bounces;
	float cutoff_weight;
	bool _shadows;
	Grid* _grid;

	RayTracer(SceneParser* scene, int maxBounces, float cutoffWeight, bool shadows)
	{
		_scene = scene;
		max_bounces = maxBounces;
		cutoff_weight = cutoffWeight;
		_shadows = shadows;

		if (nx != 0)
		{
			_grid = new Grid(_scene->getGroup()->getBoundingBox(), nx, ny, nz);
			_scene->getGroup()->insertIntoGrid(_grid, nullptr);
		}
		else
			_grid = nullptr;
	}
	~RayTracer(){}

	// bounces: the current number of bounces 
	// weight: indicates the percent contribution of this ray to the final pixel color
	// indexOfRefraction: To refract rays through transparent objects
	// hit: and returns the closest intersection in hit
	Vec3f traceRay(Ray& ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit& hit);
	
	Vec3f RayCast(Ray& ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit& hit);
	Vec3f RayCastFast(Ray& ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit& hit);
};

#endif // !_RAY_TRACER_H_

