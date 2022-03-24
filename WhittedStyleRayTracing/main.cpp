/*****************************************************************************
* @File : D:\Games101\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\main.cpp
* @Author : Wang ZhiHao
* @Date : 2021/9/13 17:57
*****************************************************************************/

#include <iostream>
#include "World.h"
#include "Camera.h"
#include "Global.h"

void WriteColorToFile(FILE* pFile, const Color3 color, int sampleCount)
{
	auto r = color.x;
	auto g = color.y;
	auto b = color.z;

	auto scale = 1.0f / sampleCount;
	/*gamma 2*/
	/*
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);
	*/
	
	r = scale * r;
	g = scale * g;
	b = scale * b;
	
	auto ir = static_cast<int>(256 * std::pow(Clamp(0.0f, 0.999, r), 0.6f));
	auto ig = static_cast<int>(256 * std::pow(Clamp(0.0f, 0.999, g), 0.6f));
	auto ib = static_cast<int>(256 * std::pow(Clamp(0.0f, 0.999, b), 0.6f));

	fprintf(pFile, "%d %d %d\n", ir, ig, ib);
}

int main()
{
	const float aspect_radio = 1280.0f / 960.0f;
	const int image_width = 1280;
	const int image_height = static_cast<int>(image_width / aspect_radio);
	const float fov = 90.f;
	const float focus_dis = image_height * 0.5f / std::tanf(Degrees_to_Radians(fov * 0.2));

	const char* szFileName = "whitted.ppm";

	Point3 camPos(0, 0, 0);
	Point3 lookat(0, 0, -1);
	Vector3f up(0, 1, 0);
	Camera cam(camPos, (lookat-camPos).Nor(), up,fov,aspect_radio, focus_dis);

	World myWorld;

	int sample_count = 1;
	std::cout << "请输入采样次数，默认次数为:" << sample_count << std::endl;
	std::cin >> sample_count;

	std::cout << "请输入光线最大弹射次数:" << std::endl;
	int maxDepth = 0;
	std::cin >> maxDepth;
	myWorld.SetMatDepth(maxDepth);

	std::cout << "请输入是否采用BVH:" << std::endl;
	std::cout << "0 不使用 其他使用:" << std::endl;
	bool bUseBVH = true;
	std::cin >> bUseBVH;
	myWorld.SetUseBVH(bUseBVH);

	myWorld.CreateWorldObj();

	std::cout << "image_width:" << image_width << "	image_height:" << image_height << std::endl;
	std::cout << "sample_count:" << sample_count << "	maxDepth:" << maxDepth << "	bUseBVH:" << bUseBVH << std::endl;
	
	FILE* pColorFile = nullptr;
	fopen_s(&pColorFile, szFileName, "w+");
	if (!pColorFile)
	{
		return 0;
	}
	fprintf(pColorFile, "P3\n%d %d\n255\n", image_width, image_height);

	time_t start, stop;
	time(&start);
	std::cout << "RayTracing……" << std::endl;
	for (int i=image_height-1;i>=0;i--)
	{
		std::cerr << "\rScanlines remaining: " << i << ' ' << std::flush;
		for (int j=0;j<image_width;j++)
		{
			Color3 color;
			for (int s=0;s< sample_count;s++)
			{
				auto u = (j + Get_random_float()) / (image_width - 1);
				auto v = (i + Get_random_float()) / (image_height - 1);

				Ray ray = cam.GetRay(u, v);
				color +=  myWorld.RayTraceColor(ray, 0);
			}
			WriteColorToFile(pColorFile, color, sample_count);
		}
	}
	fclose(pColorFile);

	time(&stop);
	double diff = difftime(stop, start);
	int hrs = (int)diff / 3600;
	int mins = ((int)diff / 60) - (hrs * 60);
	int secs = (int)diff - (hrs * 3600) - (mins * 60);

	printf(
		"\rRayTracing complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
		hrs, mins, secs);

	system("pause");
	return 0;
}