#ifndef _INTEGRATOR_H_
#define _INTEGRATOR_H_

#include "_particle.h"
#include "forcefield.h"

class Integrator {
public:
	Integrator() {}
	virtual ~Integrator() {}

	virtual void Update(Particle* particle, ForceField* forcefield, float t, float dt) = 0;
	virtual Vec3f getColor() = 0;
};

class EulerIntegrator : public Integrator {
public:
	EulerIntegrator() {}
	~EulerIntegrator() {}

	virtual void Update(Particle* particle, ForceField* forcefield, float t, float dt);
	Vec3f getColor() override { return Vec3f(1.f, 0.f, 0.f); }
};

class MidpointIntegrator : public Integrator {
public:
	MidpointIntegrator() {}
	~MidpointIntegrator() {}

	virtual void Update(Particle* particle, ForceField* forcefield, float t, float dt);
	Vec3f getColor() override { return Vec3f(0.f, 1.f, 0.f); }
};

class RungeKuttaIntegrator : public Integrator {
public:
	RungeKuttaIntegrator() {}
	~RungeKuttaIntegrator() {}

	virtual void Update(Particle* particle, ForceField* forcefield, float t, float dt);
	Vec3f getColor() override { return Vec3f(0.f, 0.f, 1.f); }
};

#endif //_INTEGRATOR_H_
