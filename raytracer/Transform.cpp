#include "Object3D.h"
#include "_matrix.h"

bool Transform::intersect(const Ray& ray, Hit& hit, float tmin)
{
	// move the ray from world space to object space
	Vec3f ro = ray.getOrigin();
	Vec3f rd = ray.getDirection();
	Matrix m_I = _matrix;
	if (m_I.Inverse()) 
	{
		m_I.Transform(ro);
		m_I.TransformDirection(rd);
		// don't normalize direction
		// then tOS = tWS

		Ray osRay(ro, rd);
		if (_object->intersect(osRay, hit, tmin)) 
		{
			Vec3f normal = hit.getNormal();
			Matrix m_IT = m_I;
			m_IT.Transpose();
			m_IT.TransformDirection(normal); //to world space
			normal.Normalize();
			hit.set(hit.getT(), hit.getMaterial(), normal, ray);
			return true;
		}
	}
	return false;
}