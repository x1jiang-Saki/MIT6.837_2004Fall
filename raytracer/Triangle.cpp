#include "Object3D.h"
#include <GL/gl.h>

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

void Triangle::paint()
{
	_material->glSetMaterial();

	glBegin(GL_TRIANGLES);
	glNormal3f(_normal.x(), _normal.y(), _normal.z());
	glVertex3f(_a.x(), _a.y(), _a.z());
	glVertex3f(_b.x(), _b.y(), _b.z());
	glVertex3f(_c.x(), _c.y(), _c.z());
	glEnd();
}

void Triangle::insertIntoGrid(Grid* grid, Matrix* matrix) {
	BoundingBox* bb_new;
	
	if (!matrix)
		bb_new = _boundingBox;
	else
	{
		Vec3f aa = _a, bb = _b, cc = _c;
		matrix->Transform(aa);
		matrix->Transform(bb);
		matrix->Transform(cc);
		bb_new = new BoundingBox(Vec3f(min(aa.x(), bb.x()), min(aa.y(), bb.y()), min(aa.z(), bb.z())),
			Vec3f(max(aa.x(), bb.x()), max(aa.y(), bb.y()), max(aa.z(), bb.z())));
		bb_new->Extend(cc);
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
	Vec3f obj_min = bb_new->getMin();
	Vec3f obj_max = bb_new->getMax();
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
	//note this
	if (start_i == nx) start_i--;
	if (start_j == ny) start_j--;
	if (start_k == nz) start_k--;

	for (int k = start_k; k <= end_k; ++k) {
		for (int j = start_j; j <= end_j; ++j) {
			for (int i = start_i; i <= end_i; ++i) {
				int index = nx * ny * k + nx * j + i;
				grid->_cells[index].push_back(this);
			}
		}
	}
}
