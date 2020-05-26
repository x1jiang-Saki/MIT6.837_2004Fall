#include "Object3D.h"
#include <gl\GL.h>

#define min3(a, b, c) (((a)<(b))?(((a)<(c))?(a):(c)):(((b)<(c))?(b):(c)))
#define max3(a, b, c) (((a)>(b))?(((a)>(c))?(a):(c)):(((b)>(c))?(b):(c)))

extern bool visualize_grid;
bool Grid::intersect(const Ray& ray, Hit& hit, float tmin)
{
	bool flag_grid = false;
	is_intersected.clear();

	MarchingInfo mi;
	initializeRayMarch(mi, ray, tmin);

	//ray trace
	if (mi.t_cur < hit.getT()) {
		while (mi.i >= 0 && mi.j >= 0 && mi.k >= 0 && mi.i < _nx && mi.j < _ny && mi.k < _nz) {
			int i = mi.i;
			int j = mi.j;
			int k = mi.k;
			int index = _nx * _ny * k + _nx * j + i;
			//visualize_grid = false;
			if (!visualize_grid && !_cells[index].empty()) {
				for (Object3D* obj : _cells[index]) {
					if (is_intersected.find(obj) != is_intersected.end())
						continue;
					if (!obj->intersect(ray, hit, tmin))
						is_intersected.insert(obj);
				}
				if (hit.getT() < min3(mi.t_next_x, mi.t_next_y, mi.t_next_z) + 0.0001) {
					flag_grid = true;
					break;
				}
			}
			//visualize_grid = true;
			if (visualize_grid && !_cells[index].empty()) {
				PhongMaterial* m;
				switch (_cells[index].size()) {
				case 1: m = new PhongMaterial(Vec3f(1, 1, 1)); break;
				case 2: m = new PhongMaterial(Vec3f(1, 0, 1)); break;
				case 3: m = new PhongMaterial(Vec3f(0, 1, 1)); break;
				case 4: m = new PhongMaterial(Vec3f(1, 1, 0)); break;
				case 5: m = new PhongMaterial(Vec3f(0.3, 0, 0.7)); break;
				case 6: m = new PhongMaterial(Vec3f(0.7, 0, 0.3)); break;
				case 7: m = new PhongMaterial(Vec3f(0, 0.3, 0.7)); break;
				case 8: m = new PhongMaterial(Vec3f(0, 0.7, 0.3)); break;
				case 9: m = new PhongMaterial(Vec3f(0, 0.3, 0.7)); break;
				case 10: m = new PhongMaterial(Vec3f(0, 0.7, 0.3)); break;
				case 11: m = new PhongMaterial(Vec3f(0, 1, 0)); break;
				case 12: m = new PhongMaterial(Vec3f(0, 0, 1)); break;
				default: m = new PhongMaterial(Vec3f(1, 0, 0)); break;
				}
				hit.set(mi.t_cur, m, mi.normal, ray);
				return true;
			}
			mi.nextCell();
		}
	}

	//infinitePrimitives
	if (!visualize_grid) {
		bool flag_infinite = false;
		Hit hit_infinite(INFINITY, nullptr, Vec3f());
		for (Object3D* obj : infinitePrimitives) {
			if (obj->intersect(ray, hit_infinite, tmin)) {
				flag_infinite = true;
			}
		}
		if (!flag_infinite) {
			return flag_grid;
		}
		if (!flag_grid) {
			hit = hit_infinite;
			return flag_infinite;
		}
		if (hit_infinite.getT() < hit.getT()) {
			hit = hit_infinite;
			return true;
		}
		else {
			return true;
		}
	}
	return false;
}

