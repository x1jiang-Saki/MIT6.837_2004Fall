﻿#include "Base_Material.h"
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
void PhongMaterial::glSetMaterial(void) const
{

	GLfloat one[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat zero[4] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat specular[4] = {
		getSpecularColor().r(),
		getSpecularColor().g(),
		getSpecularColor().b(),
		1.0 };
	GLfloat diffuse[4] = {
		getDiffuseColor().r(),
		getDiffuseColor().g(),
		getDiffuseColor().b(),
		1.0 };

	// NOTE: GL uses the Blinn Torrance version of Phong...      
	float glexponent = _exponent;
	if (glexponent < 0) glexponent = 0;
	if (glexponent > 128) glexponent = 128;

#if !SPECULAR_FIX 

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);//������
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);//������
	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);//������
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);//������

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);

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
	Vec3f V = -1.0 * ray.getDirection();
	Vec3f L = dirToLight;
	Vec3f H = L + V;
	H.Normalize();

	float diffuse = max(0.0, N.Dot3(L));
	float specular = pow(max(0.0, N.Dot3(H)), _exponent);

	Vec3f result = diffuse * _diffuseColor + specular * _specularColor;
	result = result * lightColor;
	return result;
}
