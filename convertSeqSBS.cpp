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

const string DIR_IMG = "/home/william/project/software/vr_kinetic/converter/sideBside/";
const string IMG_PREF = "SBS_";
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
	unsigned char bits[5625] = {0};
	unsigned char bits2[5625] = {0};
	unsigned char bits3[5625] = {0};
	std::string fileName = DIR_BLOB;
	int xBlob=0;
	int yBlob=0;
	int fileNum=1;//data file to start at ***IMPORTANT TO SET CORRECTLY***
	string numPrefix="0";
	std::stringstream ss;
	//go through files in data folder one at a time
	for(fileNum;fileNum<101;fileNum++)
	{
		//string file="Blobs_TF"+ +".txt"
		fileName=DIR_BLOB;
		memset(bits, 0, sizeof bits);//zero out the data
		memset(bits2, 0, sizeof bits);//zero out the data
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
		float a;
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
			if(notate==true)
			{
				cout << setprecision(5)<< "minZ:"  << minZ << "  maxZ:" << maxZ <<endl;
			}
			//extracts and prints each data point scaling each one.  
			for (int k = 0 ;k<sizeof(bits);k++)
			{
				dataPoints>>a;
				if(a*a*90>=255)
				{
					bits2[k]=255;
				}
				else
				{
					bits2[k]=a*a*90.0;
				}
			}
			Mat m2( xLength,yLength, CV_8UC1, bits2 );
			Mat m3( xLength,yLength, CV_8UC3, bits2 );
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
				bits[xBlob+yBlob*xLength]=255;//xBlob+(xLength*yLength-yBlob*xLength)
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
			Mat m1( xLength,yLength, CV_8UC1, bits );
			Mat m(yLength,xLength+xLength,CV_8UC1);
			Mat left(m,Rect(0,0,xLength,yLength));
			m1.copyTo(left);
			Mat right(m,Rect(xLength,0,xLength,yLength));
			m2.copyTo(right);
			//line(m, Point(0,12), Point(70,12), Scalar(100,100,0), thickness, lineType);
			//imwrite(fileName,m);
			namedWindow("Display Image", WINDOW_NORMAL);
			imshow("Display Image", m );
			waitKey(60);	
			//cout<<sizeof m<<endl;
		}


	}
	return 0;
}
