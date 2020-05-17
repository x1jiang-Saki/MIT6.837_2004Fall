#include "_scene_parser.h"
#include "_image.h"

// raytracer -input scene1_01.txt -size 200 200 -output output1_01.tga -depth 9 10 depth1_01.tga
int main(int argc, char** argv)
{
	// parse code
	char* input_file = nullptr;
	int width = 100;
	int height = 100;
	char* output_file = nullptr;
	float depth_min = 0;
	float depth_max = 1;
	char* depth_file = nullptr;
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
		else if (!strcmp(argv[i], "-depth")) {
			i++; assert(i < argc);
			depth_min = atof(argv[i]);
			i++; assert(i < argc);
			depth_max = atof(argv[i]);
			i++; assert(i < argc);
			depth_file = argv[i];
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}

	SceneParser scene = SceneParser(input_file);
	Camera* camera = scene.getCamera();
	Vec3f bgColor = scene.getBackgroundColor();
	//Material
	Object3D* objects = scene.getGroup();

	Image output_scene = Image(width, height);
	output_scene.SetAllPixels(bgColor);
	Image output_depth = Image(width, height);
	output_depth.SetAllPixels(Vec3f(0.0, 0.0, 0.0));

	//-- for every pixel
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			//-- construct a ray from the eye
			Ray ray = camera->generateRay(Vec2f(float(i) / width, float(j) / height));
			//-- for every object in the scene
			//-- find intersection with the ray
			Hit hit(INFINITY,nullptr);
			if (objects->intersect(ray, hit, camera->getTMin()))
			{
				// keep if closet

				output_scene.SetPixel(i, j, hit.getMaterial()->getDiffuseColor());

				float t = hit.getT();
				if (t > depth_max) t = depth_max;
				if (t < depth_min) t = depth_min;
				t = (depth_max - t) / (depth_max - depth_min);
				output_depth.SetPixel(i, j, Vec3f(t, t, t));
			}
		}
	}

	output_scene.SaveTGA(output_file);
	output_depth.SaveTGA(depth_file);
	return 0;
}