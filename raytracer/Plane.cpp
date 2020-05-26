#include "Object3D.h"
#include <GL/gl.h>
#include "_raytracing_stats.h"

// n*P = d
bool Plane::intersect(const Ray& ray, Hit& hit, float tmin)
{
	RayTracingStats::IncrementNumIntersections();

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

void Plane::paint()
{
	_material->glSetMaterial();

	// The first basis vector may be obtained by
	// taking the cross product between n and another vector v. 
	// Any vector v will do the trick, as long as it is not parallel to n.
	// So you can always use v=(1,0,0) except when n is along the x axis, in which case you can use v=(0,1,0).

	Vec3f v = (_normal == Vec3f(1, 0, 0)) ? Vec3f(0, 1, 0) : Vec3f(1, 0, 0);

	Vec3f b1, b2;
	Vec3f::Cross3(b1, v, _normal);
	Vec3f::Cross3(b2, _normal, b1);

	// Display a rectangle from (-big, -big) to (big, big) in this 2D basis
	// big should probably be < 1000000)
	const int INF = 10000;

	Vec3f center = _d * _normal;
	Vec3f pos[4] = { center + INF * b2, center + INF * b1, center - INF * b2, center - INF * b1 };

	glBegin(GL_QUADS);
	glNormal3f(_normal.x(), _normal.y(), _normal.z());
	glVertex3f(pos[0].x(), pos[0].y(), pos[0].z());
	glVertex3f(pos[1].x(), pos[1].y(), pos[1].z());
	glVertex3f(pos[2].x(), pos[2].y(), pos[2].z());
	glVertex3f(pos[3].x(), pos[3].y(), pos[3].z());
	glEnd();
}

void Plane::insertIntoGrid(Grid* grid, Matrix* matrix) {
	grid->infinitePrimitives.push_back(this);
}
