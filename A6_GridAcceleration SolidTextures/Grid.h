#pragma once
#include "Base_Object3D.h"
#include <vector>
#include "MarchingInfo.h"
#include <set>

class Grid : public Object3D
{
public:
	Grid(BoundingBox* bb, int nx, int ny, int nz)
	{
		_boundingBox = bb;
		_nx = nx;
		_ny = ny;
		_nz = nz;
		_opaque.resize(_nx * _ny * _nz);
	}
	~Grid(){}

	bool intersect(const Ray& ray, Hit& hit, float tmin);
	void paint() const;
	bool intersectShadowRay(const Ray& ray, Hit& hit, float tmin);


	void initializeRayMarch(MarchingInfo& mInfo, const Ray& ray, float tmin) const;

	int _nx;
	int _ny;
	int _nz;
	std::vector<std::vector<Object3D* > > _opaque;
	std::set<Object3D*> is_intersected;
	std::vector<Object3D*> infinitePrimitives;
};
