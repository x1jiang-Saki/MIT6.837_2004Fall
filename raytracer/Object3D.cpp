#include "Object3D.h"
#include <algorithm>

void Object3D::insertIntoGrid(Grid* grid, Matrix* matrix) {
	BoundingBox* bb_new = _boundingBox;

	if (matrix) {
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
		matrix->Transform(v1);
		matrix->Transform(v2);
		matrix->Transform(v3);
		matrix->Transform(v4);
		matrix->Transform(v5);
		matrix->Transform(v6);
		matrix->Transform(v7);
		matrix->Transform(v8);
		bb_new = new BoundingBox(Vec3f(INFINITY, INFINITY, INFINITY), Vec3f(-INFINITY, -INFINITY, -INFINITY));
		bb_new->Extend(v1);
		bb_new->Extend(v2);
		bb_new->Extend(v3);
		bb_new->Extend(v4);
		bb_new->Extend(v5);
		bb_new->Extend(v6);
		bb_new->Extend(v7);
		bb_new->Extend(v8);
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
				grid->_cells[index].push_back(new Transform(*matrix, this));
			}
		}
	}
}