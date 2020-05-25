#include "Object3D.h"

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