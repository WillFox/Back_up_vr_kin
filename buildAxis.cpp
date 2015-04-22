#include <vector>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <math.h>
//#include "cvplot.h"
using namespace cv;
using namespace std;

const string DIR_IMG = "/home/william/project/software/vr_kinetic/converter/blob_cv/";
const string IMG_PREF = "CV_";
//ex:SBS_TF00002.txt
const string DIR_BLOB = "/home/william/project/software/vr_kinetic/converter/blobs/";
const string BLOB_PREF = "Blobs_";
//ex:Blobs_TF00002.txt
const string DIR_DATA = "/home/william/project/software/vr_kinetic/converter/region/";
const string DATA_PREF = "RefineRegion_";
RNG rng(12345);
//ex:RefineRegion_TF00002.txt

int main(int argc, char** argv )
{

	string line;
	bool notate = true;//prints information related to program progress
	int xLength=75;	//The length of the image
	int yLength=75;	//The height of the image
	unsigned char bits[16875];
	memset(bits, 0, sizeof bits);
	std::string fileName = DIR_BLOB;
	int xBlob=0;
	int yBlob=0;
	int fileNum=1;//data file to start at ***IMPORTANT TO SET CORRECTLY***
	int totalFiles=2;
	string numPrefix="0";
	std::stringstream ss;
	char gradient[765];
	//CREATE GRADIENT BAR ON SIDE OF GRAPH
	for(int i = 0;i<255;i+=1)
	{
		if(i>=255)
		{
			gradient[3*i]=0;//Blue
			gradient[3*i+1]=0;//Green
			gradient[3*i+2]=255;//Red
		}
		else
		{
			gradient[3*i]=-255*log10((double)i/260);//Blue
			gradient[3*i+1]=(cos(2*i*3.141592/255+3.141592)+1)*20;//Green
			gradient[3*i+2]=pow(i/16.5,2);//Red
		}
		//printf("%d;%d;%d\n", gradient[i*3],gradient[i*3+1],gradient[i*3+2]);

	}
	//go through files in data folder one at a time
	for(fileNum;fileNum < totalFiles+1;fileNum++)
	{
		//string file="Blobs_TF"+ +".txt"
		fileName=DIR_BLOB;
		//add the file number to differentiate (create separate function later)
		if(fileNum<10)
		{
			numPrefix="TF0000";
		}
		else if(fileNum<100)
		{
			numPrefix="TF000";
		}
		else if(fileNum<1000)
		{
			numPrefix="TF00";
		}
		else if(fileNum<10000)
		{
			numPrefix="TF0";
		}
		else
		{
			numPrefix="TF";
		}
		fileName+=BLOB_PREF;
		fileName+=numPrefix;
		ss.str("");
		ss<<fileNum;
		std::string str = ss.str();
		fileName+=str;
		fileName+=".txt";
		ifstream interestPoints (fileName.c_str());
		fileName=DIR_DATA;
		fileName+=DATA_PREF;
		fileName+=numPrefix;
		fileName+=str;
		fileName+=".txt";
		ifstream dataPoints (fileName.c_str());
		double minZ;
		double maxZ;
		double a;
		if (interestPoints.is_open() && dataPoints.is_open())
		{
			if(notate==true)
			{
				cout << "Files successfully opened:"<<fileNum<<endl;	
			}
			if(getline(dataPoints, line, ' '))
			{
				minZ=atoi(line.c_str());
			}
			if(getline(dataPoints, line, '\n'))
			{
				maxZ=atoi(line.c_str());
			}
			//extracts and prints each data point scaling each one.  
			double temp = 0.0;
			for (int k = 0 ;k<yLength*xLength;k++)
			{
				dataPoints>>a;
				temp=a*a*90;
				if(temp>=255)
				{
					bits[3*k]=0;//Blue
					bits[3*k+1]=0;//Green
					bits[3*k+2]=255;//Red
				}
				else
				{
					bits[3*k]=-255*log10((double)temp/260);//Blue
					bits[3*k+1]=(cos(2*temp*3.141592/255+3.141592)+1)*20;//Green
					bits[3*k+2]=pow(temp/16.5,2);//Red
				}
			}
			Mat m( xLength,yLength, CV_8UC3, bits );
			//Extract blobs detected
			bool extracting=true;	
			while(extracting)
			{	
				if(getline (interestPoints, line,' '))
				{
					xBlob=atoi(line.c_str());
				}
				if(getline (interestPoints, line,'\n'))
				{
					yBlob=atoi(line.c_str());
				}
				else
				{
					if(notate==true)
					{
						cout << "Extraction finishing..."<<endl;	
					}
					extracting=false;
				}
				bits[xBlob*3+yBlob*xLength*3]=0;//Blue
				bits[xBlob*3+yBlob*xLength*3+1]=255;//Green
				bits[xBlob*3+yBlob*xLength*3+2]=0;//Red
			}
			interestPoints.close();
			dataPoints.close();
			if(notate==true)
			{
				cout << "Building Axis" << endl;
			}
			int thickness = 2;
			int lineType = 8;
			//start=Point(0,12);
			//end=Point(70,12);
			fileName=DIR_IMG;
			fileName+=IMG_PREF;
			fileName+=numPrefix;
			fileName+=str;
			fileName+=".jpg";
			if(notate==true)
			{
				cout << fileName << endl;
			}
			Mat mLarge, dst;
			Size size(600,600);
			resize(m,mLarge,size);
			Size blurSize(5,5);
			//blur(mLarge,mLarge,blurSize);
			

			//TEST DIFFERENT BLUR METHODS
			int MAX_KERNEL_LENGTH = 31;
			int DELAY_BLUR=100;
			int DELAY_CAPTION = 1500;
			Mat src;
			src=mLarge;
			dst=src.clone();
   			/// Applying Homogeneous blur
   			//for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
       		//	{ blur( src, dst, Size( i, i ), Point(-1,-1) );
       		//	}
    		/// Applying Gaussian blur
    		for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
        		{ GaussianBlur( src, dst, Size( i, i ), 0, 0 );
        		}			
     		/// Applying Median blur
     		//for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
         	//	{ medianBlur ( src, dst, i ); 
         	//	}			
     		/// Applying Bilateral Filter}			
     		//for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
         	//	{ bilateralFilter ( src, dst, i, i*2, i/2 );
         	//	}

			//line(m, Point(0,12), Point(70,12), Scalar(100,100,0), thickness, lineType);
			//imwrite(fileName,m);
			namedWindow("Display Image",CV_WINDOW_AUTOSIZE);//, WINDOW_NORMAL);
			
			int top,bottom,left,right;
			top = (int) (0.10*mLarge.rows); bottom = (int) (0.15*mLarge.rows);
			left = (int) (0.25*mLarge.cols); right = (int) (0.2*mLarge.cols);
			
			dst=mLarge;
			int borderType=BORDER_CONSTANT;
			Scalar value;
			value = Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
			value = Scalar(255,255,255,255);
			copyMakeBorder( mLarge,dst,top,bottom,left,right,borderType, value);
			
			//ADD GRADIENT
			Mat gradImg(255,500,CV_8UC3,Scalar(0,0,255));
			for(int n=0 ; n<255;n+=1)
			{
				for( int m = 0 ; m < 500 ; m+=1)
				{
					gradImg.at<Vec3b>(n,m)[0]=gradient[(3*n)+0];
					gradImg.at<Vec3b>(n,m)[1]=gradient[(3*n)+1];
					gradImg.at<Vec3b>(n,m)[2]=gradient[(3*n)+2];
				}
			}

			//CREATE AXIS
			putText(dst,"Relative Density for Poloidal Plane:"+numPrefix+str,Point((int) (mLarge.cols/2+0*mLarge.cols),50),CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255),1,8,false);

			imshow("Display Image", gradImg );
			waitKey(0);	
			//cout<<sizeof m<<endl;
		}


	}
	return 0;
}

