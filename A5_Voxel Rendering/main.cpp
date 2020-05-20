#include "_scene_parser.h"
#include "_image.h"
#include <algorithm>
#include "_glCanvas.h"
#include <gl\glut.h>
#include "RayTracer.h"

char* input_file = nullptr;
int width = 100;
int height = 100;
char* output_file = nullptr;
float depth_min = 0;
float depth_max = 1;
char* depth_file = nullptr;
char* normal_file = nullptr;
bool shade_back = false;
bool gui = false;
int theta_steps = 0;
int phi_steps = 0;
bool gouraud = false;
bool shadows = false;
int max_bounces = 0;
float cutoff_weight = 1.0;
bool visualize_grid = false;
int nx = 0, ny = 0, nz = 0;

SceneParser* scene;

void parseCode(int argc, char** argv);
void render();
void traceRay(float x, float y);

//raytracer - input scene3_08_sphere.txt - size 200 200 - output output3_08.tga - gui - tessellation 20 10 - gouraud
int main(int argc, char** argv)
{
	parseCode(argc, argv);
	scene = new SceneParser(input_file);

	Grid* grid = nullptr;
	if (nx != 0) {
		grid = new Grid(scene->getGroup()->getBoundingBox(), nx, ny, nz);
		scene->getGroup()->insertIntoGrid(grid, nullptr);
	}

	if (gui)
	{
		GLCanvas canvas;
		canvas.initialize(scene, render, traceRay, grid, visualize_grid);
	}
	else
		render();
	
	delete scene;
	return 0;
}

void parseCode(int argc, char** argv)
{
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-input")) {
			i++;
			assert(i < argc);
			input_file = argv[i];
		}
		else if (!strcmp(argv[i], "-size")) {
			i++;
			assert(i < argc);
			width = atoi(argv[i]);
			i++;
			assert(i < argc);
			height = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-output")) {
			i++;
			assert(i < argc);
			output_file = argv[i];
		}
		else if (!strcmp(argv[i], "-depth")) {
			i++;
			assert(i < argc);
			depth_min = atof(argv[i]);
			i++;
			assert(i < argc);
			depth_max = atof(argv[i]);
			i++;
			assert(i < argc);
			depth_file = argv[i];
		}
		else if (!strcmp(argv[i], "-normals")) {
			i++;
			assert(i < argc);
			normal_file = argv[i];
		}
		else if (!strcmp(argv[i], "-shade_back")) {
			shade_back = true;
		}
		else if (!strcmp(argv[i], "-shadows")) {
			shadows = true;
		}
		else if (!strcmp(argv[i], "-gui")) {
			gui = true;
		}
		else if (!strcmp(argv[i], "-tessellation")) {
			i++;
			assert(i < argc);
			theta_steps = atoi(argv[i]);
			i++;
			assert(i < argc);
			phi_steps = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-gouraud")) {
			gouraud = true;
		}
		else if (!strcmp(argv[i], "-bounces")) {
			i++;
			assert(i < argc);
			max_bounces = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-weight")) {
			i++;
			assert(i < argc);
			cutoff_weight = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-grid")) {
			i++;
			assert(i < argc);
			nx = atoi(argv[i]);
			i++;
			assert(i < argc);
			ny = atoi(argv[i]);
			i++;
			assert(i < argc);
			nz = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-visualize_grid")) {
			visualize_grid = true;
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
}

void render()
{
	Camera* camera = scene->getCamera();
	Image output_scene = Image(width, height);
	output_scene.SetAllPixels(scene->getBackgroundColor());

	RayTracer rayTracer(scene,max_bounces,cutoff_weight);
	//-- for every pixel
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			//-- construct a ray from the eye
			Ray ray = camera->generateRay(Vec2f(float(i) / width, float(j) / height));

			float tmin = 0.001f;
			Hit hit(INFINITY);
			Vec3f resultCol = rayTracer.traceRay(ray, tmin, 0, 1.0, hit);
			output_scene.SetPixel(i, j, resultCol);
		}
	}
	if (output_file != nullptr)
		output_scene.SaveTGA(output_file);
}

void traceRay(float x,float y)
{
	Ray ray = scene->getCamera()->generateRay(Vec2f(x, y));
	RayTracer rayTracer(scene, max_bounces, cutoff_weight);
	float tmin = 0.001f;
	Hit hit(INFINITY);
	rayTracer.traceRay(ray, tmin, 0, 1.0, hit);
	Hit gridHit(INFINITY);
	rayTracer._grid->intersect(ray, gridHit, tmin);
}