#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_
#include "Base_Material.h"
#include "_ray.h"
#include "_hit.h"

class Object3D
{
public:
	Object3D()
	{
		_material = nullptr;
	}
	~Object3D() {}

	virtual bool intersect(const Ray& ray, Hit& hit, float tmin) = 0;

	// representation
	Material* _material;
};

class Sphere : public Object3D
{
public:
	Sphere(Vec3f center, float radius, Material* material)
	{
		_center = center;
		_radius = radius;
		_material = material;
	}
	~Sphere();

	bool intersect(const Ray& ray, Hit& hit, float tmin);

private:
	Vec3f _center;
	float _radius;
};

class Group : public Object3D
{
public:
	Group(int objNum)
	{
		num_objs = objNum;
		_objects = new Object3D*[num_objs];
	}
	~Group()
	{
		delete[] _objects;
	}

	bool intersect(const Ray& ray, Hit& hit, float tmin);
	void addObject(int index, Object3D* obj);
private:
	int num_objs;
	Object3D** _objects;
};

#endif // _OBJECT3D_H_