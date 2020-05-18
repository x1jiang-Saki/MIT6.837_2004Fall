#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "_vectors.h"
#include "_hit.h"

class Material {
public:

  // CONSTRUCTORS & DESTRUCTOR
	Material(){}
	Material(const Vec3f &diffuseColor) { _diffuseColor = diffuseColor; }
	virtual ~Material() {}

  // ACCESSORS
	virtual Vec3f getDiffuseColor() const { return _diffuseColor; }
	virtual Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const = 0;
	virtual void glSetMaterial() const = 0;

protected:
  Vec3f _diffuseColor;

};

class PhongMaterial : public Material
{
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
	virtual void glSetMaterial() const;
private:
	Vec3f _specularColor;
	float _exponent;
};

#endif //  _MATERIAL_H_
