#include "Object3D.h"
#include "_matrix.h"
float det3x3(float a1, float a2, float a3,
	float b1, float b2, float b3,
	float c1, float c2, float c3);

bool Triangle::intersect(const Ray& ray, Hit& hit, float tmin) {
	Vec3f ro = ray.getOrigin();
	Vec3f rd = ray.getDirection();
	float abx = _a.x() - _b.x();
	float aby = _a.y() - _b.y();
	float abz = _a.z() - _b.z();
	float acx = _a.x() - _c.x();
	float acy = _a.y() - _c.y();
	float acz = _a.z() - _c.z();
	float aox = _a.x() - ro.x();
	float aoy = _a.y() - ro.y();
	float aoz = _a.z() - ro.z();
	float rdx = rd.x();
	float rdy = rd.y();
	float rdz = rd.z();
	float A = det3x3(abx, acx, rdx, aby, acy, rdy, abz, acz, rdz);
	float beta = det3x3(aox, acx, rdx, aoy, acy, rdy, aoz, acz, rdz) / A;
	float gamma = det3x3(abx, aox, rdx, aby, aoy, rdy, abz, aoz, rdz) / A;
	float t = det3x3(abx, acx, aox, aby, acy, aoy, abz, acz, aoz) / A;

	if (beta > 1 || beta < 0)
		return false;
	if (gamma > 1 || gamma < 0)
		return false;
	if (1 - beta - gamma > 1 || 1 - beta - gamma < 0)
		return false;
	if (t > tmin && t < hit.getT())
	{
		hit.set(t, _material, _normal, ray);
		return true;
	}
	return false;
}