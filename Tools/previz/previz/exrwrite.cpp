//////////////////////////////////////////////////////////////////////////
//
// exrwrite.cpp
//
// Matlab interface for writting a float image to exr file
//
// exrwrite(img, filename)
//
// img can be 2d (gray) or 3d (color) hdr image
//
// see also exrread.cpp
// 
// Jinwei Gu. 2006/02/10
// jwgu@cs.columbia.edu
//
// When using mex to compiling it in matlab, make sure to use VC7.1 or above
// instead of VC6. 
//////////////////////////////////////////////////////////////////////////


#include "mex.h" 
#include "matrix.h"
#include "math.h"
#include "stdio.h"

#include "ImathBox.h"
#include "ImfRgba.h"
#include "ImfRgbaFile.h"
// #pragma comment(lib, "D:/OpenEXR-1.2.2-win32/lib/IlmImf_dll.lib")
// #pragma comment(lib, "D:/OpenEXR-1.2.2-win32/lib/Half.lib")
#pragma comment(lib, "D:/home/pylaffon/Common/Libraries/OpenEXR_x64/Deploy/lib/x64_Release/IlmImf.lib")
#pragma comment(lib, "D:/home/pylaffon/Common/Libraries/OpenEXR_x64/Deploy/lib/x64_Release/Half.lib")
#pragma comment(lib, "D:/home/pylaffon/Common/Libraries/OpenEXR_x64/Deploy/lib/x64_Release/Iex.lib")
#pragma comment(lib, "D:/home/pylaffon/Common/Libraries/OpenEXR_x64/Deploy/lib/x64_Release/IlmThread.lib")
#pragma comment(lib, "D:/home/pylaffon/Common/Libraries/OpenEXR_x64/Deploy/lib/x64_Release/Imath.lib")

using namespace Imf;
using namespace Imath;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) 
{
	int i, j, k, A, nd, width, height; 
	double *img = mxGetPr(prhs[0]);
	nd = mxGetNumberOfDimensions(prhs[0]);
	if(nd==2)
	{
		height = mxGetM(prhs[0]);
		width  = mxGetN(prhs[0]);
	}
	else if(nd==3)
	{
		height = mxGetM(prhs[0]);
		width  = mxGetN(prhs[0])/3;
	}
	else
	{
		printf("**Error: img must be 2d or 3d matrix\n");
		exit(1);
	}
	char outputfile[256];
	mxGetString(prhs[1], outputfile, 256);

	struct Rgba *pixels = new struct Rgba[width*height];
	

	if(nd==3)
	{
		A = width*height;
	}
	else
	{
		A = 0;
	}
	for(i=0; i<height; i++)
	for(j=0; j<width; j++)
	{
		k = j*height+i;
		pixels[i*width+j].r = img[k];
		pixels[i*width+j].g = img[k+A];
		pixels[i*width+j].b = img[k+2*A];
		pixels[i*width+j].a = 1.0f;
	}
	RgbaOutputFile file(outputfile, width, height, WRITE_RGB);
	file.setFrameBuffer(pixels, 1, width);
	file.writePixels(height);

	delete[] pixels; pixels = NULL;
} 
