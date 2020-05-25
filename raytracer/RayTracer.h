#ifndef _RAY_TRACER_H_
#define _RAY_TRACER_H_

#include "_scene_parser.h"
// computes the radiance (color) along a ray
class RayTracer
{
	SceneParser* _scene;
	// a stopping criterion to prevent infinite recursion.
	int max_bounces;
	float cutoff_weight;
	bool _shadows;

public:
	RayTracer(SceneParser* scene, int maxBounces, float cutoffWeight, bool shadows)
	{
		_scene = scene;
		max_bounces = maxBounces;
		cutoff_weight = cutoffWeight;
		_shadows = shadows;
	}
	~RayTracer(){}

	// bounces: the current number of bounces 
	// weight: indicates the percent contribution of this ray to the final pixel color
	// indexOfRefraction: To refract rays through transparent objects
	// hit: and returns the closest intersection in hit
	Vec3f traceRay(Ray& ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit& hit) const;
};

#endif // !_RAY_TRACER_H_

