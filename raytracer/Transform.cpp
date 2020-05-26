#include "Object3D.h"
#include <GL/gl.h>
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

void Transform::paint()
{
	// You only need to specify that 
	// you want to change the current object-space-to-world-space 4x4 matrix.
	
	// save the current matrix on a matrix stack
	glPushMatrix();
	GLfloat* glMatrix = _matrix.glGet();
	// Then change the matrix
	glMultMatrixf(glMatrix);
	delete[] glMatrix;

	// recursively call the paint() method of the child object
	_object->paint();

	// restore the previous matrix from the stack
	// If you do not save and restore the matrix, 
	// your transformation will be applied to all the following primitives!
	glPopMatrix();
}

void Transform::insertIntoGrid(Grid* grid, Matrix* matrix) {
	if (matrix) {
		Matrix t = (*matrix) * _matrix;
		_object->insertIntoGrid(grid, &t);
	}
	else {
		_object->insertIntoGrid(grid, &_matrix);
	}
}