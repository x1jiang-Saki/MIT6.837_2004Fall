#include "Base_Material.h"
#include <gl\GL.h>


#ifdef SPECULAR_FIX
// OPTIONAL:  global variable allows (hacky) communication
// with glCanvas::display
extern int SPECULAR_FIX_WHICH_PASS;
#endif

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
	if (N.Dot3(ray.getDirection()) > 0)
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