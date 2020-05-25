#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_
#include "Material.h"
#include "_ray.h"
#include "_hit.h"
#include "_matrix.h"

class Object3D
{
protected:
	Material* _material;

public:
	Object3D()
	{
		_material = nullptr;
	}
	~Object3D() {}

	// tmin is used to restrict the range of intersection
	// tmin depends on the type of camera (see below)
	// and is not modified by the intersection routine
	virtual bool intersect(const Ray& ray, Hit& hit, float tmin) = 0;
	// executes the appropriate OpenGL calls 
	// to render each object in the pre-visualization interface
	virtual void paint() = 0;
};

class Sphere : public Object3D
{
	Vec3f _center;
	float _radius;

public:
	Sphere(Vec3f center, float radius, Material* material)
	{
		_center = center;
		_radius = radius;
		_material = material;
	}
	~Sphere(){}

	virtual bool intersect(const Ray& ray, Hit& hit, float tmin);
	virtual void paint();
};

class Group : public Object3D
{
	int num_objs;
	Object3D** _objects;
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

	virtual bool intersect(const Ray& ray, Hit& hit, float tmin);
	virtual void paint();

	void addObject(int index, Object3D* obj);
};

class Plane : public Object3D
{
	Vec3f _normal;
	float _d;
public:
	Plane(Vec3f &normal, float d, Material* material)
	{
		_normal = normal;
		_normal.Normalize();
		_d = d;
		_material = material;
	}
	~Plane(){}

	virtual bool intersect(const Ray& ray, Hit& hit, float tmin);
	virtual void paint();
};

class Triangle : public Object3D
{
	Vec3f _a, _b, _c;
	Vec3f _normal;
public:
	Triangle(Vec3f& a, Vec3f& b, Vec3f& c, Material* material)
	{
		_a = a; _b = b; _c = c;
		Vec3f::Cross3(_normal, _b - _a, _c - _a);
		_normal.Normalize();

		_material = material;
	}
	~Triangle(){}

	virtual bool intersect(const Ray& ray, Hit& hit, float tmin);
	virtual void paint();

};

class Transform : public Object3D
{
	Object3D* _object;
	Matrix _matrix;
public:
	Transform(Matrix& matrix, Object3D* object)
	{
		_object = object;
		_matrix = matrix;
	}

	virtual bool intersect(const Ray& ray, Hit& hit, float tmin);
	virtual void paint();

};

#endif // _OBJECT3D_H_