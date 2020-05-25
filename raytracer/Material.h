#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "_vectors.h"
#include "_ray.h"
#include "_hit.h"

class Material 
{
protected:
	Vec3f _diffuseColor;

public:
	Material(){}
	Material(const Vec3f &diffuseColor) { _diffuseColor = diffuseColor; }
	virtual ~Material() {}

	virtual Vec3f getDiffuseColor() const { return _diffuseColor; }

	// computes the local interaction of light and the material
	virtual Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight,const Vec3f& lightColor) const = 0;

	// send the appropriate OpenGL commands to specify the local shading model.
	virtual void glSetMaterial(void) const = 0;
};

class PhongMaterial : public Material
{
	Vec3f _specularColor;
	float _exponent;

public:
	PhongMaterial(const Vec3f& diffuseColor, const Vec3f& specularColor, float exponent)
	{
		_diffuseColor = diffuseColor;
		_specularColor = specularColor;
		_exponent = exponent;
	}
	~PhongMaterial(){}

	Vec3f getSpecularColor() const { return _specularColor; }

	virtual Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const;
	virtual void glSetMaterial(void) const;
};
#endif //  _MATERIAL_H_
