#include "Object3D.h"

bool Sphere::intersect(const Ray& ray, Hit& hit, float tmin)
{
	// Algebratic
	Vec3f ro = ray.getOrigin() - _center;
	Vec3f rd = ray.getDirection();

	float a = rd.Dot3(rd);
	float b = 2.0 * ro.Dot3(rd);
	float c = ro.Dot3(ro) - _radius * _radius;
	float dd = b * b - 4.0 * a * c;

	float t = INFINITY;
	if (dd >= 0.0)
	{
		float d = sqrt(dd);
		float t1 = (-b - d) / (2.0 * a); // first
		float t2 = (-b + d) / (2.0 * a); // second

		// if t1 < tmin means the first intersect point is behind the ray_origin
		if (t1 >= tmin)
			t = t1;
		else if (t2 >= tmin)
			t = t2;

		// update hit
		if (t < hit.getT())
		{
			Vec3f normal = ray.pointAtParameter(t) - _center;
			normal.Normalize();
			hit.set(t, _material, normal,ray);
			return true;
		}
	}

	// no solution for the quadratic equation
	// and the intersect point is farther
	return false;
}

//bool Sphere::intersect(const Ray& ray, Hit& hit, float tmin)
//{
//	// Geometric: can decide is the origin inside the sphere
//	Vec3f ro = ray.getOrigin() - _center;
//	float tp = -ro.Dot3(ray.getDirection());
//	float d = sqrt(ro.Dot3(ro) - tp * tp);
//	if (d > _radius)
//		return false;
//	
//	float tt = sqrt(_radius * _radius - d * d);
//	float tA = tp - tt;
//	float tB = tp + tt;
//	if (tA < tmin)
//	{
//		if (tB > tmin && tB < hit.getT())
//		{
//			Vec3f normal = ray.pointAtParameter(tB) - _center;
//			normal.Normalize();
//			hit.set(tB, _material,normal, ray);
//			return true;
//		}
//		else
//			return false;
//	}
//	else if(tA > tmin && tA < hit.getT())
//	{
//		Vec3f normal = ray.pointAtParameter(tA) - _center;
//		normal.Normalize();
//		hit.set(tA, _material, normal,ray);
//		return true;
//	}
//	
//	return false;
//}

