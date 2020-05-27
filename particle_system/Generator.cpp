#include "Generator.h"
#include <gl/gl.h>

#define M_PI 3.1415926

int HoseGenerator::numNewParticles(float current_time, float dt) const {
	return dt * _desiredNumParticles / _lifespan;
}

Particle* HoseGenerator::Generate(float current_time, int i) {
	Vec3f p = _position + _positionRandomness * _random->randomVector();
	Vec3f v = _velocity + _velocityRandomness * _random->randomVector();
	Vec3f c = _color + _colorRandomness * _random->randomVector();
	Vec3f dead_c = _deadColor + _colorRandomness * _random->randomVector();
	float m = _mass + _massRandomness * _random->next();
	float l = _lifespan + _lifespanRandomness * _random->next();
	Particle* particle = new Particle(p, v, c, dead_c, m, l);
	return particle;
}

int RingGenerator::numNewParticles(float current_time, float dt) const {
	return (current_time < 3 ? current_time / 3 : 1) * _desiredNumParticles;
}

Particle* RingGenerator::Generate(float current_time, int i) {
	float radius = current_time < 3 ? current_time : 3;
	Vec3f p = Vec3f(radius * sin(i) + 0.2 * _random->next(), -4, radius * cos(i) + 0.2 * _random->next());
	Vec3f v = _velocity + _velocityRandomness * _random->randomVector();
	Vec3f c = _color + _colorRandomness * _random->randomVector();
	Vec3f dead_c = _deadColor + _colorRandomness * _random->randomVector();
	float m = _mass + _massRandomness * _random->next();
	float l = _lifespan + _lifespanRandomness * _random->next();
	Particle* particle = new Particle(p, v, c, dead_c, m, l);
	return particle;
}

void RingGenerator::Paint() const {
	glColor3f(0.3, 0.3, 0.3);
	glBegin(GL_QUADS);
	glVertex3f(5, -4, 5);
	glVertex3f(5, -4, -5);
	glVertex3f(-5, -4, -5);
	glVertex3f(-5, -4, 5);
	glEnd();
}
