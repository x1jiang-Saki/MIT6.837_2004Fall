#include "Base_Object3D.h"
#include "_matrix.h"
#include <gl\GL.h>
#include <vector>
#include "Grid.h"
#include "_boundingbox.h"
#include "_raytracing_stats.h"

const float PI = 3.14159265358979323846;
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

void Object3D::insertIntoGrid(Grid* grid, Matrix* matrix) {
	assert(_boundingBox != nullptr);

	BoundingBox* bb = _boundingBox;
	if (matrix) {
		Vec3f obj_min = _boundingBox->getMin();
		Vec3f obj_max = _boundingBox->getMax();
		float x1 = obj_min.x(), y1 = obj_min.y(), z1 = obj_min.z();
		float x2 = obj_max.x(), y2 = obj_max.y(), z2 = obj_max.z();
		// eight point
		Vec3f v1 = Vec3f(x1, y1, z1);
		Vec3f v2 = Vec3f(x1, y1, z2);
		Vec3f v3 = Vec3f(x1, y2, z1);
		Vec3f v4 = Vec3f(x1, y2, z2);
		Vec3f v5 = Vec3f(x2, y1, z1);
		Vec3f v6 = Vec3f(x2, y1, z2);
		Vec3f v7 = Vec3f(x2, y2, z1);
		Vec3f v8 = Vec3f(x2, y2, z2);
		matrix->Transform(v1);
		matrix->Transform(v2);
		matrix->Transform(v3);
		matrix->Transform(v4);
		matrix->Transform(v5);
		matrix->Transform(v6);
		matrix->Transform(v7);
		matrix->Transform(v8);

		bb = new BoundingBox(Vec3f(INFINITY, INFINITY, INFINITY), Vec3f(-INFINITY, -INFINITY, -INFINITY));

		bb->Extend(v1);
		bb->Extend(v2);
		bb->Extend(v3);
		bb->Extend(v4);
		bb->Extend(v5);
		bb->Extend(v6);
		bb->Extend(v7);
		bb->Extend(v8);
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
	Vec3f obj_min = bb->getMin();
	Vec3f obj_max = bb->getMax();
	int start_i = int((obj_min - grid_min).x() / cellx);
	int start_j = int((obj_min - grid_min).y() / celly);
	int start_k = int((obj_min - grid_min).z() / cellz);
	int end_i = int((obj_max - grid_min).x() / cellx);
	int end_j = int((obj_max - grid_min).y() / celly);
	int end_k = int((obj_max - grid_min).z() / cellz);
	if (start_i > nx || start_j > ny || start_k > nz || end_i < 0 || end_j < 0 || end_k < 0)
		return;
	start_i = max(start_i, 0);
	start_j = max(start_j, 0);
	start_k = max(start_k, 0);
	end_i = min(end_i, nx - 1);
	end_j = min(end_j, ny - 1);
	end_k = min(end_k, nz - 1);

	if (start_i == nx) start_i--;
	if (start_j == ny) start_j--;
	if (start_k == nz) start_k--;

	for (int k = start_k; k <= end_k; ++k) 
	{
		for (int j = start_j; j <= end_j; ++j) 
		{
			for (int i = start_i; i <= end_i; ++i) 
			{
				int index = nx * ny * k + nx * j + i;
				grid->_opaque[index].push_back(new Transform(*matrix, this));
			}
		}
	}
}

bool Sphere::intersect(const Ray& ray, Hit& hit, float tmin)
{
	// Algebraic: quadratic equation of one unknown
	RayTracingStats::IncrementNumIntersections();
	Vec3f ro = ray.getOrigin() - _center;
	Vec3f rd = ray.getDirection();

	float a = rd.Dot3(rd);
	float b = 2 * ro.Dot3(rd);
	float c = ro.Dot3(ro) - _radius * _radius;
	float delta = b * b - 4 * a * c;
	float t = INFINITY;
	if (delta >= 0) {
		float d = sqrt(delta);
		float t1 = (-b - d) / (2 * a);
		float t2 = (-b + d) / (2 * a);
		if (t1 >= tmin) {
			t = t1;
		}
		else if (t2 >= tmin) {
			t = t2;
		}
		if (t < hit.getT()) {
			Vec3f normal = ro + t * rd;
			normal.Normalize();
			hit.set(t, _material, normal, ray);
			return true;
		}
	}
	return false;

	// Geometric: can decide is the origin inside the sphere
	//Vec3f ro = ray.getOrigin() - _center;
	//float rd_length = ray.getDirection().Length();
	//float tp = -ro.Dot3(ray.getDirection()) / rd_length;
	//if (tp < 0)
	//	return false;

	//float d = sqrt(ro.Dot3(ro) - tp * tp);
	//if (d > _radius)
	//	return false;
	//
	//float tt = sqrt(_radius * _radius - d * d);
	//float tA = (tp - tt) / rd_length;
	//float tB = (tp + tt) / rd_length;
	//if (tA < tmin)
	//{
	//	if (tB > tmin && tB < hit.getT())
	//	{
	//		Vec3f normal = ro + tB * ray.getDirection();
	//		normal.Normalize();
	//		hit.set(tB, _material, normal, ray);
	//		return true;
	//	}
	//	else
	//		return false;
	//}
	//else if(tA > tmin && tA < hit.getT())
	//{
	//	Vec3f normal = ro + tA * ray.getDirection();
	//	normal.Normalize();
	//	hit.set(tA, _material, normal, ray);
	//	return true;
	//}
	//
	//return false;
}

extern int theta_steps;
extern int phi_steps;
extern bool gouraud;
void Sphere::paint() const
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
		}
	}

	glBegin(GL_QUADS);
	for (int p = 0; p < phi_steps; p++)
	{
		for (int t = 0; t < theta_steps; t++)
		{
			//glNormal3f(normal.x(), normal.y(), normal.z());
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

void Sphere::insertIntoGrid(Grid* grid, Matrix* matrix)
{
	if (matrix)
	{
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

	float d = sqrt(cellx * cellx + celly * celly + cellz * cellz);

	for (int k = 0; k < nz; ++k) 
	{
		for (int j = 0; j < ny; ++j)
		{
			for (int i = 0; i < nx; ++i) 
			{
				float x = (i + 0.5) * cellx;
				float y = (j + 0.5) * celly;
				float z = (k + 0.5) * cellz;
				if((Vec3f(x,y,z) - (_center - grid_min)).Length() < _radius + d / 2.0)
				{
					int index = nx * ny * k + nx * j + i;
					grid->_opaque[index].push_back(this);
				}
			}
		}
	}
}

bool Plane::intersect(const Ray& ray, Hit& hit, float tmin)
{
	RayTracingStats::IncrementNumIntersections();
	if (fabs(_normal.Dot3(ray.getDirection())) < 0.00001)
		return false;
	float t = (_d - _normal.Dot3(ray.getOrigin())) / _normal.Dot3(ray.getDirection());
	if (t > tmin && t < hit.getT())
	{
		hit.set(t, _material, _normal, ray);
		return true;
	}
	return false;
}

const int INF = 10000;
//void Plane::paint() const
//{
//	_material->glSetMaterial();
//
//	Vec3f v;
//	if (_normal.x() == 1 && _normal.y() == 0 && _normal.z() == 0)
//		v = Vec3f(0, 1, 0);
//	else
//		v = Vec3f(1, 0, 0);
//
//	Vec3f b1, b2, central;
//	Vec3f::Cross3(b1, _normal, v);
//	Vec3f::Cross3(b2, _normal, b1);
//	if (_normal.z() != 0)
//	{
//		central = Vec3f(0, 0, _d / _normal.z());
//	}
//	else if (_normal.y() != 0)
//	{
//		central = Vec3f(0, _d / _normal.y(), 0);
//	}
//	else
//	{
//		central = Vec3f(_d / _normal.x(), 0, 0);
//	}
//
//	// simply draw very big rectangles
//	Vec3f vex[4] = { central + b1 * INF,central + b2 * INF,central - b1 * INF,central - b2 * INF };
//
//	glBegin(GL_QUADS);
//	glNormal3f(_normal.x(), _normal.y(), _normal.z());
//	glVertex3f(vex[0].x(), vex[0].y(), vex[0].z());
//	glVertex3f(vex[1].x(), vex[1].y(), vex[1].z());
//	glVertex3f(vex[2].x(), vex[2].y(), vex[2].z());
//	glVertex3f(vex[3].x(), vex[3].y(), vex[3].z());
//
//	glEnd();
//}

void Plane::paint() const {
	Vec3f u(1, 0, 0);
	Vec3f n = _normal;
	n.Normalize();
	if (fabs(n.Dot3(u) - 1) < 0.01)
		u = Vec3f(0, 1, 0);
	Vec3f v;
	Vec3f::Cross3(v, n, u);
	Vec3f::Cross3(u, v, n);
	Vec3f center = _d * n;
	const int INF = 10000;
	Vec3f pos[4] = { center + INF * u, center + INF * v, center - INF * u, center - INF * v };
	_material->glSetMaterial();
	glBegin(GL_QUADS);
	glNormal3f(n.x(), n.y(), n.z());
	glVertex3f(pos[0].x(), pos[0].y(), pos[0].z());
	glVertex3f(pos[1].x(), pos[1].y(), pos[1].z());
	glVertex3f(pos[2].x(), pos[2].y(), pos[2].z());
	glVertex3f(pos[3].x(), pos[3].y(), pos[3].z());
	glEnd();
}

void Plane::insertIntoGrid(Grid* grid, Matrix* matrix)
{
	grid->infinitePrimitives.push_back(this);
}

bool Triangle::intersect(const Ray& ray, Hit& hit, float tmin) {
	RayTracingStats::IncrementNumIntersections();

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

void Triangle::paint() const
{
	_material->glSetMaterial();

	glBegin(GL_TRIANGLES);
	glNormal3f(_normal.x(), _normal.y(), _normal.z());
	glVertex3f(_pointA.x(), _pointA.y(), _pointA.z());
	glVertex3f(_pointB.x(), _pointB.y(), _pointB.z());
	glVertex3f(_pointC.x(), _pointC.y(), _pointC.z());
	glEnd();
}

BoundingBox* Triangle::getTriangleBoundingBox(const Matrix* matrix) const {
	if (!matrix) return _boundingBox;
	Vec3f a = _pointA, b = _pointB, c = _pointC;
	matrix->Transform(a);
	matrix->Transform(b);
	matrix->Transform(c);
	BoundingBox* bb = new BoundingBox(
		Vec3f(min2(a.x(), b.x()), min2(a.y(), b.y()), min2(a.z(), b.z())),
		Vec3f(max2(a.x(), b.x()), max2(a.y(), b.y()), max2(a.z(), b.z()))
	);
	bb->Extend(c);
	return bb;
}

void Triangle::insertIntoGrid(Grid* grid, Matrix* matrix)
{
	BoundingBox* grid_bb = grid->getBoundingBox();
	Vec3f grid_min = grid_bb->getMin();
	Vec3f grid_max = grid_bb->getMax();
	int nx = grid->_nx;
	int ny = grid->_ny;
	int nz = grid->_nz;
	float cellx = (grid_max - grid_min).x() / float(nx);
	float celly = (grid_max - grid_min).y() / float(ny);
	float cellz = (grid_max - grid_min).z() / float(nz);

	BoundingBox* b = getTriangleBoundingBox(matrix);
	Vec3f obj_min = b->getMin();
	Vec3f obj_max = b->getMax();
	int start_i = int((obj_min - grid_min).x() / cellx);
	int start_j = int((obj_min - grid_min).y() / celly);
	int start_k = int((obj_min - grid_min).z() / cellz);
	int end_i = int((obj_max - grid_min).x() / cellx);
	int end_j = int((obj_max - grid_min).y() / celly);
	int end_k = int((obj_max - grid_min).z() / cellz);
	if (start_i > nx || start_j > ny || start_k > nz || end_i < 0 || end_j < 0 || end_k < 0)
		return;
	start_i = max(start_i, 0);
	start_j = max(start_j, 0);
	start_k = max(start_k, 0);
	end_i = min(end_i, nx - 1);
	end_j = min(end_j, ny - 1);
	end_k = min(end_k, nz - 1);

	if (start_i == nx) start_i--;
	if (start_j == ny) start_j--;
	if (start_k == nz) start_k--;

	for (int k = start_k; k <= end_k; ++k) 
	{
		for (int j = start_j; j <= end_j; ++j)
		{
			for (int i = start_i; i <= end_i; ++i) 
			{
				int index = nx * ny * k + nx * j + i;
				if (matrix)
					grid->_opaque[index].push_back(new Transform(*matrix, this));
				else
					grid->_opaque[index].push_back(this);
			}
		}
	}
}

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

void Transform::paint() const
{
	glPushMatrix();
	GLfloat* glMatrix = _matrix.glGet();
	glMultMatrixf(glMatrix);
	delete[] glMatrix;

	_object->paint();

	glPopMatrix();
}

void Transform::insertIntoGrid(Grid* grid, Matrix* matrix)
{
	if (matrix)
	{
		Matrix t = (*matrix) * _matrix;
		_object->insertIntoGrid(grid, &t);
	}
	else 
	{
		_object->insertIntoGrid(grid, &_matrix);
	}
}

BoundingBox* Transform::getBoundingBox() const 
{
	assert(_boundingBox != nullptr);
	if (_object->is_triangle()) {
		return _object->getTriangleBoundingBox(&_matrix);
	}
	Vec3f obj_min = _boundingBox->getMin();
	Vec3f obj_max = _boundingBox->getMax();
	float x1 = obj_min.x(), y1 = obj_min.y(), z1 = obj_min.z();
	float x2 = obj_max.x(), y2 = obj_max.y(), z2 = obj_max.z();
	Vec3f v1 = Vec3f(x1, y1, z1);
	Vec3f v2 = Vec3f(x1, y1, z2);
	Vec3f v3 = Vec3f(x1, y2, z1);
	Vec3f v4 = Vec3f(x1, y2, z2);
	Vec3f v5 = Vec3f(x2, y1, z1);
	Vec3f v6 = Vec3f(x2, y1, z2);
	Vec3f v7 = Vec3f(x2, y2, z1);
	Vec3f v8 = Vec3f(x2, y2, z2);
	_matrix.Transform(v1);
	_matrix.Transform(v2);
	_matrix.Transform(v3);
	_matrix.Transform(v4);
	_matrix.Transform(v5);
	_matrix.Transform(v6);
	_matrix.Transform(v7);
	_matrix.Transform(v8);

	BoundingBox* bb_new = new BoundingBox(
		Vec3f(INFINITY, INFINITY, INFINITY), 
		Vec3f(-INFINITY, -INFINITY, -INFINITY)
	);
	bb_new->Extend(v1);
	bb_new->Extend(v2);
	bb_new->Extend(v3);
	bb_new->Extend(v4);
	bb_new->Extend(v5);
	bb_new->Extend(v6);
	bb_new->Extend(v7);
	bb_new->Extend(v8);
	return bb_new;
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

void Group::paint() const
{
	for (int i = 0; i < num_objs; i++)
		_objects[i]->paint();
}

void Group::insertIntoGrid(Grid* grid, Matrix* matrix) {
	for (int i = 0; i < num_objs; i++) {
		_objects[i]->insertIntoGrid(grid, matrix);
	}
}

// only used in _scene_parser.h
void Group::addObject(int index, Object3D* obj)
{
	_objects[index] = obj;
	if (obj->getBoundingBox())
		_boundingBox->Extend(obj->getBoundingBox());
}
