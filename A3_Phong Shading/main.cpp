#include "_scene_parser.h"
#include "_image.h"
#include <algorithm>
#include "_glCanvas.h"
#include <gl\glut.h>

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

SceneParser* scene;

void parseCode(int argc, char** argv);
void render();

//raytracer - input scene3_08_sphere.txt - size 200 200 - output output3_08.tga - gui - tessellation 20 10 - gouraud
int main(int argc, char** argv)
{
	parseCode(argc, argv);
	scene = new SceneParser(input_file);
	if (gui)
	{
		GLCanvas canvas;
		canvas.initialize(scene, render);
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
		else if (!strcmp(argv[i], "-normals")) {
			i++; assert(i < argc);
			normal_file = argv[i];
		}
		else if (!strcmp(argv[i], "-depth")) {
			i++; assert(i < argc);
			depth_min = atof(argv[i]);
			i++; assert(i < argc);
			depth_max = atof(argv[i]);
			i++; assert(i < argc);
			depth_file = argv[i];
		}
		else if (!strcmp(argv[i], "-shade_back")) {
			shade_back = true;
		}
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
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
}

void render()
{
	Camera* camera = scene->getCamera();
	Vec3f bgColor = scene->getBackgroundColor();
	Vec3f ambientColor = scene->getAmbientLight();
	//Material
	Object3D* objects = scene->getGroup();

	Image output_scene = Image(width, height);
	output_scene.SetAllPixels(bgColor);
	Image output_depth = Image(width, height);
	output_depth.SetAllPixels(Vec3f(0.0, 0.0, 0.0));
	Image output_normal = Image(width, height);
	output_normal.SetAllPixels(Vec3f(0.0, 0.0, 0.0));

	//-- for every pixel
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			//-- construct a ray from the eye
			Ray ray = camera->generateRay(Vec2f(float(i) / width, float(j) / height));
			//-- for every object in the scene
			//-- find intersection with the ray
			Hit hit(INFINITY, nullptr, Vec3f(0.0, 0.0, 0.0));
			if (objects->intersect(ray, hit, camera->getTMin()))
			{
				Vec3f resultCol = ambientColor * hit.getMaterial()->getDiffuseColor();
				// shading for each light
				for (int lit = 0; lit < scene->getNumLights(); lit++)
				{
					Light* curLight = scene->getLight(lit);
					Vec3f point, lightDir, lightCol;
					float distance;
					// now we only have Directional light, it doesn't matter what the value is the the hit Point
					// the direction and color are all the same
					curLight->getIllumination(point, lightDir, lightCol, distance);

					resultCol += hit.getMaterial()->Shade(ray, hit, lightDir, lightCol);

					//// shade_back
					//if (shade_back)
					//	resultCol += abs(lightDir.Dot3(hit.getNormal())) * lightCol;
					//else
					//	resultCol += max(0.0f, lightDir.Dot3(hit.getNormal())) * lightCol;
				}
				output_scene.SetPixel(i, j, resultCol);
				output_normal.SetPixel(i, j, Vec3f(fabs(hit.getNormal().x()), fabs(hit.getNormal().y()), fabs(hit.getNormal().z())));

				float t = hit.getT();
				if (t > depth_max) t = depth_max;
				if (t < depth_min) t = depth_min;
				t = (depth_max - t) / (depth_max - depth_min);
				output_depth.SetPixel(i, j, Vec3f(t, t, t));
			}
		}
	}
	if (output_file != nullptr)
		output_scene.SaveTGA(output_file);
	if (depth_file != nullptr)
		output_depth.SaveTGA(depth_file);
	if (normal_file != nullptr)
		output_normal.SaveTGA(normal_file);
}
