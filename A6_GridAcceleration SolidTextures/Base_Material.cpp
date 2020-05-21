#include "Base_Material.h"
#include <gl\GL.h>
#include "_matrix.h"
#include "_perlin_noise.h"


#ifdef SPECULAR_FIX
// OPTIONAL:  global variable allows (hacky) communication
// with glCanvas::display
extern int SPECULAR_FIX_WHICH_PASS;
#endif

extern bool shade_back;
// ====================================================================
// Set the OpenGL parameters to render with the given material
// attributes.
// ====================================================================
void PhongMaterial::glSetMaterial() const {
	GLfloat one[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat zero[4] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat specular[4] = {
			_specularColor.r(),
			_specularColor.g(),
			_specularColor.b(),
			1.0 };
	GLfloat diffuse[4] = {
			_diffuseColor.r(),
			_diffuseColor.g(),
			_diffuseColor.b(),
			1.0 };

	// NOTE: GL uses the Blinn Torrance version of Phong...
	float glexponent = _exponent;
	if (glexponent < 0) glexponent = 0;
	if (glexponent > 128) glexponent = 128;

#if !SPECULAR_FIX

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

#else

	// OPTIONAL: 3 pass rendering to fix the specular highlight
	// artifact for small specular exponents (wide specular lobe)

	if (SPECULAR_FIX_WHICH_PASS == 0) {
		// First pass, draw only the specular highlights
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

	}
	else if (SPECULAR_FIX_WHICH_PASS == 1) {
		// Second pass, compute normal dot light
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
	}
	else {
		// Third pass, add ambient & diffuse terms
		assert(SPECULAR_FIX_WHICH_PASS == 2);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
	}

#endif
}

Vec3f PhongMaterial::Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const
{
	Vec3f N = hit.getNormal();
	if (shade_back && N.Dot3(ray.getDirection()) > 0)
		N = -1.0 * N;
	Vec3f V = -1.0 * ray.getDirection();
	V.Normalize();
	Vec3f L = dirToLight;
	Vec3f H = L + V;
	H.Normalize();

	float diffuse = max(0.0, N.Dot3(L));
	float specular = pow(max(0.0, N.Dot3(H)), _exponent);

	Vec3f result = diffuse * _diffuseColor + specular * _specularColor;
	result = result * lightColor;
	return result;
}

bool PhongMaterial::reflect(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& reflected) const
{
	if (_reflectiveColor.Length() < 0.0001)
		return false;

	Vec3f N = hit.getNormal();
	Vec3f I = ray.getDirection();
	if (N.Dot3(I) > 0)
		N = -1 * N;

	Vec3f reflectDir = -2.0 * N.Dot3(I) * N + I;
	reflectDir.Normalize();

	reflected = Ray(hit.getIntersectionPoint(), reflectDir);
	attenuation = _reflectiveColor;
	return true;
}

bool PhongMaterial::refract(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& refracted) const
{
	if (_transparentColor.Length() < 0.0001)
		return false;

	Vec3f N = hit.getNormal();
	Vec3f I = ray.getDirection();
	Vec3f V = -1.0 * I;

	float ir = _indexOfRefraction;
	if (N.Dot3(I) > 0)
		N = -1.0 * N;
	else
		ir = 1.0 / ir;

	float NV = N.Dot3(V);
	float t = 1 - ir * ir * (1 - NV * NV);
	if (t > 0)
	{
		Vec3f refractDir = (ir * NV - sqrt(t)) * N - ir * V;
		refractDir.Normalize();

		refracted = Ray(hit.getIntersectionPoint(), refractDir);
		attenuation = _transparentColor;
		return true;
	}
	else
		return false;
}

Vec3f Checkerboard::Shade(const Ray& ray, const Hit& hit, const Vec3f& lightDir, const Vec3f& lightCol) const
{
	Vec3f p = hit.getIntersectionPoint();
	_matrix->Transform(p);
	float sines = sin(3 * p.x()) * sin(3 * p.y()) * sin(3 * p.z());
	if (sines > 0)
		return _material1->Shade(ray, hit, lightDir, lightCol);
	else
		return _material2->Shade(ray, hit, lightDir, lightCol);
}

void Checkerboard::glSetMaterial() const 
{
	_material1->glSetMaterial();
}

bool Checkerboard::reflect(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& reflected) const 
{
	Vec3f p = hit.getIntersectionPoint();
	_matrix->Transform(p);
	float sines = sin(3 * p.x()) * sin(3 * p.y()) * sin(3 * p.z());
	if (sines > 0)
		return _material1->reflect(ray, hit, attenuation, reflected);
	else
		return _material2->reflect(ray, hit, attenuation, reflected);
}

bool Checkerboard::refract(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& refracted) const 
{
	Vec3f p = hit.getIntersectionPoint();
	_matrix->Transform(p);
	float sines = sin(3 * p.x()) * sin(3 * p.y()) * sin(3 * p.z());
	if (sines > 0)
		return _material1->refract(ray, hit, attenuation, refracted);
	else
		return _material2->refract(ray, hit, attenuation, refracted);
}

Vec3f Noise::Shade(const Ray& ray, const Hit& hit, const Vec3f& lightDir, const Vec3f& lightCol) const 
{
	Vec3f p = hit.getIntersectionPoint();
	_matrix->Transform(p);
	float N = 0;
	for (int i = 0; i < _octaves; ++i) {
		int t = pow(2, i);
		N += PerlinNoise::noise(t * p.x(), t * p.y(), t * p.z()) / t;
	}
	N = N < 0 ? 0 : N;
	N = N > 1 ? 1 : N;
	return _material1->Shade(ray, hit, lightDir, lightCol) * N + _material2->Shade(ray, hit, lightDir, lightCol) * (1 - N);
}

void Noise::glSetMaterial() const 
{

}

bool Noise::reflect(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& reflected) const
{
	Vec3f p = hit.getIntersectionPoint();
	_matrix->Transform(p);
	float N = 0;
	for (int i = 0; i < _octaves; ++i) {
		int t = pow(2, i);
		N += PerlinNoise::noise(t * p.x(), t * p.y(), t * p.z()) / t;
	}
	N = N < 0 ? 0 : N;
	N = N > 1 ? 1 : N;
	if (_material1->reflect(ray, hit, attenuation, reflected)) {
		attenuation = _material1->getDiffuseColor() * N + _material2->getDiffuseColor() * (1 - N);
		//attenuation = getDiffuseColor();
		return true;
	}
	return false;

}

bool Noise::refract(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& refracted) const 
{
	Vec3f p = hit.getIntersectionPoint();
	_matrix->Transform(p);
	float N = 0;
	for (int i = 0; i < _octaves; ++i) {
		int t = pow(2, i);
		N += PerlinNoise::noise(t * p.x(), t * p.y(), t * p.z()) / t;
	}
	N = N < 0 ? 0 : N;
	N = N > 1 ? 1 : N;
	if (_material1->refract(ray, hit, attenuation, refracted)) {
		attenuation = _material1->getDiffuseColor() * N + _material2->getDiffuseColor() * (1 - N);
		//attenuation = getDiffuseColor();
		return true;
	}
	return false;
}

Vec3f Noise::getDiffuseColor() const 
{
	return (_material1->getDiffuseColor() + _material2->getDiffuseColor()) * 0.5;
}

Vec3f Marble::Shade(const Ray& ray, const Hit& hit, const Vec3f& lightDir, const Vec3f& lightCol) const
{
	Vec3f p = hit.getIntersectionPoint();
	_matrix->Transform(p);
	float N = 0;
	for (int i = 0; i < _octaves; ++i) {
		int t = pow(2, i);
		N += PerlinNoise::noise(t * p.x(), t * p.y(), t * p.z()) / t;
	}
	//N = N < 0 ? 0 : N;
	//N = N > 1 ? 1 : N;
	float M = sin(_frequency * p.x() + _amplitude * N);
	M = M < 0 ? 0 : M;
	M = M > 1 ? 1 : M;
	return _material1->Shade(ray, hit, lightDir, lightCol) * M + _material2->Shade(ray, hit, lightDir, lightCol) * (1 - M);
}

void Marble::glSetMaterial() const 
{

}

bool Marble::reflect(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& reflected) const 
{
	Vec3f p = hit.getIntersectionPoint();
	_matrix->Transform(p);
	float N = 0;
	for (int i = 0; i < _octaves; ++i) {
		int t = pow(2, i);
		N += PerlinNoise::noise(t * p.x(), t * p.y(), t * p.z()) / t;
	}
	//N = N < 0 ? 0 : N;
	//N = N > 1 ? 1 : N;
	float M = sin(_frequency * p.x() + _amplitude * N);
	M = M < 0 ? 0 : M;
	M = M > 1 ? 1 : M;
	if (_material1->reflect(ray, hit, attenuation, reflected)) {
		attenuation = _material1->getDiffuseColor() * M + _material2->getDiffuseColor() * (1 - M);
		//attenuation = getDiffuseColor();
		return true;
	}
	return false;
}

bool Marble::refract(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& refracted) const 
{
	Vec3f p = hit.getIntersectionPoint();
	_matrix->Transform(p);
	float N = 0;
	for (int i = 0; i < _octaves; ++i) {
		int t = pow(2, i);
		N += PerlinNoise::noise(t * p.x(), t * p.y(), t * p.z()) / t;
	}
	//N = N < 0 ? 0 : N;
	//N = N > 1 ? 1 : N;
	float M = sin(_frequency * p.x() + _amplitude * N);
	M = M < 0 ? 0 : M;
	M = M > 1 ? 1 : M;
	if (_material1->refract(ray, hit, attenuation, refracted)) {
		attenuation = _material1->getDiffuseColor() * M + _material2->getDiffuseColor() * (1 - M);
		//attenuation = getDiffuseColor();
		return true;
	}
	return false;
}

Vec3f Marble::getDiffuseColor() const 
{
	return (_material1->getDiffuseColor() + _material2->getDiffuseColor()) * 0.5;
}

Vec3f Wood::Shade(const Ray& ray, const Hit& hit, const Vec3f& lightDir, const Vec3f& lightCol) const 
{
	Vec3f p = hit.getIntersectionPoint();
	_matrix->Transform(p);
	float N = 0;
	for (int i = 0; i < _octaves; ++i) {
		int t = pow(2, i);
		N += PerlinNoise::noise(t * p.x(), t * p.y(), t * p.z()) / t;
	}
	//N = N < 0 ? 0 : N;
	//N = N > 1 ? 1 : N;
	float M = sin(_frequency * p.x() + _amplitude * N);
	M = M < 0 ? 0 : M;
	M = M > 1 ? 1 : M;
	return _material1->Shade(ray, hit, lightDir, lightCol) * M + _material2->Shade(ray, hit, lightDir, lightCol) * (1 - M);
}

void Wood::glSetMaterial() const
{

}

bool Wood::reflect(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& reflected) const 
{
	return false;
}

bool Wood::refract(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& refracted) const
{
	return false;
}

Vec3f Wood::getDiffuseColor() const 
{
	return (_material1->getDiffuseColor() + _material2->getDiffuseColor()) * 0.5;
}
