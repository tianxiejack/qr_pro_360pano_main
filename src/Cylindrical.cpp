#include"Cylindrical.hpp"
#include"osa.h"
#include"math.h"

void CylindricalWithImgData(unsigned char  *srcdata, unsigned char *drcdata,int width,int height)
{
	
	int i_original_img_hnum, i_original_img_wnum;
	double distance_to_a_y, distance_to_a_x;
	int original_point_a, original_point_b, original_point_c, original_point_d;

	int l_width = width* 3;
	int drcpoint;
	double R = 1200;
	double x, y;
	for (int hnum = 0; hnum < height; hnum++)
	{
		for (int wnum = 0; wnum < width; wnum++)
		{
			drcpoint = l_width*hnum + wnum * 3;

			double k = R / sqrt(R*R + (wnum- width / 2) * (wnum - width / 2));
			x = (wnum - width / 2) / k + width / 2;
			y = (hnum - height / 2) / k + height / 2;
			if (x >= 0 && y >= 0 && x < width && y < height)
			{
			
				i_original_img_hnum = y;
				i_original_img_wnum = x;
				distance_to_a_y = y - i_original_img_hnum;
				distance_to_a_x = x - i_original_img_wnum;

				original_point_a = i_original_img_hnum*l_width + i_original_img_wnum * 3;
				original_point_b = original_point_a + 3;
				original_point_c = original_point_a + l_width;
				original_point_d = original_point_c + 3;

				if (i_original_img_hnum == height - 1)
				{
					original_point_c = original_point_a;
					original_point_d = original_point_b;
				}
				if (i_original_img_wnum == width - 1)
				{
					original_point_a = original_point_b;
					original_point_c = original_point_d;
				}

				drcdata[drcpoint + 0] =
					srcdata[original_point_a + 0] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
					srcdata[original_point_b + 0] * distance_to_a_x*(1 - distance_to_a_y) +
					srcdata[original_point_c + 0] * distance_to_a_y*(1 - distance_to_a_x) +
					srcdata[original_point_c + 0] * distance_to_a_y*distance_to_a_x;
				drcdata[drcpoint + 1] =
					srcdata[original_point_a + 1] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
					srcdata[original_point_b + 1] * distance_to_a_x*(1 - distance_to_a_y) +
					srcdata[original_point_c + 1] * distance_to_a_y*(1 - distance_to_a_x) +
					srcdata[original_point_c + 1] * distance_to_a_y*distance_to_a_x;
				drcdata[drcpoint + 2] =
					srcdata[original_point_a + 2] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
					srcdata[original_point_b + 2] * distance_to_a_x*(1 - distance_to_a_y) +
					srcdata[original_point_c + 2] * distance_to_a_y*(1 - distance_to_a_x) +
					srcdata[original_point_c + 2] * distance_to_a_y*distance_to_a_x;
			
			}
		}
	}
}

/*
Mat cylinder(Mat imgIn, int f)
{
    int colNum, rowNum;
    colNum = 2 * f*atan(0.5*imgIn.cols / f);
    rowNum = 0.5*imgIn.rows*f / sqrt(pow(f, 2)) + 0.5*imgIn.rows;

    Mat imgOut = Mat::zeros(rowNum, colNum, CV_8UC1);
    Mat_<uchar> im1(imgIn);
    Mat_<uchar> im2(imgOut);

 
    int x1(0), y1(0);
    for (int i = 0; i < imgIn.rows; i++)
        for (int j = 0; j < imgIn.cols; j++)
        {
            x1 = f*atan((j - 0.5*imgIn.cols) / f) + f*atan(0.5*imgIn.cols / f);
            y1 = f*(i - 0.5*imgIn.rows) / sqrt(pow(j - 0.5*imgIn.cols, 2) + pow(f, 2)) + 0.5*imgIn.rows;
            if (x1 >= 0 && x1 < colNum&&y1 >= 0 && y1<rowNum)
            {
                im2(y1, x1) = im1(i, j);
            }
        }
    return imgOut;
}
*/
/*
void Cylinder(unsigned char  *srcdata, unsigned char *drcdata,int nWidth,int nHeight,double focusLength,)
{
	int i,j, ix, iy;

	double k;
	double dbx;
	double dby;
	int fovAngle = (float)(2*atan(nWidth/(2.0*focusLength)));
	int cylWindth = (int)(focusLength * fovAngle + 1)/2*2;
	int wrapWindth = cylWindth;

	for(i = 0; i < wrapWindth; i++)
	{  
		double dbi = (double)i;

		dbx =  nWidth/2.0 - focusLength * tan((focusLength * fovAngle/2 - dbi)/focusLength) + 0.5;

		k = sqrt(focusLength * focusLength + (dbx - nWidth/2.0)*(dbx - nWidth/2.0));

		ix = (int)(dbx);

		for (j = 0; j< nHeight; j++)
		{
			dby = nHeight/2.0 + k*(j-nHeight/2.0)/focusLength + 0.5;
			iy = (int)(dby);

			if( ix >= 0 && ix < nWidth && iy >= 0 && iy < nHeight )
			{
				matrix[j * wrapWindth + i].x = ix;
				matrix[j * wrapWindth + i].y = iy;
			}
		}
	}

}

void Stitch_cylinder_t_plane_map(STITCH_Handle handle,double focusLength, int nWidth, int nHeight)
{
	int i,j, ix, iy;

	double k;
	double dbx;
	double dby;
	double fovAngle;
	//double scale = 2;
	Point_16S *matrix;

	int cylWindth;
	
	int wrapWindth;

	CYLMAT_HNDL planeMapMat;

	planeMapMat = handle->cylMapMat;

	planeMapMat->fx = (float)focusLength;

	planeMapMat->fovAngle = (float)(2*atan(nWidth/(2.0*focusLength)));

	planeMapMat->cylWindth = (int)(focusLength * planeMapMat->fovAngle + 1)/2*2;

	fovAngle = planeMapMat->fovAngle;

	matrix = planeMapMat->matrix;
	
	wrapWindth = planeMapMat->cylWindth;

	for(i = 0; i < wrapWindth; i++)
	{  
		double dbi = (double)i;

		dbx =  nWidth/2.0 - focusLength * tan((focusLength * fovAngle/2 - dbi)/focusLength) + 0.5;

		k = sqrt(focusLength * focusLength + (dbx - nWidth/2.0)*(dbx - nWidth/2.0));

		ix = (int)(dbx);

		for (j = 0; j< nHeight; j++)
		{
			dby = nHeight/2.0 + k*(j-nHeight/2.0)/focusLength + 0.5;
			iy = (int)(dby);

			if( ix >= 0 && ix < nWidth && iy >= 0 && iy < nHeight )
			{
				matrix[j * wrapWindth + i].x = ix;
				matrix[j * wrapWindth + i].y = iy;
			}
		}
	}
}

*/
	
