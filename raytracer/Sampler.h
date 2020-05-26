#ifndef _SAMPLER_H_
#define _SAMPLER_H_

#include "_vectors.h"

class Sampler {
protected:
	int _nSamples = 0;

public:
	//static shared_ptr<Sampler> GetSamplerByStrategy(const std::string& strategyString, int nSamples);
	//static std::shared_ptr<Sampler> GetDefaultSampler();

	Sampler(int nSamples)
	{
		_nSamples = nSamples;
	}
	Sampler() {}
	~Sampler(){}

	virtual Vec2f getSamplePosition(int n) = 0;
};

class RandomSampler : public Sampler {
public:
	RandomSampler(int nSamples)
	{
		_nSamples = nSamples;
	}
	~RandomSampler(){}

	virtual Vec2f getSamplePosition(int n)
	{
		return Vec2f(1.0f * rand() / RAND_MAX, 1.0f * rand() / RAND_MAX);
	}
};

class UniformSampler : public Sampler {
protected:
	int _alignedStep;

public:
	UniformSampler(int nSamples)
	{
		assert(int(sqrtf(nSamples)) * int(sqrtf(nSamples)) == int(roundf(nSamples)));
		_nSamples = nSamples;
		_alignedStep = int(sqrtf(_nSamples));
	}
	UniformSampler() {}
	~UniformSampler(){}

	virtual Vec2f getSamplePosition(int n)
	{
		int x = n % _alignedStep, y = n / _alignedStep;
		float xPos = 1.f / _alignedStep * (x + 0.5f), yPos = 1.f / _alignedStep * (y + 0.5f);
		return Vec2f(xPos, yPos);
	}
};

class JitteredSampler : public UniformSampler {
public:
	JitteredSampler(int nSamples)
	{
		_nSamples = nSamples;
	}
	~JitteredSampler(){}

	virtual Vec2f getSamplePosition(int n)
	{
		Vec2f uniformPos = UniformSampler::getSamplePosition(n);
		auto turbulence = Vec2f(1.0*rand() / RAND_MAX - 0.5f, 1.0 * rand() / RAND_MAX - 0.5f);
		turbulence /= float(_alignedStep);
		auto jitteredPos = uniformPos;
		jitteredPos += turbulence;
		jitteredPos.Clamp();
		return jitteredPos;
	}
};

#endif //_SAMPLER_H_
