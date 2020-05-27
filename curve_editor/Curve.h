#ifndef _CURVE_
#define _CURVE_

#include "Spline.h"
#include <vector>
#include "_arg_parser.h"
#include "_vectors.h"

class Curve : public Spline 
{
public:
	Curve(int num) 
	{
		num_vertices = num;
		vertices.resize(num_vertices);
	}

	virtual void set(int i, const Vec3f& v) { vertices[i] = v; }
	virtual int getNumVertices() { return num_vertices; }
	virtual Vec3f getVertex(int i) { return vertices[i]; }

	virtual void Paint(ArgParser* args);
	
protected:
	void drawLines();
	void drawPoints();
	virtual void drawCurves(ArgParser* args) = 0;

	int num_vertices;
	std::vector<Vec3f> vertices;
};

class BezierCurve : public Curve 
{
public:
	BezierCurve(int num) : Curve(num) {}

	virtual void OutputBezier(FILE* file);
	virtual void OutputBSpline(FILE* file);

	virtual void moveControlPoint(int selectedPoint, float x, float y);
	virtual void addControlPoint(int selectedPoint, float x, float y);
	virtual void deleteControlPoint(int selectedPoint);

	virtual TriangleMesh* OutputTriangles(ArgParser* args);

private:
	virtual void drawCurves(ArgParser* args);
};


class BSplineCurve : public Curve 
{
public:
	BSplineCurve(int num) : Curve(num) {}

	virtual void OutputBezier(FILE* file);
	virtual void OutputBSpline(FILE* file);

	virtual void moveControlPoint(int selectedPoint, float x, float y);
	virtual void addControlPoint(int selectedPoint, float x, float y);
	virtual void deleteControlPoint(int selectedPoint);

	virtual TriangleMesh* OutputTriangles(ArgParser* args);

private:
	virtual void drawCurves(ArgParser* args);
};

#endif // !_CURVE_
