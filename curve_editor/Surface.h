#ifndef _SURFACE_
#define _SURFACE_

#include "Curve.h"

class Surface : public Spline 
{
public:
	Surface() {}
};


class SurfaceOfRevolution : public Surface 
{
public:
	SurfaceOfRevolution(Curve* c) : curve(c) {}

	virtual void set(int i, const Vec3f& v) { curve->set(i, v); }
	virtual int getNumVertices() { return curve->getNumVertices(); }
	virtual Vec3f getVertex(int i) { return curve->getVertex(i); }

	virtual void Paint(ArgParser* args);

	virtual void moveControlPoint(int selectedPoint, float x, float y);
	virtual void addControlPoint(int selectedPoint, float x, float y);
	virtual void deleteControlPoint(int selectedPoint);

	virtual TriangleMesh* OutputTriangles(ArgParser* args);

	virtual void OutputBSpline(FILE* file);

private:
	Curve* curve;
};

class BezierPatch : public Surface 
{
public:
	BezierPatch() 
	{
		num_vertices = 16;
		vertices.resize(num_vertices);
	}

	virtual void set(int i, const Vec3f& v) { vertices[i] = v; }
	virtual int getNumVertices() { return num_vertices; }
	virtual Vec3f getVertex(int i) { return vertices[i]; }

	virtual void Paint(ArgParser* args);

	virtual TriangleMesh* OutputTriangles(ArgParser* args);

private:
	int num_vertices;
	std::vector<Vec3f> vertices;
};


#endif // !_SURFACE_
