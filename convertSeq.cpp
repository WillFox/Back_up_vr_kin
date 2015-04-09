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
//#include "cvplot.h"
using namespace cv;
using namespace std;

const string DIR_IMG = "/home/william/project/software/vr_kinetic/converter/blob_cv/";
const string DIR_BLOB = "/home/william/project/software/vr_kinetic/converter/blobs/";
const string DIR_DATA = "/home/william/project/software/vr_kinetic/converter/region/";
int main(int argc, char** argv )
{
	string line;
	bool notate = true;//prints information related to program progress
	int xLength=75;	//The length of the image
	int yLength=75;	//The height of the image
	unsigned char bits[5625] = {0};
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
		//add the file number to differentiate (create separate function later)
		if(fileNum<10)
		{
			numPrefix="Blobs_TF0000";
		}
		else if(fileNum<100)
		{
			numPrefix="Blobs_TF000";
		}
		else if(fileNum<1000)
		{
			numPrefix="Blobs_TF00";
		}
		else if(fileNum<10000)
		{
			numPrefix="Blobs_TF0";
		}
		else
		{
			numPrefix="Blobs_TF";;
		}
		fileName+=numPrefix;
		ss.str("");
		ss<<fileNum;
		std::string str = ss.str();
		fileName+=str;
		fileName+=".txt";
		ifstream interestPoints (fileName.c_str());
		if (interestPoints.is_open())
		{
			if(notate==true)
			{
				cout << "File successfully opened:"<<fileNum<<endl;	
			}
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
					interestPoints.close();
				}
				bits[xBlob+(xLength*yLength-yBlob*xLength)]=1000;
			}
			if(notate==true)
			{
				cout << "Building Axis" << endl;
			}
			int thickness = 2;
			int lineType = 8;
			//start=Point(0,12);
			//end=Point(70,12);
			fileName=DIR_IMG;
			fileName+=numPrefix;
			fileName+=str;
			fileName+=".jpg";
			if(notate==true)
			{
				cout << fileName << endl;
			}
			Mat m( xLength,yLength, CV_8UC1, bits );
			//line(m, Point(0,12), Point(70,12), Scalar(100,100,0), thickness, lineType);
			//imwrite(fileName,m);
			namedWindow("Display Image", WINDOW_NORMAL);
			imshow("Display Image", m );

			waitKey(125);	
			//cout<<sizeof m<<endl;
		}


	}
	return 0;
}
