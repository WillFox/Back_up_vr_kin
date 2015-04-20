#include <vector>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
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
					bits[3*k]=60;//Blue
					bits[3*k+1]=120;//Green
					bits[3*k+2]=255;//Red
				}
				else
				{
					/*
					Basis of algorithm from:
					Tanner Helland
					How to convert temperature (K) to RGB: Algorithm and sample code 
					www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
					*/
					//calculate BLUE
					if(temp <= 66)
					{
						bits[3*k]=255;
					}
					else
					{
						bits[3*k]=temp-60;
						bits[3*k]=329.698727446 * pow(bits[3*k],-0.1332047592);
						if(bits[3*k]<0)
						{
							bits[3*k]=0;
						}
						if(bits[3*k]>255)
						{
							bits[3*k]=255;
						}
					}
					//calculate GREEN
					if(temp<=66)
					{
						bits[3*k+1]=temp;
						bits[3*k+1]=99.4708025861 * log(bits[3*k+1]) - 161.1195681661;
						if(bits[3*k+1]<0)
						{
							bits[3*k+1]=0;
						}
						if(bits[3*k+1]>255)
						{
							bits[3*k+1]=255;
						}
					}
					else
					{
						bits[3*k+1]=temp-60;
						bits[3*k+1]=288.1221695283 * pow(bits[3*k+1], -0.0755148492);
						if(bits[3*k+1]<0)
						{
							bits[3*k+1]=0;
						}
						if(bits[3*k+1]>255)
						{
							bits[3*k+1]=255;
						}
					}
					//calculate RED
					if( temp >=66)
					{
						bits[3*k+2]=255;
					}
					else
					{
						if( temp<=19)
						{
							bits[3*k+2]=0;
						}
						else
						{
							bits[3*k+2]=temp-10;
							bits[3*k+2]=138.5177312231 * log(bits[3*k+2]) - 305.0447927307;
							if(bits[3*k+2]<0)
							{
								bits[3*k+2]=0;
							}
							if(bits[3*k+2]>255)
							{
								bits[3*k+2]=255;
							}
						}

					}
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
			Mat mLarge;
			Size size(225,225);
			resize(m,mLarge,size);
			Size blurSize(5,5);
			blur(mLarge,mLarge,blurSize);
			//line(m, Point(0,12), Point(70,12), Scalar(100,100,0), thickness, lineType);
			//imwrite(fileName,m);
			namedWindow("Display Image", WINDOW_NORMAL);
			imshow("Display Image", mLarge );
			waitKey(100);	
			//cout<<sizeof m<<endl;
		}


	}
	return 0;
}

