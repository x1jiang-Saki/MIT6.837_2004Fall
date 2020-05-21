#include "_vectors.h"

class MarchingInfo
{
public:
	MarchingInfo() 
	{
		t_cur = INFINITY;
		t_next_x = INFINITY;
		t_next_y = INFINITY;
		t_next_z = INFINITY;
	}

	void nextCell();

	float t_cur; // the current value of tmin
	int i, j, k; // the grid indices
	float t_next_x, t_next_y, t_next_z; // the next values of intersection along each axis
	float dt_x, dt_y, dt_z; // the marching increments along each axis
	int sign_x, sign_y, sign_z; // and the sign of the direction vector components
	// To render the occupied grid cells for visualization 
	// you will also need to store the surface normal of the cell face
	// which was crossed to enter the current cell
	Vec3f normal;

};