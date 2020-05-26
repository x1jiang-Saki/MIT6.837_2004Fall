#include "_scene_parser.h"
#include "_image.h"
#include <algorithm>
#include "_glCanvas.h"
#include "RayTracer.h"

// A1
char* input_file = nullptr;
int width = 100;
int height = 100;
char* output_file = nullptr;
float depth_min = 0;
float depth_max = 1;
char* depth_file = nullptr;

// A2
char* normal_file = nullptr;
bool shade_back = false;

// A3
bool gui = false;
bool gouraud = false;
int theta_steps = 10;
int phi_steps = 5;

// A4
int max_bounces = 0;
float cutoff_weight = 0.01;
bool shadows = false;
bool useTransparentShadows = true; // actually not working QAQ

// A5
bool visualize_grid = false;
int nx = 0, ny = 0, nz = 0;

SceneParser* scene;

void parseCode(int argc, char** argv);
void render();
void traceRay(float x, float y);

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
		// A1
		if (!strcmp(argv[i], "-input")) {
			i++; assert(i < argc);
			input_file = argv[i];
		}
		else if (!strcmp(argv[i], "-size")) {
			i++; assert(i < argc);
			width = atoi(argv[i]);
			i++; assert(i < argc);
			height = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-output")) {
			i++; assert(i < argc);
			output_file = argv[i];
		}
		else if (!strcmp(argv[i], "-depth")) {
			i++; assert(i < argc);
			depth_min = atof(argv[i]);
			i++; assert(i < argc);
			depth_max = atof(argv[i]);
			i++; assert(i < argc);
			depth_file = argv[i];
		}
		// A2
		else if (!strcmp(argv[i], "-normals")) {
			i++; assert(i < argc);
			normal_file = argv[i];
		}
		else if (!strcmp(argv[i], "-shade_back")) {
			shade_back = true;
		}
		// A3
		else if (!strcmp(argv[i], "-gui")) {
			gui = true;
		}
		else if (!strcmp(argv[i], "-tessellation")) {
			i++; assert(i < argc);
			theta_steps = atof(argv[i]);
			i++; assert(i < argc);
			phi_steps = atof(argv[i]);

		}
		else if (!strcmp(argv[i], "-gouraud")) {
			gouraud = true;
		}
		// A4
		else if (!strcmp(argv[i], "-shadows")) {
			shadows = true;
		}
		else if (!strcmp(argv[i], "-bounces")) {
			i++; assert(i < argc);
			max_bounces = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-weight")) {
			i++; assert(i < argc);
			cutoff_weight = atof(argv[i]);
		}
		// A5
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
	Image output_depth = Image(width, height);
	output_depth.SetAllPixels(Vec3f(0.0, 0.0, 0.0));
	Image output_normal = Image(width, height);
	output_normal.SetAllPixels(Vec3f(0.0, 0.0, 0.0));

	RayTracer raytracer(scene, max_bounces, cutoff_weight, shadows);
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			Ray ray = camera->generateRay(Vec2f(float(i) / width, float(j) / height));
			Hit hit(INFINITY, nullptr,Vec3f(0.0,0.0,0.0));
			
			Vec3f resultCol = raytracer.traceRay(ray, camera->getTMin(), 0.0, 0.0, 1.0, hit);

			float t = hit.getT();
			if (t > depth_max) t = depth_max;
			if (t < depth_min) t = depth_min;
			t = (depth_max - t) / (depth_max - depth_min);
			Vec3f depth = Vec3f(t, t, t);

			Vec3f N = hit.getNormal();
			Vec3f normal(abs(N.x()), abs(N.y()), abs(N.z()));

			output_scene.SetPixel(i, j, resultCol);
			output_depth.SetPixel(i, j, depth);
			output_normal.SetPixel(i, j, normal);
			
		}
	}

	if (output_file) output_scene.SaveTGA(output_file);
	if (depth_file) output_depth.SaveTGA(depth_file);
	if (normal_file) output_normal.SaveTGA(normal_file);
}

void traceRay(float x, float y)
{
	Camera* camera = scene->getCamera();
	RayTracer raytracer(scene, max_bounces, cutoff_weight, shadows);

	Ray ray = camera->generateRay(Vec2f(x, y));
	Hit hit(INFINITY, nullptr, Vec3f(0.0, 0.0, 0.0));

	Vec3f resultCol = raytracer.traceRay(ray, camera->getTMin(), 0.0, cutoff_weight, 0.0, hit);

	Hit hit2(INFINITY, nullptr, Vec3f(0.0, 0.0, 0.0));
	raytracer._grid->intersect(ray, hit2, 0.001f);
}