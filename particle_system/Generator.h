#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "_particle.h"
#include "_random.h"

class Generator {
protected:
	Vec3f _color, _deadColor;
	float _colorRandomness;

	float _lifespan, _lifespanRandomness;
	int _desiredNumParticles;

	float _mass, _massRandomness;

	Random* _random;

public:
	Generator() { _random = new Random(); }
	virtual ~Generator() { delete _random; }

	// initialization
	void SetColors(const Vec3f& color, const Vec3f& dead_color, float color_randomness)
	{
		_color = color;
		_deadColor = dead_color;
		_colorRandomness = color_randomness;
	}
	void SetLifespan(float lifespan, float lifespan_randomness, int desired_num_particles)
	{
		_lifespan = lifespan;
		_lifespanRandomness = lifespan_randomness;
		_desiredNumParticles = desired_num_particles;
	}
	void SetMass(float mass, float mass_randomness)
	{
		_mass = mass;
		_massRandomness = mass_randomness;
	}

	// on each timestep, create some particles
	virtual int numNewParticles(float current_time, float dt) const = 0;
	virtual Particle* Generate(float current_time, int i) = 0;

	// for the gui
	virtual void Paint() const {}
	void Restart() { delete _random; _random = new Random; }
};

class HoseGenerator : public Generator 
{
	Vec3f _position;
	float _positionRandomness;
	Vec3f _velocity;
	float _velocityRandomness;

public:
	HoseGenerator(const Vec3f& position, float position_randomness, const Vec3f& velocity, float velocity_randomness)
	{
		_position = position; _positionRandomness = position_randomness;
		_velocity = velocity; _velocityRandomness = velocity_randomness;
	}

	virtual int numNewParticles(float current_time, float dt) const;
	virtual Particle* Generate(float current_time, int i);
};

class RingGenerator : public Generator {
	Vec3f _velocity;
	float _positionRandomness, _velocityRandomness;

public:
	RingGenerator(float position_randomness, const Vec3f& velocity, float velocity_randomness)
	{
		_velocity = velocity;
		_positionRandomness = position_randomness;
		_velocityRandomness = velocity_randomness;
	}

	virtual int numNewParticles(float current_time, float dt) const;
	virtual Particle* Generate(float current_time, int i);

	virtual void Paint() const;
};

#endif //_GENERATOR_H_