void Grid::paint()
{
	Vec3f minP = _boundingBox->getMin();
	Vec3f maxP = _boundingBox->getMax();
	float cellx = (maxP - minP).x() / float(_nx);
	float celly = (maxP - minP).y() / float(_ny);
	float cellz = (maxP - minP).z() / float(_nz);

	for (int i = 0; i < _nx; i++)
	{
		for (int j = 0; j < _ny; j++)
		{
			for (int k = 0; k < _nz; k++)
			{
				int index = _nx * _ny * k + _nx * j + i;
				if (!_cells[index].empty()) 
				{
					Vec3f a = minP + Vec3f(i * cellx, j * celly, k * cellz);
					Vec3f b = minP + Vec3f((i + 1) * cellx, j * celly, k * cellz);
					Vec3f c = minP + Vec3f((i + 1) * cellx, (j + 1) * celly, k * cellz);
					Vec3f d = minP + Vec3f(i * cellx, (j + 1) * celly, k * cellz);
					Vec3f e = minP + Vec3f(i * cellx, j * celly, (k + 1) * cellz);
					Vec3f f = minP + Vec3f((i + 1) * cellx, j * celly, (k + 1) * cellz);
					Vec3f g = minP + Vec3f((i + 1) * cellx, (j + 1) * celly, (k + 1) * cellz);
					Vec3f h = minP + Vec3f(i * cellx, (j + 1) * celly, (k + 1) * cellz);
				
					PhongMaterial* m;
					switch (_cells[index].size()) {
						case 1: m = new PhongMaterial(Vec3f(1, 1, 1)); break;
						case 2: m = new PhongMaterial(Vec3f(1, 0, 1)); break;
						case 3: m = new PhongMaterial(Vec3f(0, 1, 1)); break;
						case 4: m = new PhongMaterial(Vec3f(1, 1, 0)); break;
						case 5: m = new PhongMaterial(Vec3f(0.3, 0, 0.7)); break;
						case 6: m = new PhongMaterial(Vec3f(0.7, 0, 0.3)); break;
						case 7: m = new PhongMaterial(Vec3f(0, 0.3, 0.7)); break;
						case 8: m = new PhongMaterial(Vec3f(0, 0.7, 0.3)); break;
						case 9: m = new PhongMaterial(Vec3f(0, 0.3, 0.7)); break;
						case 10: m = new PhongMaterial(Vec3f(0, 0.7, 0.3)); break;
						case 11: m = new PhongMaterial(Vec3f(0, 1, 0)); break;
						case 12: m = new PhongMaterial(Vec3f(0, 0, 1)); break;
						default: m = new PhongMaterial(Vec3f(1, 0, 0)); break;
					}
					m->glSetMaterial();
					glBegin(GL_QUADS);
					glNormal3f(0, 0, -1);
					glVertex3f(a.x(), a.y(), a.z());
					glVertex3f(b.x(), b.y(), b.z());
					glVertex3f(c.x(), c.y(), c.z());
					glVertex3f(d.x(), d.y(), d.z());
					glNormal3f(0, 0, 1);
					glVertex3f(e.x(), e.y(), e.z());
					glVertex3f(f.x(), f.y(), f.z());
					glVertex3f(g.x(), g.y(), g.z());
					glVertex3f(h.x(), h.y(), h.z());
					glNormal3f(1, 0, 0);
					glVertex3f(b.x(), b.y(), b.z());
					glVertex3f(c.x(), c.y(), c.z());
					glVertex3f(g.x(), g.y(), g.z());
					glVertex3f(f.x(), f.y(), f.z());
					glNormal3f(-1, 0, 0);
					glVertex3f(a.x(), a.y(), a.z());
					glVertex3f(d.x(), d.y(), d.z());
					glVertex3f(h.x(), h.y(), h.z());
					glVertex3f(e.x(), e.y(), e.z());
					glNormal3f(0, -1, 0);
					glVertex3f(a.x(), a.y(), a.z());
					glVertex3f(b.x(), b.y(), b.z());
					glVertex3f(f.x(), f.y(), f.z());
					glVertex3f(e.x(), e.y(), e.z());
					glNormal3f(0, 1, 0);
					glVertex3f(c.x(), c.y(), c.z());
					glVertex3f(d.x(), d.y(), d.z());
					glVertex3f(h.x(), h.y(), h.z());
					glVertex3f(g.x(), g.y(), g.z());
					glEnd();
				}
			}
		}
	}
}

