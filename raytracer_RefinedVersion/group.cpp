#include "object3D.h"

bool Group::intersect(const Ray& ray, Hit& hit, float tmin)
{
	for (int i = 0; i < _objsNum; i++)
	{
		if (_objects[i]->intersect(ray, hit, tmin))
			return true;
	}
	return false;
}

void Group::addObject(int index, Object3D* obj)
{
	_objects[index] = obj;
}