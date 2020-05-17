#ifndef _IFS_H_
#define _IFS_H_
#include "_matrix.h"
#include "_image.h"

class IFS
{
public:
	// CONSTRUCTOR
	IFS()
	{
		num_trans = 0;
		matrix_tran = nullptr;
		probability = nullptr;
	}
	~IFS()
	{
		delete[] matrix_tran;
		delete[] probability;
	}

	// METHOD
	void input(const char* file);
	void render(Image& image, int num_points, int num_iters);

private:
	int      num_trans;	  // number of transformtions
	Matrix*  matrix_tran; // the array of transformations
	float*   probability; // the correspongding probabilities of each transformation
};
#endif // _IFS_H