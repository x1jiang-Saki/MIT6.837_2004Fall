// fopen&fscanf: 
//		This function or variable may be unsafe. Consider using fopen_s instead.
//		To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
#define _CRT_SECURE_NO_WARNINGS 

#include "ifs.h"

void IFS::input(const char* file)
{
	FILE* input = fopen(file, "r"); assert(input != nullptr);
	fscanf(input, "%d", &num_trans); assert(num_trans > 0);

	matrix_tran = new Matrix[num_trans];
	probability = new float[num_trans];

	for (int i = 0; i < num_trans; i++)
	{
		fscanf(input, "%f", &probability[i]);
		matrix_tran[i].Read3x3(input);
	}

	fclose(input);
}
void IFS::render(Image& image, int num_points, int num_iters)
{
	int w = image.Width();
	int h = image.Height();

	//-- for "lots" of random points(x0, y0)
	for (int i = 0; i < num_points; i++)
	{
		Vec2f randPoint = Vec2f(float(rand()) / RAND_MAX, float(rand()) / RAND_MAX);
		//-- for j = 0 to num_iters
		for (int j = 0; j < num_iters; j++)
		{
			//-- pick a random transformation
			float randProb = float(rand()) / RAND_MAX;
			float sumTmp = 0.0;
			for (int k = 0; k < num_trans; k++)
			{
				sumTmp += probability[k];
				if (sumTmp > randProb)
				{
					//-- (xk + 1, yk + 1) = fi(xk, yk)
					matrix_tran[k].Transform(randPoint);
					break;
				}
			}
		}
		//-- display a dot at(xk, yk)
		if(randPoint.x() >= 0 && randPoint.x() <= 1 && randPoint.y() >= 0 && randPoint.y() <= 1)
			image.SetPixel(randPoint.x() * w, randPoint.y() * h, Vec3f(0.0, 0.0, 0.0));
	}
}
