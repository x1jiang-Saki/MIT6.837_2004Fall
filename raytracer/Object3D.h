#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_
#include "Material.h"
#include "_ray.h"
#include "_hit.h"
#include "_matrix.h"
#include "_boundingbox.h"
#include <vector>

enum ObjectType {
	SphereObj, 
	PlaneObj, 
	TriangleObj, 
	Grid,
	Group
};

class Object3D
{
protected:
	Material* _material;
	BoundingBox* _boundingBox;
	ObjectType _type;
public:
	Object3D()
	{
		_material = nullptr;
		_boundingBox = nullptr;
	}
	~Object3D() {}

	virtual BoundingBox* getBoundingBox() const { return _boundingBox; }

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
		_type = ObjectType::SphereObj;
		_boundingBox = new BoundingBox(
			Vec3f(_center - Vec3f(_radius, _radius, _radius)),
			Vec3f(_center + Vec3f(_radius, _radius, _radius))
		);
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
		_type = ObjectType::Group;
		_boundingBox = new BoundingBox(
			Vec3f(INFINITY, INFINITY, INFINITY),
			Vec3f(-INFINITY, -INFINITY, -INFINITY)
		);
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
		_type = ObjectType::PlaneObj;
		// To avoid creating an infinite bounding box, 
		// don't try to compute the bounding box of a Plane 
		// Don't include this infinite primitive when computing the bounding box of a group. 
		_boundingBox = nullptr;
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
		_type = ObjectType::TriangleObj;
		_boundingBox = new BoundingBox(
			Vec3f(INFINITY, INFINITY, INFINITY),
			Vec3f(-INFINITY, -INFINITY, -INFINITY)
		);
		_boundingBox->Extend(_a);
		_boundingBox->Extend(_b);
		_boundingBox->Extend(_c);
	}
	~Triangle(){}

	Vec3f getVertex(int index) const
	{
		switch (index)
		{
		case 0:return _a;
		case 1:return _b;
		case 2:return _c;
		default: assert(0); return Vec3f();
		}
	}

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

		_boundingBox = new BoundingBox(
			Vec3f(INFINITY, INFINITY, INFINITY),
			Vec3f(-INFINITY, -INFINITY, -INFINITY)
		);
		if (_type != ObjectType::TriangleObj)
		{
			Vec3f bb[2] = {
				_object->getBoundingBox()->getMin(),
				_object->getBoundingBox()->getMax()
			};

			Vec3f bbVertes_trans[8];
			for (int i = 0; i < 8; i++)
			{
				float x = bb[i % 2].x();
				float y = bb[(i / 2) % 2].y();
				float z = bb[i / 4].z();
				Vec3f bbVert(x, y, z);
				_matrix.Transform(bbVert);
				bbVertes_trans[i] = bbVert;
			}

			for (int i = 0; i < 8; i++)
				_boundingBox->Extend(bbVertes_trans[i]);
		}
		else
		{
			Triangle* tri = (Triangle*)_object;
			Vec3f triVert[3] = {
				tri->getVertex(0),
				tri->getVertex(1),
				tri->getVertex(2)
			};
			for (int i = 0; i < 3; i++)
				_matrix.Transform(triVert[i]);

			for (int i = 0; i < 3; i++)
				_boundingBox->Extend(triVert[i]);
		}
	}

	virtual bool intersect(const Ray& ray, Hit& hit, float tmin);
	virtual void paint();

};

class MarchingInfo 
{
public:
	float tmin;
	int index[3];
	Vec3f t_next, d_t;
	int sign[3];
	Vec3f normal_to_cell;
	bool hit = false;
};
class Grid : public Object3D
{
	int _nx, _ny, _nz;
	std::vector<std::vector<Object3D*> > _cells;

public:
	Grid(BoundingBox* bb, int nx, int ny, int nz)
	{
		_nx = nx;
		_ny = ny;
		_nz = nz;

		_cells.resize(_nx * _ny * _ny);
		_boundingBox = bb;
		_type = ObjectType::Grid;
	}
	~Grid() {}

	virtual bool intersect(const Ray& ray, Hit& hit, float tmin);
	virtual void paint() const;

	void initializeRayMarch(MarchingInfo& mi, const Ray& ray, float tmin) const;

};
#endif // _OBJECT3D_H_