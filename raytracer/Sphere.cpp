#include "Object3D.h"
#include <GL/gl.h>
#include "_raytracing_stats.h"

const float PI = 3.1415926;
bool Sphere::intersect(const Ray& ray, Hit& hit, float tmin)
{
	RayTracingStats::IncrementNumIntersections();
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

extern int theta_steps;
extern int phi_steps;
extern bool gouraud;
void Sphere::paint()
{
	_material->glSetMaterial();

	float delta_theta = 2 * PI / theta_steps;
	float delta_phi = PI / phi_steps;

	// initialize all the position for points
	Vec3f* points = new Vec3f[theta_steps * (phi_steps + 1)];
	for (int p = 0; p < phi_steps + 1; p++)
	{
		for (int t = 0; t < theta_steps; t++)
		{
			// from top to button, one circle to one circle
			int index = theta_steps * p + t;

			float theta = t * delta_theta;
			float phi = p * delta_phi;
			Vec3f pos = Vec3f(cos(theta) * sin(phi), cos(phi), sin(theta) * sin(phi));
			points[index] = _center + pos * _radius;
			//normal.push_back(pos);
		}
	}

	glBegin(GL_QUADS);
	for (int p = 0; p < phi_steps; p++)
	{
		for (int t = 0; t < theta_steps; t++)
		{
			int index[4] = {
				// clock-wise
				theta_steps * p + t ,
				theta_steps * (p + 1) + t,
				theta_steps * (p + 1) + (t + 1) % theta_steps ,
				theta_steps * p + (t + 1) % theta_steps
			};
			Vec3f normal;
			if (!gouraud)
			{
				Vec3f::Cross3(normal, points[index[0]] - points[index[1]], points[index[2]] - points[index[1]]);
				if (normal.Length() == 0)
				{
					Vec3f::Cross3(normal, points[index[1]] - points[index[2]], points[index[3]] - points[index[2]]);
				}
			}
			for (int i = 0; i < 4; i++)
			{
				if (gouraud)
				{
					normal = points[index[i]] - _center;
				}
				normal.Normalize();
				glNormal3f(normal.x(), normal.y(), normal.z());

				glVertex3f(points[index[i]].x(), points[index[i]].y(), points[index[i]].z());
			}
		}
	}

	glEnd();
}

void Sphere::insertIntoGrid(Grid* grid, Matrix* matrix) {
	if (matrix) {
		Object3D::insertIntoGrid(grid, matrix);
		return;
	}
	BoundingBox* grid_bb = grid->getBoundingBox();
	Vec3f grid_min = grid_bb->getMin();
	Vec3f grid_max = grid_bb->getMax();
	int nx = grid->_nx;
	int ny = grid->_ny;
	int nz = grid->_nz;
	float cellx = (grid_max - grid_min).x() / float(nx);
	float celly = (grid_max - grid_min).y() / float(ny);
	float cellz = (grid_max - grid_min).z() / float(nz);
	float diagonal = sqrt(cellx * cellx + celly * celly + cellz * cellz);
	for (int k = 0; k < nz; ++k) {
		for (int j = 0; j < ny; ++j) {
			for (int i = 0; i < nx; ++i) {
				float x = (i + 0.5) * cellx;
				float y = (j + 0.5) * celly;
				float z = (k + 0.5) * cellz;
				if ((Vec3f(x, y, z) - (_center - grid_min)).Length() < _radius + diagonal / 2) {
					int index = nx * ny * k + nx * j + i;
					grid->_cells[index].push_back(this);
				}
			}
		}
	}
}
