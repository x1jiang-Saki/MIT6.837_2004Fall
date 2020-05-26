#ifndef _FILTER_H_
#define _FILTER_H_

#include <string>

#include "_vectors.h"
#include "_film.h"

class Filter {
public:
	//static std::shared_ptr<Filter> GetFilterByStrategy(const std::string& strategyString, float filterArgument);
	//static std::shared_ptr<Filter> GetDefaultFilter();

	Vec3f getColor(int i, int j, Film* film);

	virtual float getWeight(float x, float y) = 0;
	virtual int getSupportRadius() = 0;
};

class BoxFilter : public Filter {
	float _radius;
public:
	BoxFilter(float radius)
	{
		_radius = radius;
	}
	~BoxFilter(){}

	virtual float getWeight(float x, float y);
	virtual int getSupportRadius();
};

class TentFilter : public Filter {
	float _radius;
public:
	TentFilter(float radius)
	{
		_radius = radius;
	}
	~TentFilter(){}

	virtual float getWeight(float x, float y);
	virtual int getSupportRadius();
};

class GaussianFilter : public Filter {
	float _sigma;

public:
	GaussianFilter(float sigma)
	{
		_sigma = sigma;
	}
	~GaussianFilter(){}

	virtual float getWeight(float x, float y);
	virtual int getSupportRadius();
};

#endif //_FILTER_H_
