/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RayTracingManual\PPMFormat\main.cpp
* @Author : Wang ZhiHao
* @Date : 2021/9/13 17:31
*****************************************************************************/

#include <iostream>

int main()
{
    const int image_width = 256;
    const int image_height = 256;

    const char* szFileName = "test.ppm";

    FILE* pFile = nullptr;
    fopen_s(&pFile,szFileName, "w+");
    if (!pFile)
    {
        return 0;
    }
    fprintf(pFile, "P3\n%d %d\n255\n", image_width, image_height);
    for (auto i=0;i< image_height;i++)
    {
        for (auto j=0;j< image_width;j++)
        {
            auto r = double(i) / (image_width - 1);
            auto g = double(j) / (image_height - 1);
            auto b = 0.25;

            int ir = static_cast<int>(255 * r);
            int ig = static_cast<int>(255 * g);
            int ib = static_cast<int>(255 * b);

            fprintf(pFile, "%d %d %d\n", ir, ig, ib);
            std::cout << "Line:" << i << " " << "Row:" << j << std::endl;
        }
    }

    fclose(pFile);
    return 0;
}

