#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_
#include "Base_Material.h"
#include "_ray.h"
#include "_hit.h"
#include "_matrix.h"

class Object3D
{
public:
	Object3D(){ _material = nullptr; }
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
	~Sphere(){}

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

class Plane : public Object3D
{
public:
	Plane(Vec3f normal, float d, Material* material)
	{
		_normal = normal;
		_normal.Normalize();
		_d = d;
		_material = material;
	}
	~Plane(){}
	bool intersect(const Ray& ray, Hit& hit, float tmin);

private:
	float _d; // the offset from the origin
	Vec3f _normal; // the normal of the plane
};

class Triangle : public Object3D
{
public:
	Triangle(Vec3f pointA, Vec3f pointB, Vec3f pointC, Material* material)
	{
		_pointA = pointA;
		_pointB = pointB;
		_pointC = pointC;
		_material = material;
		// the usual convention that counter-clockwise vertex ordering indicates the outward-facing side.
		Vec3f::Cross3(_normal, pointB - pointA, pointC - pointA);
		_normal.Normalize();
	}
	~Triangle(){}

	bool intersect(const Ray& ray, Hit& hit, float tmin);

private:
	Vec3f _pointA;
	Vec3f _pointB;
	Vec3f _pointC;
	Vec3f _normal;
};

class Transform : public Object3D
{
public:
	Transform(Matrix &matrix, Object3D* object)
	{
		_matrix = matrix;
		_object = object;
	}
	~Transform(){}

	bool intersect(const Ray& ray, Hit& hit, float tmin);

private:
	Matrix _matrix;
	Object3D* _object;
};
#endif // _OBJECT3D_H_