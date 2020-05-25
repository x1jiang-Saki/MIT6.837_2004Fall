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
	virtual Vec3f getTransparentColor() const = 0;
	virtual Vec3f getReflectiveColor() const = 0;
	virtual float getIndexOfRefraction() const = 0;
	
	// computes the local interaction of light and the material
	virtual Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight,const Vec3f& lightColor) const = 0;

	// send the appropriate OpenGL commands to specify the local shading model.
	virtual void glSetMaterial(void) const = 0;
};

class PhongMaterial : public Material
{
	Vec3f _specularColor;
	float _exponent;

	Vec3f _reflectiveColor;
	Vec3f _transparentColor;
	float _indexOfRefraction;

public:
	PhongMaterial(const Vec3f& diffuseColor, const Vec3f& specularColor, float exponent, const Vec3f& reflectiveColor,const Vec3f& transparentColor,float indexOfRefraction)
	{
		_diffuseColor = diffuseColor;
		_specularColor = specularColor;
		_exponent = exponent;

		_reflectiveColor = reflectiveColor;
		_transparentColor = transparentColor;
		_indexOfRefraction = indexOfRefraction;
	}
	~PhongMaterial(){}

	Vec3f getSpecularColor() const { return _specularColor; }
	Vec3f getTransparentColor() const { return _transparentColor; }
	Vec3f getReflectiveColor() const { return _reflectiveColor; }
	float getIndexOfRefraction() const { return _indexOfRefraction; }

	virtual Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const;
	virtual void glSetMaterial(void) const;
};
#endif //  _MATERIAL_H_
