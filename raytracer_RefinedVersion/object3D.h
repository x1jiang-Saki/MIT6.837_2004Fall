#pragma once
#include "material.h"
#include "hit.h"
#include "ray.h"

class Object3D
{
protected:
	Material* _material;

public:
	Object3D()
	{
		_material = nullptr;
	}
	// the material should deleted with all scene
	~Object3D(){}

	// tmin is used to restrict the range of intersection
	// tmin depends on the type of camera (see below)
	// and is not modified by the intersection routine
	virtual bool intersect(const Ray& ray, Hit& hit, float tmin) = 0;
};

class Sphere : public Object3D
{
	float _center;
	float _radius;
public:
	Sphere(float center, float radius, Material* material)
	{
		_center = center;
		_radius = radius;
		_material = material;
	}
	~Sphere(){}

	virtual bool intersect(const Ray& ray, Hit& hit, float tmin);
};

class Group : public Object3D
{
	Object3D** _objects;
	int _objsNum;

public:
	Group(int objsNum)
	{
		_objsNum = objsNum;
		_objects = new Object3D * [_objsNum];
	}
	~Group()
	{
		delete[] _objects;
	}

	void addObject(int index, Object3D* obj);

	virtual bool intersect(const Ray& ray, Hit& hit, float tmin);
};