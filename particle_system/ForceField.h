#ifndef _FORCEFIELD_H_
#define _FORCEFIELD_H_

#include "_vectors.h"

class ForceField {
public:
	ForceField() {}
	virtual ~ForceField() {}

	virtual Vec3f getAcceleration(const Vec3f& position, float mass, float t) const = 0;
};

class GravityForceField : public ForceField 
{
	Vec3f gravity;
public:
	GravityForceField(const Vec3f& gravity) : gravity(gravity) {}

	virtual Vec3f getAcceleration(const Vec3f& position, float mass, float t) const
	{
		return gravity;
	}
};

class ConstantForceField : public ForceField 
{
	Vec3f force;
public:
	ConstantForceField(const Vec3f& force) : force(force) {}

	virtual Vec3f getAcceleration(const Vec3f& position, float mass, float t) const
	{
		return force * (1.0 / mass);
	}

};

class RadialForceField : public ForceField {
	float magnitude;
public:
	RadialForceField(float magnitude) : magnitude(magnitude) {}

	virtual Vec3f getAcceleration(const Vec3f& position, float mass, float t) const
	{
		return magnitude * (-1 * position) * (1.0f / mass);
	}
};

class VerticalForceField : public ForceField {
	float magnitude;
public:
	VerticalForceField(float magnitude) : magnitude(magnitude) {}

	virtual Vec3f getAcceleration(const Vec3f& position, float mass, float t) const
	{
		return magnitude * Vec3f(0, -position.y(), 0) * (1.0f / mass);
	}

};

class WindForceField : public ForceField {
	float magnitude;
public:
	WindForceField(float magnitude) : magnitude(magnitude) {}

	virtual Vec3f getAcceleration(const Vec3f& position, float mass, float t) const
	{
		Vec3f force = magnitude * Vec3f(sin(10 * t), cos(10 * t), 0);
		return force * (1.0f / mass);
	}
};
#endif // !_FORCEFIELD_H_
