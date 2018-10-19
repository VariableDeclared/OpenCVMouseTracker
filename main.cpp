#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;

CvFont font = cvFont(2);

/* ENVS TO RUN THE PROGRAM:
*   
* MOCAP_IMAGE_DIR: If defined program will save image to directory, over-writing the last capture, when motion is detected.
* MOCAP_DEBUG_MODE: Mode for outputting console output (affects performance). 
*       Modes to define: INFO, PROD, DEBUG, VERBOSE
*       INFO - 1
*       VERBOSE - 4
*/
const char * env_p = std::getenv("MOCAP_IMAGE_DIR");
String DEBUG = String(std::getenv("MOCAP_DEBUG_MODE"));

void displayOpenCVVersion() {
	std::cout << "OpenCV Version: " << std::endl << 
		"v " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << std::endl;
}

int main(int argc, char** argv) 
{
    font.font_face = CV_FONT_HERSHEY_COMPLEX;
    displayOpenCVVersion();

    cout << "Camera index?" << endl;
    int cIndex = 0;
    cin >> cIndex;
    
    VideoCapture cap(cIndex);

    Mat image1, image2;
    Mat grayscale1, grayscale2;
    Mat absOut, thresholdImg;

    if (!cap.isOpened())
    {
        cout << "Cannot open web cam" << endl;
        return -1;
    }

    namedWindow("original", 1);


    while(true) 
    {
        
        if(!cap.read(image1))
        {
            cout << "Cannot read a frame from the video stream" << endl;
            break;
        }
        

        if (!cap.read(image2)) 
        {
            cout << "Cannot read a frame from the video stream" << endl;
            break;
        }


        cvtColor(image1, grayscale1, COLOR_BGR2GRAY);
        cvtColor(image2, grayscale2, COLOR_BGR2GRAY);

        absdiff(grayscale1, grayscale2, absOut);
        
        threshold(absOut, thresholdImg, 30, 255, THRESH_BINARY);

        blur(thresholdImg, thresholdImg, Size(200, 200));
        
        
        
        erode(thresholdImg, thresholdImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        dilate(thresholdImg, thresholdImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

        Moments mome = moments(thresholdImg, true);
        double dM01 = mome.m01;
        double dM10 = mome.m10;
        double dArea = mome.m00;

        int posX = dM10 / dArea;
        int posY = dM01 / dArea;

        if (DEBUG == "4")
            cout << posX << " " << posY << endl;
        if(posX >= 0 && posY >= 0 && (posX != posY))
        {
            putText(image1, "Movement detected", Point(0,40), CV_FONT_NORMAL, 1, Scalar(0, 0, 255));
            line(image1, Point(posX + 30, posY), Point(posX - 30, posY), Scalar(0, 0, 255), 2);
            line(image1, Point(posX, posY + 30), Point(posX, posY - 30), Scalar(0, 0, 255), 2);
            if(env_p) 
            {
                String imageDirectory = String(env_p) + "/capture.png";
                cout << "Saving capture to: " << imageDirectory << endl;
                imwrite(imageDirectory, image1);
            }
        }


        imshow("original", image1);

        if (waitKey(30) == 27)
        {
            cout << "esc key pressed" << endl;
            break;
        }

        

    }

    return 0;
}