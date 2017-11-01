/**
* This file should contain calls to the main functions in each phase only
* The functions specific to each phase must be defined in its own file
*/

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "GetMyHand/handDetection.hpp"

#include <iostream>
#include <ctime>
#include <experimental/filesystem>

#define OVERALL 0

using namespace std;
using namespace cv;
namespace fs = std::experimental::filesystem;

void processFrame(Mat& image);

string subDirName;

string tempTimesLabels[] = {"Overall"};

vector<string> timesLabels(tempTimesLabels, tempTimesLabels + sizeof(tempTimesLabels)/sizeof(string));
vector<double> maxTimes(timesLabels.size(),0);
vector<double> minTimes(timesLabels.size(),10000);
vector<double> avgTimes(timesLabels.size(),0);
double noOfFramesCollected = 0;


int main(int argc, char** argv){

	
	if(argc==3 && strcmp(argv[1],"-img")==0){	
		subDirName = string(argv[2]);
		subDirName = "./CCDC-Data/"+subDirName.substr(0,subDirName.find_last_of("/"));
		fs::create_directories(subDirName);
				
		
		Mat image = imread(argv[2],1);
		
		double startTime=(double)getTickCount();
		
		processFrame(image);		
	
		double timeTaken=(getTickCount()-(double)startTime)/getTickFrequency();
		maxTimes[OVERALL]=timeTaken>maxTimes[OVERALL]?timeTaken:maxTimes[OVERALL];
		minTimes[OVERALL]=timeTaken<minTimes[OVERALL]?timeTaken:minTimes[OVERALL];
		
		waitKey(0);
	}
	else if(argc==3 && strcmp(argv[1],"-AllImgs")==0){
		string subDirName1 = string(argv[2]);
		subDirName = "./CCDC-Data/"+subDirName1.substr(0,subDirName1.find_last_of("/"));
		fs::create_directories(subDirName);
		fs::remove(subDirName+"/data.csv");
		
		vector<string> files;
		for(auto &tempp1:fs::directory_iterator(subDirName1)){
			files.push_back(tempp1.path().string());
		}

		sort(files.begin(),files.end());

		for(int i=0;i<files.size();i++){
			cout<<"Processing "<<files[i]<<endl;
			Mat image = imread(files[i],1);
			
			double startTime=(double)getTickCount();
			
			processFrame(image);		
		
			double timeTaken=(getTickCount()-(double)startTime)/getTickFrequency();
			maxTimes[OVERALL]=timeTaken>maxTimes[OVERALL]?timeTaken:maxTimes[OVERALL];
			minTimes[OVERALL]=timeTaken<minTimes[OVERALL]?timeTaken:minTimes[OVERALL];
		}
	}
	else{
	
		string trainingImagesFolderPath;
		int imgNo=1;
		if(argc==3 && strcmp(argv[1],"-cap")==0){
			/*cout<<"Enter name of subdirectory for storing the training images: "<<endl;*/
			subDirName = string(argv[2]);
			//cin>>subDirName;
			trainingImagesFolderPath="./training-images/"+subDirName;
			fs::create_directories(trainingImagesFolderPath);
			fs::create_directories("./CCDC-Data/"+subDirName);
			for(auto &tempp1:fs::directory_iterator(trainingImagesFolderPath)){
				imgNo++;
			}
			//mkdir("./training-images/"+subDirName);
		}
		
		VideoCapture cap(0);
	
		if(!cap.isOpened()){
			return -1;
		}	
		
	
		while(true){
			Mat image;
			cap>>image;
		
			double startTime=(double)getTickCount();
			
			if(!image.data) continue;
		
			processFrame(image);			
			

			noOfFramesCollected++;
			double timeTaken=(getTickCount()-(double)startTime)/getTickFrequency();
			maxTimes[OVERALL] = timeTaken > maxTimes[OVERALL] ? timeTaken : maxTimes[OVERALL];
			minTimes[OVERALL] = timeTaken < minTimes[OVERALL] ? timeTaken : minTimes[OVERALL];
			avgTimes[OVERALL] = avgTimes[OVERALL] * ((noOfFramesCollected-1)/noOfFramesCollected) + timeTaken/noOfFramesCollected;

			
			if(waitKey(20)=='q') break;
			if(argc==3 && waitKey(30)=='c'){
				imwrite(trainingImagesFolderPath+"/"+to_string(imgNo)+".png",image);
				imgNo++;
			}

		}
	
		
	
	}
	
	cout<<"Times for "<<noOfFramesCollected<<" frames:"<<endl;
	for(int i=0;i<timesLabels.size();i++){
		cout<<timesLabels[i]<<":"<<endl;
		cout<<"     Min Time: "<<minTimes[i]<<"s"<<endl;
		cout<<"     Avg Time: "<<avgTimes[i]<<"s"<<endl;
		cout<<"     Max Time: "<<maxTimes[i]<<"s"<<endl;
	}
		
	
	return 0;
}

void processFrame(Mat& image){
	/* All processing functions go after this point */
		
	Mat contours = getMyHand(image);  //Defined in GetMyContours/getMyContours.cpp

	/* All processing functions come before this point */
}
