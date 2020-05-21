#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "_vectors.h"
#include "_hit.h"

class Material {
public:
	Material(){}
	Material(const Vec3f &diffuseColor) { _diffuseColor = diffuseColor; }
	virtual ~Material() {}

	virtual Vec3f getDiffuseColor() const { return _diffuseColor; }

	virtual bool reflect(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& reflected) const = 0;
	virtual bool refract(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& refracted) const = 0;
	virtual Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const = 0;
	virtual void glSetMaterial() const = 0;

protected:
  Vec3f _diffuseColor;

};

class PhongMaterial : public Material
{
public:
	PhongMaterial(const Vec3f& diffuseColor)
	{
		_diffuseColor = diffuseColor;
		_exponent = 0;
		_indexOfRefraction = 0;
	}
	PhongMaterial(const Vec3f& diffuseColor, const Vec3f& specularColor, float exponent, Vec3f reflectiveColor, Vec3f transparentColor, float indexOfRefraction)
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
	Vec3f getReflectiveColor() const { return _reflectiveColor; }
	Vec3f getTransparentColor() const { return _transparentColor; }
	float getIndexOfRefraction() const { return _indexOfRefraction; }

	virtual Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const;
	virtual void glSetMaterial() const;
	virtual bool reflect(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& reflected) const;
	virtual bool refract(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& refracted) const;

private:
	Vec3f _specularColor;
	float _exponent;

	Vec3f _reflectiveColor;
	Vec3f _transparentColor;
	float _indexOfRefraction;
};

class Checkerboard : public Material 
{
public:
	Checkerboard(Matrix* matrix, Material* mat1, Material* mat2)
	{
		_diffuseColor = Vec3f();
		_matrix = matrix;
		_material1 = mat1;
		_material2 = mat2;
	}

	void glSetMaterial() const;
	Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& lightDir, const Vec3f& lightCol) const;
	bool reflect(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& reflected) const;
	bool refract(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& refracted) const;

private:
	Material* _material1;
	Material* _material2;
	Matrix* _matrix;
};

class Noise : public Material
{
public:
	Noise(Matrix* matrix, Material* mat1, Material* mat2, int octaves)
	{
		_diffuseColor = Vec3f();
		_matrix = matrix;
		_material1 = mat1;
		_material2 = mat2;
		_octaves = octaves;
	}
	Vec3f getDiffuseColor() const;


	void glSetMaterial() const;
	Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& lightDir, const Vec3f& lightCol) const;
	bool reflect(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& reflected) const;
	bool refract(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& refracted) const;

private:
	Material* _material1;
	Material* _material2;
	Matrix* _matrix;
	int _octaves;
};

class Marble : public Material
{
public:
	Marble(Matrix* matrix, Material* mat1, Material* mat2, int octaves, float frequency, float amplitude)
	{
		_diffuseColor = Vec3f();
		_matrix = matrix;
		_material1 = mat1;
		_material2 = mat2;
		_octaves = octaves;
		_frequency = frequency;
		_amplitude = amplitude;
	}
	Vec3f getDiffuseColor() const;

	void glSetMaterial() const;
	Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& lightDir, const Vec3f& lightCol) const;
	bool reflect(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& reflected) const;
	bool refract(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& refracted) const;

private:
	Material* _material1;
	Material* _material2;
	Matrix* _matrix;
	int _octaves;
	float _frequency, _amplitude;
};

class Wood : public Material
{
public:
	Wood(Matrix* matrix, Material* mat1, Material* mat2, int octaves, float frequency, float amplitude)
	{
		_diffuseColor = Vec3f();
		_matrix = matrix;
		_material1 = mat1;
		_material2 = mat2;
		_octaves = octaves;
		_frequency = frequency;
		_amplitude = amplitude;
	}
	Vec3f getDiffuseColor() const;

	void glSetMaterial() const;
	Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& lightDir, const Vec3f& lightCol) const;
	bool reflect(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& reflected) const;
	bool refract(const Ray& ray, const Hit& hit, Vec3f& attenuation, Ray& refracted) const;

private:
	Material* _material1;
	Material* _material2;
	Matrix* _matrix;
	int _octaves;
	float _frequency, _amplitude;
};

#endif //  _MATERIAL_H_
