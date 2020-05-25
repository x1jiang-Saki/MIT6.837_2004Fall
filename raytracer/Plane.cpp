#include "Object3D.h"

// n*P = d
bool Plane::intersect(const Ray& ray, Hit& hit, float tmin)
{
	Vec3f ro = ray.getOrigin();
	Vec3f rd = ray.getDirection();

	float d = _normal.Dot3(rd);
	// parallel
	if (d == 0)
		return false;

	float t = (_d - _normal.Dot3(ro)) / d;
	if (t > tmin && t < hit.getT())
	{
		hit.set(t, _material, _normal, ray);
		return true;
	}
	return false;
}