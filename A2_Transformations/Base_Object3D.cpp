#include "Base_Object3D.h"
#include "_matrix.h"

// the determination of the matrix
float det2(float a, float b,
	float c, float d) {
	return a * d - b * c;
}
float det3(float a1, float a2, float a3,
	float b1, float b2, float b3,
	float c1, float c2, float c3) {
	return
		a1 * det2(b2, b3, c2, c3)
		- b1 * det2(a2, a3, c2, c3)
		+ c1 * det2(a2, a3, b2, b3);
}

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
	float rd_length = ray.getDirection().Length();
	float tp = -ro.Dot3(ray.getDirection()) / rd_length;
	
	float d = sqrt(ro.Dot3(ro) - tp * tp);
	if (d > _radius)
		return false;
	
	float tt = sqrt(_radius * _radius - d * d);
	float tA = (tp - tt) / rd_length;
	float tB = (tp + tt) / rd_length;
	if (tA < tmin)
	{
		if (tB > tmin && tB < hit.getT())
		{
			Vec3f normal = ro + tB * ray.getDirection();
			normal.Normalize();
			hit.set(tB, _material, normal, ray);
			return true;
		}
		else
			return false;
	}
	else if(tA > tmin && tA < hit.getT())
	{
		Vec3f normal = ro + tA * ray.getDirection();
		normal.Normalize();
		hit.set(tA, _material, normal, ray);
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

bool Plane::intersect(const Ray& ray, Hit& hit, float tmin)
{
	if (_normal.Dot3(ray.getDirection()) == 0)
		return false;
	float t = (_d - _normal.Dot3(ray.getOrigin())) / _normal.Dot3(ray.getDirection());
	if (t > tmin && t < hit.getT())
	{
		hit.set(t, _material, _normal, ray);
		return true;
	}
	return false;
}

bool Triangle::intersect(const Ray& ray, Hit& hit, float tmin) {
	Vec3f ro = ray.getOrigin();
	Vec3f rd = ray.getDirection();
	float abx = _pointA.x() - _pointB.x();
	float aby = _pointA.y() - _pointB.y();
	float abz = _pointA.z() - _pointB.z();
	float acx = _pointA.x() - _pointC.x();
	float acy = _pointA.y() - _pointC.y();
	float acz = _pointA.z() - _pointC.z();
	float aox = _pointA.x() - ro.x();
	float aoy = _pointA.y() - ro.y();
	float aoz = _pointA.z() - ro.z();
	float rdx = rd.x();
	float rdy = rd.y();
	float rdz = rd.z();
	float A = det3(abx, acx, rdx, aby, acy, rdy, abz, acz, rdz);
	float beta = det3(aox, acx, rdx, aoy, acy, rdy, aoz, acz, rdz) / A;
	float gamma = det3(abx, aox, rdx, aby, aoy, rdy, abz, aoz, rdz) / A;
	if (beta > 0 && gamma > 0 && (beta + gamma) < 1) {
		float t = det3(abx, acx, aox, aby, acy, aoy, abz, acz, aoz) / A;
		if (t > tmin && t < hit.getT()) {
			hit.set(t, _material, _normal, ray);
			return true;
		}
	}
	return false;
}

//bool Triangle::intersect(const Ray& ray, Hit& hit, float tmin)
//{
//	// Cramer's Rule
//	Vec3f ab = _pointA - _pointB;
//	Vec3f ac = _pointA - _pointC;
//	Vec3f rd = ray.getDirection();
//	Vec3f ao = _pointA - ray.getOrigin();
//
//	float A = det3(
//		ab.x(), ac.x(), rd.x(),
//		ab.y(), ac.y(), rd.y(),
//		ab.z(), ac.z(), rd.z()
//	);
//	float beta = det3(
//		ao.x(), ac.x(), rd.x(),
//		ao.y(), ac.y(), rd.y(),
//		ao.z(), ac.z(), rd.z()
//	) / A;
//	float gamma = det3(
//		ab.x(), ao.x(), rd.x(),
//		ab.y(), ao.y(), rd.y(),
//		ab.z(), ao.z(), rd.z()
//	) / A;
//	float t = det3(
//		ab.x(), ac.x(), ao.x(),
//		ab.y(), ac.y(), ao.y(),
//		ab.z(), ac.z(), ao.z()
//	) / A;
//
//	if (beta > 0 && gamma > 0 && (beta + gamma) < 1)
//	{
//		if(t > tmin && t > hit.getT())
//		{
//			hit.set(t, _material, _normal, ray);
//			return true;
//		}
//	}
//	return false;
//}

bool Transform::intersect(const Ray& ray, Hit& hit, float tmin)
{
	// move the ray frome World Space to the ObjectSpace
	Vec3f newOrigin = ray.getOrigin();
	Vec3f newDirection = ray.getDirection();

	Matrix matrix_I = _matrix;
	if (matrix_I.Inverse())
	{
		matrix_I.Transform(newOrigin);
		matrix_I.TransformDirection(newDirection);

		// if direction is normalized ,the tOS != tWS
		// since we need it to be equal, we are not going to normalize the direction
		Ray rayOS(newOrigin, newDirection);
		if (_object->intersect(rayOS, hit, tmin))
		{
			// calculate the normal
			Vec3f newNormal = hit.getNormal();
			Matrix matrix_IT = matrix_I;
			matrix_IT.Transpose(matrix_IT);
			matrix_IT.TransformDirection(newNormal);
			newNormal.Normalize();

			hit.set(hit.getT(), hit.getMaterial(), newNormal, ray);
			return true;
		}
	}
	return false;
}

// only used in _scene_parser.h
void Group::addObject(int index, Object3D* obj)
{
	_objects[index] = obj;
}