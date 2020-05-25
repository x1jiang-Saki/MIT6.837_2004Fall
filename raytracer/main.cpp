#include "_scene_parser.h"
#include "_image.h"
#include <algorithm>
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

SceneParser* scene;

void parseCode(int argc, char** argv);
void render();

int main(int argc, char** argv)
{
	parseCode(argc, argv);
	scene = new SceneParser(input_file);
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
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
}

void render()
{
	Camera* camera = scene->getCamera();
	Object3D* objects = scene->getGroup();

	Image output_scene = Image(width, height);
	output_scene.SetAllPixels(scene->getBackgroundColor());
	Image output_depth = Image(width, height);
	output_depth.SetAllPixels(Vec3f(0.0, 0.0, 0.0));
	Image output_normal = Image(width, height);
	output_normal.SetAllPixels(Vec3f(0.0, 0.0, 0.0));

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			Ray ray = camera->generateRay(Vec2f(float(i) / width, float(j) / height));
			Hit hit(INFINITY, nullptr,Vec3f(0.0,0.0,0.0));
			if (objects->intersect(ray, hit, camera->getTMin()))
			{
				Vec3f baseColor = hit.getMaterial()->getDiffuseColor();

				Vec3f ambientTerm = scene->getAmbientLight() * baseColor;
				Vec3f resultCol = ambientTerm;

				Vec3f N = hit.getNormal();
				//if (N.Dot3(ray.getDirection()) > 0)
				//{
				//	if (shade_back)
				//		N = -1.0 * N;
				//	else
				//		output_scene.SetPixel(i, j, Vec3f(0.0, 0.0, 0.0)); continue;
				//}

				for (int lit = 0; lit < scene->getNumLights(); lit++)
				{
					Light* curLight = scene->getLight(lit);
					Vec3f pos = hit.getIntersectionPoint();
					Vec3f L, lightCol;
					curLight->getIllumination(pos, L, lightCol);

					float diffuse = N.Dot3(L);
					if (shade_back)
						diffuse = abs(diffuse);
					else
						diffuse = max(0.0f, diffuse);
					Vec3f diffuseTerm = diffuse * lightCol * baseColor;

					resultCol += diffuseTerm;
				}
				
				float t = hit.getT();
				if (t > depth_max) t = depth_max;
				if (t < depth_min) t = depth_min;
				t = (depth_max - t) / (depth_max - depth_min);
				Vec3f depth = Vec3f(t, t, t);

				Vec3f normal(abs(N.x()), abs(N.y()), abs(N.z()));

				output_scene.SetPixel(i, j, resultCol);
				output_depth.SetPixel(i, j, depth);
				output_normal.SetPixel(i, j, normal);
			}
		}
	}

	if (output_file) output_scene.SaveTGA(output_file);
	if (depth_file) output_depth.SaveTGA(depth_file);
	if (normal_file) output_normal.SaveTGA(normal_file);
}