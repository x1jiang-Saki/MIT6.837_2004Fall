#pragma once
#include "material.h"
#include "hit.h"
#include "ray.h"

class Object3D
{
	Material* _material;

public:
	Object3D()
	{
		_material = nullptr;
	}
	// the material should deleted with all scene
	~Object3D(){}

	virtual bool intersect(const Ray& ray, Hit& hit, float tmin) = 0;
};