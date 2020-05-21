#pragma once

#include "Base_Material.h"
#include "_ray.h"
#include "_hit.h"
#include "_matrix.h"
#include "_boundingbox.h"

class Grid;

class Object3D
{
public:
	Object3D() { _material = nullptr; _boundingBox = nullptr; }
	~Object3D() {}

	virtual Material* getMaterial() const { return _material; }
	virtual BoundingBox* getBoundingBox() const { return _boundingBox; }
	
	virtual bool is_triangle() const { return false; }
	virtual BoundingBox* getTriangleBoundingBox(const Matrix* m) const { return _boundingBox; }


	virtual bool intersect(const Ray& ray, Hit& hit, float tmin) = 0;
	virtual void paint() const = 0;
	
	// sets the opaqueness of each voxel which may intersect
	virtual void insertIntoGrid(Grid* grid, Matrix* matrix);

	// representation
	Material* _material;
	BoundingBox* _boundingBox;
};

class Sphere : public Object3D
{
public:
	Sphere(Vec3f center, float radius, Material* material)
	{
		_center = center;
		_radius = radius;
		_material = material;
		
		_boundingBox = new BoundingBox(
			_center - Vec3f(_radius, _radius, _radius), 
			_center + Vec3f(_radius, _radius, _radius)
		);
	}
	~Sphere(){}

	bool intersect(const Ray& ray, Hit& hit, float tmin);
	void paint() const;
	void insertIntoGrid(Grid* grid, Matrix* matrix);

private:
	Vec3f _center;
	float _radius;
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
	void paint() const;

	// no need to draw the box
	BoundingBox* getBoundingBox() const { return nullptr; }

	void insertIntoGrid(Grid* grid, Matrix* matrix);

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

		_boundingBox = new BoundingBox(
			Vec3f(min2(_pointA.x(), _pointB.x()), min2(_pointA.y(), _pointB.y()), min2(_pointA.z(), _pointB.z())),
			Vec3f(max2(_pointA.x(), _pointB.x()), max2(_pointA.y(), _pointB.y()), max2(_pointA.z(), _pointB.z()))
		);
		_boundingBox->Extend(_pointC);
	}
	~Triangle(){}

	// //use a special case for transformed triangles
	BoundingBox* getTriangleBoundingBox(const Matrix* m) const;
	bool is_triangle() const { return true; }
	
	bool intersect(const Ray& ray, Hit& hit, float tmin);
	void paint() const;
	void insertIntoGrid(Grid* grid, Matrix* matrix);

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

		_material = object->getMaterial();
		_boundingBox = object->getBoundingBox();
	}
	~Transform(){}

	bool intersect(const Ray& ray, Hit& hit, float tmin);
	void paint() const;
	void insertIntoGrid(Grid* grid, Matrix* matrix);

	// the BB of transform need to be recalculate
	virtual BoundingBox* getBoundingBox() const;


private:
	Matrix _matrix;
	Object3D* _object;
};

class Group : public Object3D
{
public:
	Group(int objNum)
	{
		num_objs = objNum;
		_objects = new Object3D * [num_objs];

		_boundingBox = new BoundingBox(
			Vec3f(INFINITY, INFINITY, INFINITY),
			Vec3f(-INFINITY, -INFINITY, -INFINITY)
		);
	}
	~Group()
	{
		delete[] _objects;
	}

	bool intersect(const Ray& ray, Hit& hit, float tmin);
	void paint() const;
	void insertIntoGrid(Grid* grid, Matrix* matrix);

	void addObject(int index, Object3D* obj);
private:
	int num_objs;
	Object3D** _objects;
};
