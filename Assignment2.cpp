//Tousar Mohammed 1001241976
//Computer Vision Summer 2018



#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
// configuration parameters
#define NUM_COMNMAND_LINE_ARGUMENTS 1

using namespace cv;
using namespace std;
Vec3b color;
float getCircularityThresh(vector<Point> cntr);

int dime=0;
int penny=0;
int quarter=0;
int nickel=0;

int main(int argc, char **argv)
{
    cv::Mat imageIn;
    
    // validate and parse the command line arguments
    if(argc != NUM_COMNMAND_LINE_ARGUMENTS + 1)
    {
        std::printf("USAGE: %s <image_path> \n", argv[0]);
        return 0;
    }
    else
    {
        imageIn = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
        
        // check for file error
        if(!imageIn.data)
        {
            std::cout << "Error while opening file " << argv[1] << std::endl;
            return 0;
        }
    }
    
    // get the image size
   // std::cout << "image width: " << imageIn.size().width << std::endl;
   // std::cout << "image height: " << imageIn.size().height << std::endl;
   

    //resize the image to 20% of its size
    cv::resize(imageIn,imageIn,cv::Size(),.20,.20);
    
   
  
    // convert the image to grayscale
    cv::Mat imageGray;
    cv::cvtColor(imageIn, imageGray, cv::COLOR_BGR2GRAY);
    
    // find the image edges
    cv::Mat imageEdges;
    //blurr the image first
    GaussianBlur(imageGray, imageGray, Size(3, 3), 3, 3);
    

    //Calculate the Otsu threshold values
    Mat opimg = Mat(imageGray.rows, imageGray.cols, CV_8UC1);
    double otsu = threshold(
                                       imageGray, opimg, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU
                                       );
    
    //imshow("otsu", opimg);
    
    double high = otsu,
    low = otsu * 0.5;
    
    Canny(imageGray, imageEdges, low, high, 3);
    
    
    //Morphology
    //Dilation
    int dilation_type = MORPH_RECT, dilation_size = 1;
    Mat element = getStructuringElement(dilation_type,
                                        Size(2 * dilation_size + 1, 2 * dilation_size + 1),
                                        Point(dilation_size, dilation_size));
    
    morphologyEx(imageEdges, imageEdges, MORPH_DILATE, element);
    
    //Canny to thin the edges after dilation
    Canny(imageEdges, imageEdges, 0, 400, 5);
    
   // namedWindow("Canny", 0);
    //imshow("Canny", imageEdges);
    
    

    vector<vector<Point> > contours, contoursfil;
    vector<Vec4i> hierarchy;

    Mat contourImg = Mat::ones(imageEdges.rows, imageEdges.cols, imageEdges.type());
    
    //Find all contours in the edges image
    findContours(imageEdges.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
    
    float circThresh;
    
    for (int j = 0; j < contours.size(); j++)
    {
   
        if ((hierarchy[j][2] >= 0) && (hierarchy[j][3] < 0))
        {
            circThresh = getCircularityThresh(contours[j]);
         
            if ((circThresh > 10) && (circThresh <= 30))
            {
                contoursfil.push_back(contours[j]);
            }
        }
    }
    
    for (int j = 0; j < contoursfil.size(); j++)
    {
        drawContours(contourImg, contoursfil, j, CV_RGB(255, 255, 255), 1, 8);
    }
  // namedWindow("Contour Image Filtered", 0);
  //  imshow("Contour Image Filtered", contourImg2);
    
    
    
  //find circle
    vector<Vec3f> circles;
    HoughCircles(contourImg, circles, CV_HOUGH_GRADIENT, 2, imageGray.rows / 8, 200, 30,45,90);

    float change = 0;
  
    //draw circle based on size and color them accordingly and count each coin and total
    for (size_t i = 0; i < circles.size(); i++)
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        float radius = circles[i][2];
 
       // cout<<endl<<"radius"<<radius<<endl;
    
        
        if (radius >= 85)
        {
             circle( imageIn, center, radius, Scalar(0,255,0), 2, 8, 0 );

            change = change + .25;
            quarter++;
        }
        else if ((radius >= 75) && (radius<85))
        {
            circle( imageIn, center, radius, Scalar(0,255,255), 2, 8, 0 );
            
            change = change + .05;
            nickel++;
        }
        else if ((radius >= 72) && (radius<75))
        {
            circle( imageIn, center, radius, Scalar(0,0,255), 2, 8, 0 );
          
            change = change + .01;
            penny++;
        }
        else if ((radius >= 65) && (radius<71))
        {
            circle( imageIn, center, radius, Scalar(255,0,0), 2, 8, 0 );
         
            change = change + .1;
            dime++;
        }
        
    }
    
    cout<<"Penny - "<<penny<<endl;
     cout<<"Nickel - "<<nickel<<endl;
     cout<<"Dime - "<<dime<<endl;
     cout<<"Quarter - "<<quarter<<endl;
     cout<<"Total -$"<<change<<endl;
    imshow("HoughCircle",imageIn);
    
    
    cv::waitKey();
    return 0;
}


float getCircularityThresh(vector<Point> cntr)
{
    float perm, area;
    
    perm = arcLength(Mat(cntr), true);
    area = contourArea(Mat(cntr));
    
    return ((perm*perm) / area);
    
}