void Grid::initializeRayMarch(MarchingInfo& mi, const Ray& ray, float tmin) const {
	Vec3f ro = ray.getOrigin();
	Vec3f rd = ray.getDirection();

	Vec3f minP = _boundingBox->getMin();
	Vec3f maxP = _boundingBox->getMax();
	float cellx = (maxP - minP).x() / _nx;
	float celly = (maxP - minP).y() / _ny;
	float cellz = (maxP - minP).z() / _nz;

	//mi.dt_x
	mi.dt_x = fabs(cellx / rd.x());
	mi.dt_y = fabs(celly / rd.y());
	mi.dt_z = fabs(cellz / rd.z());

	//mi.sign_x
	mi.sign_x = rd.x() > 0 ? 1 : -1;
	mi.sign_y = rd.y() > 0 ? 1 : -1;
	mi.sign_z = rd.z() > 0 ? 1 : -1;

	//mi.t_cur
	float t_near, t_far;
	float t1_x = (minP - ro).x() / rd.x();
	float t2_x = (maxP - ro).x() / rd.x();
	if (rd.x() < 0)
		swap(t1_x, t2_x);
	float t1_y = (minP - ro).y() / rd.y();
	float t2_y = (maxP - ro).y() / rd.y();
	if (rd.y() < 0)
		swap(t1_y, t2_y);
	float t1_z = (minP - ro).z() / rd.z();
	float t2_z = (maxP - ro).z() / rd.z();
	if (rd.z() < 0)
		swap(t1_z, t2_z);

	t_near = max3(t1_x, t1_y, t1_z);
	t_far = min3(t2_x, t2_y, t2_z);
	// box is missed
	if (t_near >= t_far)
		return;
	if (t_far <= tmin)
		return;

	//inside
	if (t_near < tmin) {
		//cout << "inside" << endl;
		if (t1_x > -INFINITY) {
			while (t1_x < tmin)
				t1_x += mi.dt_x;
		}
		if (t1_y > -INFINITY) {
			while (t1_y < tmin)
				t1_y += mi.dt_y;
		}
		if (t1_z > -INFINITY) {
			while (t1_z < tmin)
				t1_z += mi.dt_z;
		}
		//note INFINITY and here use min3
		//cout << t1_x << " " << t1_y << " " << t1_z << endl;
		t1_x = t1_x > tmin ? t1_x : INFINITY;
		t1_y = t1_y > tmin ? t1_y : INFINITY;
		t1_z = t1_z > tmin ? t1_z : INFINITY;
		//cout << t1_x << " " << t1_y << " " << t1_z << endl;
		t_near = min3(t1_x, t1_y, t1_z);
		mi.t_cur = t_near;
		if (t_near == t1_x) {
			mi.normal = Vec3f(-1, 0, 0) * mi.sign_x;
			mi.t_next_x = t1_x + mi.dt_x;
			mi.t_next_y = t1_y;
			mi.t_next_z = t1_z;
		}
		if (t_near == t1_y) {
			mi.normal = Vec3f(0, -1, 0) * mi.sign_y;
			mi.t_next_x = t1_x;
			mi.t_next_y = t1_y + mi.dt_y;
			mi.t_next_z = t1_z;
		}
		if (t_near == t1_z) {
			mi.normal = Vec3f(0, 0, -1) * mi.sign_z;
			mi.t_next_x = t1_x;
			mi.t_next_y = t1_y;
			mi.t_next_z = t1_z + mi.dt_z;
		}
	}
	else { //outside
	 //cout << "outside" << endl;
		mi.t_cur = t_near;
		if (t_near == t1_x) mi.normal = Vec3f(-1, 0, 0) * mi.sign_x;
		if (t_near == t1_y) mi.normal = Vec3f(0, -1, 0) * mi.sign_y;
		if (t_near == t1_z) mi.normal = Vec3f(0, 0, -1) * mi.sign_z;
		//mi.t_next_x
		if (t1_x > -INFINITY) {
			while (t1_x <= t_near)
				t1_x += mi.dt_x;
			mi.t_next_x = t1_x;
		}
		if (t1_y > -INFINITY) {
			while (t1_y <= t_near)
				t1_y += mi.dt_y;
			mi.t_next_y = t1_y;
		}
		if (t1_z > -INFINITY) {
			while (t1_z <= t_near)
				t1_z += mi.dt_z;
			mi.t_next_z = t1_z;
		}
	}

	//mi.i j k
	Vec3f p = ro + rd * t_near - minP;
	mi.i = int(p.x() / cellx);
	if (mi.sign_x < 0 && mi.i == _nx) mi.i--;
	mi.j = int(p.y() / celly);
	if (mi.sign_y < 0 && mi.j == _ny) mi.j--;
	mi.k = int(p.z() / cellz);
	if (mi.sign_z < 0 && mi.k == _nz) mi.k--;

}
