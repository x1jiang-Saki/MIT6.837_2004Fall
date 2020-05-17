#include <string.h>
#include <cassert>
#include <cstdio>
#include "_matrix.h"
#include "_image.h"
#include "ifs.h"

// ifs -input sierpinski_triangle.txt -points 10000 -iters 0 -size 200 -output sierpinski_triangle_0.tga
int main(int argc, char** argv)
{
	// parse code
	char* input_file = nullptr;
	int num_points = 10000;
	int num_iters = 10;
	int size = 100;
	char* output_file = nullptr;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-input")) {
			i++; assert(i < argc);
			input_file = argv[i];
		}
		else if (!strcmp(argv[i], "-points")) {
			i++; assert(i < argc);
			num_points = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-iters")) {
			i++; assert(i < argc);
			num_iters = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-size")) {
			i++; assert(i < argc);
			size = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-output")) {
			i++; assert(i < argc);
			output_file = argv[i];
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}

	Image output_tga = Image(size, size);
	output_tga.SetAllPixels(Vec3f(1.0, 1.0, 1.0));
	
	IFS ifs;
	ifs.input(input_file);
	ifs.render(output_tga, num_points, num_iters);

	output_tga.SaveTGA(output_file);
}