#include "Camera.h"
#include "_matrix.h"
#include <GL/gl.h>
#include <gl\GLU.h>

Ray OrthographicCamera::generateRay(Vec2f point)
{
	Vec3f origin = _center + (point.x() - 0.5) * _size * _horizontal + (point.y() - 0.5) * _size * _up;
	Vec3f direction = _direction;
	direction.Normalize();
	return Ray(origin, direction);
}

float OrthographicCamera::getTMin() const
{
	// a large negative value
	return -INFINITY;
}


void OrthographicCamera::glInit(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w > h)
		glOrtho(-_size / 2.0, _size / 2.0, -_size * (float)h / (float)w / 2.0, _size * (float)h / (float)w / 2.0, 0.5, 40.0);
	else
		glOrtho(-_size * (float)w / (float)h / 2.0, _size * (float)w / (float)h / 2.0, -_size / 2.0, _size / 2.0, 0.5, 40.0);
}

// ====================================================================
// Place an orthographic camera within an OpenGL scene
// ====================================================================

void OrthographicCamera::glPlaceCamera(void)
{
	gluLookAt(_center.x(), _center.y(), _center.z(),
		_center.x() + _direction.x(), _center.y() + _direction.y(), _center.z() + _direction.z(),
		_up.x(), _up.y(), _up.z());
}

// ====================================================================
// dollyCamera, truckCamera, and RotateCamera
//
// Asumptions:
//  - up is really up (i.e., it hasn't been changed
//    to point to "screen up")
//  - up and direction are normalized
// Special considerations:
//  - If your constructor precomputes any vectors for
//    use in 'generateRay', you will likely to recompute those
//    values at athe end of the these three routines
// ====================================================================

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================

void OrthographicCamera::dollyCamera(float dist)
{
	_center += _direction * dist;

	// ===========================================
	// ASSIGNMENT 3: Fix any other affected values
	// ===========================================


}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void OrthographicCamera::truckCamera(float dx, float dy)
{
	Vec3f horizontal;
	Vec3f::Cross3(horizontal, _direction, _up);
	horizontal.Normalize();

	Vec3f screenUp;
	Vec3f::Cross3(screenUp, horizontal, _direction);

	_center += horizontal * dx + screenUp * dy;

	// ===========================================
	// ASSIGNMENT 3: Fix any other affected values
	// ===========================================


}

// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================

void OrthographicCamera::rotateCamera(float rx, float ry)
{
	Vec3f horizontal;
	Vec3f::Cross3(horizontal, _direction, _up);
	horizontal.Normalize();

	// Don't let the model flip upside-down (There is a singularity
	// at the poles when 'up' and 'direction' are aligned)
	float tiltAngle = acos(_up.Dot3(_direction));
	if (tiltAngle - ry > 3.13)
		ry = tiltAngle - 3.13;
	else if (tiltAngle - ry < 0.01)
		ry = tiltAngle - 0.01;

	Matrix rotMat = Matrix::MakeAxisRotation(_up, rx);
	rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

	rotMat.Transform(_center);
	rotMat.TransformDirection(_direction);

	// ===========================================
	// ASSIGNMENT 3: Fix any other affected values
	// ===========================================


}