#include <iostream>
#include <stdio.h>
#include <cstring>
#include <string>
using namespace std;

class YUV{
public:
	char *Y, *U, *V;
	int width, height, pixels;
	YUV(){
		Y = U = V = NULL;
		width = height = pixels = 0;
	}
	YUV(const int& w, const int& h){
		width = w;
		height = h;
		pixels = width * height;
		Y = new char[pixels];
		U = new char[pixels / 4];
		V = new char[pixels / 4];
	}
	YUV(const YUV& yuv){
		width = yuv.width;
		height = yuv.height;
		pixels = yuv.pixels;
		if(Y)
			delete Y;
		if(U)
			delete U;
		if(V)
			delete V;

	}
	~YUV(){
		if(Y)
			delete Y;
		if(U)
			delete U;
		if(V)
			delete V;
	}
};

int main(int argc, char** argv){
	if(argc != 3)
	{
		printf("Invalid command\n");
		return 0;
	}

	FILE *fin, *fout;
	fdin = fopen(argv[1], "r");
	if(!fin)
	{
		printf("No such file\n");
		return 0;
	}
	fout = fopen(argv[2], "rw");
	if(!fout)
	{
		printf("Output file error\n");
		return 0;
	}

	const int width = 1920;
	const int height = 1080;

	int pixels = width * height;
	char* Y = new char[pixels];
	char* U = new char[pixels / 4];
	char* V = new char[pixels / 4];

	char* R = new char[pixels];
	char* G = new char[pixels];
	char* B = new char[pixels];
	char* A = new char[pixels];

	fread(Y, 1, pixels, fin);
	fread(U, 1, pixels / 4, fin);
	fread(V, 1, pixels / 4, fin);

	for(int a = 1; a <= 255; a += 3)
	{
		char str[20];
		itoa(a, str, 10);
		char filename[30] = "alpha";
		strcat(filename, str);
		strcat(filename, ".yuv");
		fout = fopen(filename, "rw");
		if(!fout)
		{
			printf("Output file error\n");
			return 0;
		}
		for(int i = 0; i < pixels; ++i)
		{
			int row = i / width;
			int col = i % width;
			int index = rol / 2 * width / 2 + col / 2;
			R[i] = Y[i] + 1.140 * V[index];
			G[i] = Y[i] - 0.394 * U[index] - 0.581 * V[index];
			B[i] = Y[i] + 2.032 * U[index];
			A[i] = a;
		}
	}

	return 0;
}



//下面的4个函数应该统计出图像处理的时间;
//函数参数和返回值可以需要自己定.
int process_without_simd(){
	int time = 0;

	/*处理过程*/

	return time;
}

int process_with_mmx(){
	int time = 0;

	/*处理过程*/

	return time;
}

int process_with_sse(){
	int time = 0;

	/*处理过程*/

	return time;
}

int process_with_avx(){
	int time = 0;
	/*处理过程*/


	return time;
}