#include "object3D.h"

bool Sphere::intersect(const Ray& ray, Hit& hit, float tmin)
{
	// Algebratic
	Vec3f ro = ray.getOrigin();
	Vec3f rd = ray.getDirection();
	rd.Normalize();

	float a = rd.Dot3(rd);
	float b = 2.0 * ro.Dot3(rd);
	float c = ro.Dot3(ro) - _radius * _radius;
	float dd = b * b - 4.0 * a * c;
	if (dd >= 0.0)
	{

		float d = sqrt(dd);
		float t1 = (-b - d) / (2.0 * a); // first
		float t2 = (-b + d) / (2.0 * a); // second

		float t = INFINITY;
		// if t1 < tmin means the first intersect point is behind the ray_origin
		if (t1 >= tmin)
			t = t1;
		else if (t2 >= tmin)
			t = t2;

		// update hit
		if (t < hit.getT())
		{
			hit.set(t, _material, ray);
			return true;
		}
	}

	// no solution for the quadratic equation
	// and the intersect point is farther
	return false;
}