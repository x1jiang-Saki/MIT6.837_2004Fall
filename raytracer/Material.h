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

	virtual Vec3f getDiffuseColor(const Vec3f& wsCoord) const { return _diffuseColor; }
	virtual Vec3f getTransparentColor(const Vec3f& wsCoord) const = 0;
	virtual Vec3f getReflectiveColor(const Vec3f& wsCoord) const = 0;
	virtual float getIndexOfRefraction(const Vec3f& wsCoord) const = 0;
	
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
	PhongMaterial(const Vec3f& diffuseColor)
	{
		_diffuseColor = diffuseColor;
	}
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
	virtual Vec3f getTransparentColor(const Vec3f& wsCoord) const { return _transparentColor; }
	virtual Vec3f getReflectiveColor(const Vec3f& wsCoord) const { return _reflectiveColor; }
	virtual float getIndexOfRefraction(const Vec3f& wsCoord) const { return _indexOfRefraction; }

	virtual Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const;
	virtual void glSetMaterial(void) const;
};

class ProceduralMaterial : public Material 
{
protected:
	// describes how the world space coordinates
	// are mapped to the 3D solid texture space
	Matrix* _matrix;

	ProceduralMaterial(){}
	~ProceduralMaterial(){}

	explicit ProceduralMaterial(Matrix* matrix)
	{
		_matrix = matrix;
	}

	Vec3f getTextureSpaceCoord(const Vec3f& wsCoord) const
	{
		auto tsCoord = wsCoord;
		_matrix->Transform(tsCoord);
		return tsCoord;
	}
};

class Checkerboard : public ProceduralMaterial {
	Material *_mat1, *_mat2;

	Material* getMaterial(const Vec3f& wsCoord) const
	{
		auto tsp = getTextureSpaceCoord(wsCoord);
		auto index = int(std::fabsf(std::floorf(tsp.x()) + std::floorf(tsp.y()) + std::floorf(tsp.z()))) % 2;
		return (index == 0 ? _mat1 : _mat2);
	}

public:
	Checkerboard(){}
	~Checkerboard(){}
	Checkerboard(Matrix* matrix, Material* mat1, Material* mat2)
	{
		_matrix = matrix;
		_mat1 = mat1;
		_mat2 = mat2;
	}

	virtual Vec3f getDiffuseColor(const Vec3f& wsCoord) const override { return getMaterial(wsCoord)->getDiffuseColor(wsCoord); }
	virtual Vec3f getReflectiveColor(const Vec3f& wsCoord) const override { return getMaterial(wsCoord)->getReflectiveColor(wsCoord); }
	virtual Vec3f getTransparentColor(const Vec3f& wsCoord) const override { return getMaterial(wsCoord)->getTransparentColor(wsCoord); }
	virtual float getIndexOfRefraction(const Vec3f& wsCoord) const override { return getMaterial(wsCoord)->getIndexOfRefraction(wsCoord); }

	virtual void glSetMaterial() const { _mat1->glSetMaterial(); }
	virtual Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const;
};

class Noise : public ProceduralMaterial {
protected:
	Material *_mat1, *_mat2;
	int _octaves;
	float _rangeLength = 0.f, _rangeOffset = 0.f;

protected:
	virtual float _noise(const Vec3f& tsCoord) const;
	float _clampedNoise(const Vec3f& tsCoord) const;
	
	template <class T>
	T _lerp(const T& a, const T& b, float t) const { return a + t * (b - a); }

public:
	Noise(){}
	~Noise(){}
	Noise(Matrix* matrix, Material* mat1, Material* mat2, int octaves)
	{
		_matrix = matrix;
		_mat1 = mat1;
		_mat2 = mat2;
		_octaves = octaves;
		_rangeLength = 1.0;
		_rangeOffset = 0.5;
	}

	virtual Vec3f getDiffuseColor(const Vec3f& wsCoord) const override {
		auto tsCoord = getTextureSpaceCoord(wsCoord);
		return _lerp(_mat1->getDiffuseColor(wsCoord), _mat2->getDiffuseColor(wsCoord), _clampedNoise(tsCoord));
	}
	virtual Vec3f getReflectiveColor(const Vec3f& wsCoord) const override {
		auto tsCoord = getTextureSpaceCoord(wsCoord);
		return _lerp(_mat1->getReflectiveColor(wsCoord), _mat2->getReflectiveColor(wsCoord), _clampedNoise(tsCoord));
	}
	virtual Vec3f getTransparentColor(const Vec3f& wsCoord) const override {
		auto tsCoord = getTextureSpaceCoord(wsCoord);
		return _lerp(_mat1->getTransparentColor(wsCoord), _mat2->getTransparentColor(wsCoord), _clampedNoise(tsCoord));
	}
	virtual float getIndexOfRefraction(const Vec3f& wsCoord) const override {
		auto tsCoord = getTextureSpaceCoord(wsCoord);
		return _lerp(_mat1->getIndexOfRefraction(wsCoord), _mat2->getIndexOfRefraction(wsCoord), _clampedNoise(tsCoord));
	}

	virtual void glSetMaterial() const { _mat1->glSetMaterial(); }
	virtual Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const;
};

class Marble : public Noise {
	float _frequency, _amplitude;

	float _noise(const Vec3f& tsCoord) const override;

public:
	Marble(Matrix* matrix, Material* mat1, Material* mat2, int octaves, float frequency, float amplitude)
	{
		_matrix = matrix;
		_mat1 = mat1;
		_mat2 = mat2;
		_octaves = octaves;
		_rangeLength = 2.0;
		_rangeOffset = 1.0;
		_frequency = frequency;
		_amplitude = amplitude;
	}
};

class Wood : public Noise {
	float _frequency, _amplitude;

	float _noise(const Vec3f& tsCoord) const override;

public:
	Wood(Matrix* matrix, Material* mat1, Material* mat2, int octaves, float frequency, float amplitude)
	{
		_matrix = matrix;
		_mat1 = mat1;
		_mat2 = mat2;
		_octaves = octaves;
		_rangeLength = 2.0;
		_rangeOffset = 1.0;
		_frequency = frequency;
		_amplitude = amplitude;
	}
};

#endif //  _MATERIAL_H_
