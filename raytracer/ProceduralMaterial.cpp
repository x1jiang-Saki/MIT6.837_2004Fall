#include "Material.h"
#include "_perlin_noise.h"
#include <algorithm>

Vec3f Checkerboard::Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const {
	const auto& intersection = hit.getIntersectionPoint();
	return getMaterial(intersection)->Shade(ray, hit, dirToLight, lightColor);
}

float Noise::_noise(const Vec3f& tsCoord) const {
	float noise = 0.0f;
	auto noiseP = tsCoord;
	float noiseScale = 1.f;
	for (int i = 0; i < _octaves; ++i) {
		float x, y, z;
		noiseP.Get(x, y, z);
		noise += PerlinNoise::noise(x, y, z) / noiseScale;
		noiseP *= 2.f;
		noiseScale *= 2.f;
	}
	return noise;
}

float Noise::_clampedNoise(const Vec3f& tsCoord) const {
	float n = (_rangeOffset - _noise(tsCoord)) / _rangeLength;
	return min(1.f, max(0.f, n));
}

Vec3f Noise::Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const {
	auto color1 = _mat1->Shade(ray, hit, dirToLight, lightColor);
	auto color2 = _mat2->Shade(ray, hit, dirToLight, lightColor);
	float noise = _clampedNoise(getTextureSpaceCoord(hit.getIntersectionPoint()));
	return _lerp(color1, color2, noise);
}

float Marble::_noise(const Vec3f& tsCoord) const {
	return std::sinf(_frequency * tsCoord.x() + _amplitude * Noise::_noise(tsCoord));
}

float Wood::_noise(const Vec3f& tsCoord) const {
	float l = tsCoord.x() * tsCoord.x() + tsCoord.z() * tsCoord.z();
	float s = sinf(_frequency * sqrtf(l) + _amplitude * Noise::_noise(tsCoord) + tsCoord.y() * 0.4f);
	float sign = s < 0 ? -1.f : 1.f;
	s = fabsf(s);
	return sign * (1 - std::powf(1 - s, _frequency));
}
