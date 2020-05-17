#include "Base_Object3D.h"

bool Sphere::intersect(const Ray& ray, Hit& hit, float tmin)
{
	// Algebraic: quadratic equation of one unknown
	/*
	// float a = 1.0f; the direcction is always normalized
	float b = 2.0 * ray.getOrigin().Dot3(ray.getDirection());
	// ray.getOriginz().Length() also have the right result, but the origin is a point, use Dot3 is more reasonable
	float c = ray.getOrigin().Dot3(ray.getOrigin());

	// quadratic formula
	float d2 = b * b - 4.0 * c;
	if (d2 >= 0)
	{
		float d = sqrt(d2);
		float tA = (-b - d) / 2.0;
		float tB = (-b + d) / 2.0;
		// it's hard to determine whether the origin is inside the sphere
		if (tA >= tmin && tA < hit.getT())
		{
			hit.set(tA, _material, ray);
			return true;
		}
	}
	return false;
	*/

	// Geometric: can decide is the origin inside the sphere
	Vec3f ro = ray.getOrigin() - _center;
	float tp = -ro.Dot3(ray.getDirection());
	float d = sqrt(ro.Dot3(ro) - tp * tp);
	if (d > _radius)
		return false;
	
	float tt = sqrt(_radius * _radius - d * d);
	float tA = tp - tt;
	float tB = tp + tt;
	if (tA < tmin)
	{
		if (tB > tmin && tB < hit.getT())
		{
			hit.set(tB, _material, ray);
			return true;
		}
		else
			return false;
	}
	else if(tA > tmin && tA < hit.getT())
	{
		hit.set(tA, _material, ray);
		return true;
	}
	
	return false;
}

bool Group::intersect(const Ray& ray, Hit& hit, float tmin)
{
	bool flag = false;
	for (int i = 0; i < num_objs; i++)
	{
		if (_objects[i]->intersect(ray, hit, tmin))
			flag = true;
	}
	return flag;
}

// only used in _scene_parser.h
void Group::addObject(int index, Object3D* obj)
{
	_objects[index] = obj;
}