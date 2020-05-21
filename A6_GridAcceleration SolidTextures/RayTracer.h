#pragma once

#include "_scene_parser.h"
#include "Grid.h"
extern bool shadows;
extern bool shade_back;
extern bool visualize_grid;
extern int nx, ny, nz;

class RayTracer {
public:
	RayTracer(SceneParser* scene, int maxBounces, float minWeight)
	{
		_scene = scene;
		max_bounces = maxBounces;
		min_weight = minWeight;

		_grid = nullptr;
		if (nx != 0)
		{
			_grid = new Grid(scene->getGroup()->getBoundingBox(), nx, ny, nz);
			scene->getGroup()->insertIntoGrid(_grid, nullptr);
		}
	}
	~RayTracer()
	{
		delete[] _grid;
	}

	Grid* getGrid() { return _grid; }

	Vec3f RayCast(Ray& ray, float tmin, int bounces, float weight, Hit& hit);
	Vec3f RayCastFast(Ray& ray, float tmin, int bounces, float weight, Hit& hit);
	Vec3f traceRay(Ray& ray, float tmin, int bounces, float weight, Hit& hit);

	SceneParser* _scene;
	int max_bounces;
	float min_weight;
	Grid* _grid;
};
