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
	int totalFiles=100;
	string numPrefix="0";
	std::stringstream ss;
	unsigned char gradient[765];
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
			gradient[3*i]=-255*log10((double)(i+30)/300)+(cos((double) 2*i*3.141592/255+3.141592)+1)*30;//Blue
			gradient[3*i+1]=(cos((double) 2*i*3.141592/255+3.141592)+1)*127;//Green
			gradient[3*i+2]=pow((double) i/41.0,(double) 3.0);//Red
		}
		//printf("%u;%u;%u\n", gradient[i*3],gradient[i*3+1],gradient[i*3+2]);

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
				temp=pow(a,1.3)*70;//a*a*90;//pow(a,6);
				if(temp>=255)
				{
					bits[3*k]=0;//Blue
					bits[3*k+1]=0;//Green
					bits[3*k+2]=170;//Red
				}
				else
				{
					bits[3*k]=-255*log10((double)(temp+30)/300)+(cos((double) 2*temp*3.141592/255+3.141592)+1)*30;//Blue
					bits[3*k+1]=(cos((double)2*temp*3.141592/255+3.141592)+1)*127;//Green
					bits[3*k+2]=pow((double) temp/41.0,(double) 3);//pow(temp/40.0,3);//Red
				}
				//printf("%lf:  %u;%u;%u\n", temp, bits[k*3],bits[k*3+1],bits[k*3+2]);
			}
			Mat m( xLength,yLength, CV_8UC3, bits );
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
			int i;
   			/// Applying Homogeneous blur (dulls the highs and lows)
   			i = 21;//max=MAX_KERNAL_LENGTH and must be odd
   			blur( src, mLarge, Size( i, i ), Point(-1,-1) );
       		i = 23;
       		blur( src, mLarge, Size( i, i ), Point(-1,-1) );
    		/// Applying Gaussian blur (Very slow compared to homogeneous)
    		//i=25;//max=MAX_KERNAL_LENGTH and must be odd
        	//GaussianBlur( src, mLarge, Size( i, i ), 0, 0 );			
     		/// Applying Median blur (Very slow compared to Gaussian 
     		/// and shows discontinuous gradient (might be a good thing though))
         	//i=29;//max=MAX_KERNAL_LENGTH and must be odd
         	//medianBlur ( src, mLarge, i );		
     		/// Applying Bilateral Filter (By far the slowest algorithm of these 4)	
     		//i = 21;//max=MAX_KERNAL_LENGTH and must be odd
         	//bilateralFilter ( dst, mLarge, i, i*2, i/2 );

			//line(m, Point(0,12), Point(70,12), Scalar(100,100,0), thickness, lineType);
			
			//Extract and mark blobs detected
			bool extracting=true;	
			while(extracting)
			{	
				xBlob=-1;
				yBlob=-1;
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
				if(xLength!=-1 && yBlob!=-1)
				{
					circle(mLarge,
						Point((int) (xBlob*1.0/xLength*mLarge.cols),(int)(yBlob*1.0/yLength*mLarge.rows)),
						3.0,//radius
						Scalar(0,0,255),//color
						2,//thickness
						8);//linetype
				}
				//bits[xBlob*3+yBlob*xLength*3]=0;//Blue
				//bits[xBlob*3+yBlob*xLength*3+1]=255;//Green
				//bits[xBlob*3+yBlob*xLength*3+2]=255;//Red
			}
			interestPoints.close();
			namedWindow("Display Image",CV_WINDOW_AUTOSIZE);//, WINDOW_NORMAL);
			
			int top,bottom,left,right;
			double pad_top,pad_bottom,pad_left,pad_right;
			pad_top = 0.10;
			pad_bottom = 0.15;
			pad_left = 0.25;
			pad_right = 0.25;
			top = (int) (pad_top*mLarge.rows); bottom = (int) (pad_bottom*mLarge.rows);
			left = (int) (pad_left*mLarge.cols); right = (int) (pad_right*mLarge.cols);
			
			dst=mLarge;
			int borderType=BORDER_CONSTANT;
			Scalar value;
			value = Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
			value = Scalar(255,255,255,255);
			copyMakeBorder( mLarge,dst,top,bottom,left,right,borderType, value);
			
			//ADD GRADIENT
			Mat gradImg(255,500,CV_8UC3,Scalar(0,0,255));
			//Nice to put this in memory rather than generating each timeframe
			int index;
			for(int n=0 ; n<mLarge.rows;n+=1)
			{
				index=(int) 255.0*n/mLarge.rows;
				//printf("%d\n",index);
				for( int m = 0 ; m < 50 ; m+=1)
				{
					dst.at<Vec3b>((int) (pad_top*mLarge.rows+n),(int) (pad_left*mLarge.cols+mLarge.cols+0.1*pad_right*mLarge.cols+m))[0]=gradient[(int) 3*index+0];
					dst.at<Vec3b>((int) (pad_top*mLarge.rows+n),(int) (pad_left*mLarge.cols+mLarge.cols+0.1*pad_right*mLarge.cols+m))[1]=gradient[(int) 3*index+1];
					dst.at<Vec3b>((int) (pad_top*mLarge.rows+n),(int) (pad_left*mLarge.cols+mLarge.cols+0.1*pad_right*mLarge.cols+m))[2]=gradient[(int) 3*index+2];
					//printf("%d;%d;%d\n",gradient[(int) (3*n/4)+0],gradient[(int) (3*n/4)+1],gradient[(int) (3*n/4)+2]);
				}
			}

			//CREATE AXIS
			maxZ = 0.25;
			minZ = -0.25;
			double maxR = 2.32;
			double minR = 2.25;
			int labelX = 8;
			int labelY = 12;
			double label_add;
			putText(dst,"Relative Density for Poloidal Plane:"+numPrefix+str,Point((int) (mLarge.cols/2),50),CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255),1,8,false);
			//X Axis components
			int xpoints;
			for(xpoints=0;xpoints<=labelX;xpoints+=1)
			{
				label_add=(maxR-minR)/(1.0*labelX)*(labelX-xpoints)+minR;
				ss.str("");
				ss<<label_add;
				std::string str = ss.str();
				putText(dst,
					str,
					Point((int) (pad_left*mLarge.cols+(1.0*xpoints/labelX*mLarge.cols)-15),(int) (dst.rows-pad_bottom*0.75*mLarge.rows)),
					CV_FONT_HERSHEY_SIMPLEX, 
					0.5, 
					Scalar(255),
					1,8,false);
			}
			//Y Axis components
			int ypoints;
			for(ypoints=0;ypoints<=labelY;ypoints+=1)
			{
				label_add=(maxZ-minZ)/(1.0*labelY)*(labelY-ypoints)+minZ;
				ss.str("");
				ss<<label_add;
				std::string str = ss.str();
				putText(dst,
					str,
					Point((int) (0.7*pad_left*mLarge.cols),(int) (pad_top*mLarge.rows+(1.0*ypoints/labelY)*mLarge.rows)),
					CV_FONT_HERSHEY_SIMPLEX, 
					0.5, 
					Scalar(255),
					1,8,false);
			}
			//putText(dst,"Relative Density for Poloidal Plane:"+numPrefix+str,Point((int) (mLarge.cols/2+0*mLarge.cols),50),CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255),1,8,false);
			//putText(dst,"Relative Density for Poloidal Plane:"+numPrefix+str,Point((int) (mLarge.cols/2+0*mLarge.cols),50),CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255),1,8,false);
			//putText(dst,"Relative Density for Poloidal Plane:"+numPrefix+str,Point((int) (mLarge.cols/2+0*mLarge.cols),50),CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255),1,8,false);
			//putText(dst,"Relative Density for Poloidal Plane:"+numPrefix+str,Point((int) (mLarge.cols/2+0*mLarge.cols),50),CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255),1,8,false);


			imwrite(fileName,dst);
			imshow("Display Image", dst );
			waitKey(50);	
			//cout<<sizeof m<<endl;
		}


	}
	return 0;
}

