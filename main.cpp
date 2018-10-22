#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/tracking/tracker.hpp"


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
vector<string> trackerTypes = { "BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN", "MOSSE", "CSRT" };


void displayOpenCVVersion() {
	std::cout << "OpenCV Version: " << std::endl << 
		"v " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << std::endl;
}


Ptr<Tracker> createTrackerByName(string trackerType) 
{
    Ptr<Tracker> tracker;
    
    if(trackerType == trackerTypes[0])
        tracker = TrackerBoosting::create();
    else if (trackerType == trackerTypes[1])
        tracker = TrackerMIL::create();
    else if (trackerType == trackerTypes[2])
        tracker = TrackerKCF::create();
    else if (trackerType == trackerTypes[3])
        tracker = TrackerTLD::create();
    else if (trackerType == trackerTypes[4])
        tracker = TrackerMedianFlow::create();
    else if (trackerType == trackerTypes[5])
        tracker = TrackerGOTURN::create();
    else if (trackerType == trackerTypes[6])
        tracker = TrackerMOSSE::create();
    else if (trackerType == trackerTypes[7])
        tracker = TrackerCSRT::create();
    else {
        cout << "Incorrect tracker name" << endl;
        cout << "Available trackers are: " << endl;
        for (vector<string>::iterator it = trackerTypes.begin() ; it != trackerTypes.end(); ++it) {
            std::cout << " " << *it << endl;
        }
    }
    return tracker;
}


void getRandomColors(vector<Scalar>& colors, int numColors)
{
  RNG rng(0);
  for(int i=0; i < numColors; i++)
    colors.push_back(Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255))); 
}


int main(int argc, char** argv) 
{
    font.font_face = CV_FONT_HERSHEY_COMPLEX;
    displayOpenCVVersion();

    cout << "Camera index?" << endl;
    int cIndex = 0;
    cin >> cIndex;
    
    VideoCapture cap(cIndex);
    
    vector<Rect> bboxes;
    Mat frame;
    
    if(!cap.isOpened())
    {
        cout << "Error opening open camera stream " << cIndex << endl;
        return -1;
    }

    // While loop probably starts here...
    cap >> frame;

    bool showCrossHair = true;
    bool fromCentre = false;
    cout << "\n==========================================================\n";
    cout << "OpenCV says press c to cancel objects selection process" << endl;
    cout << "It doesn't work. Press Escape to exit selection process" << endl;
    cout << "\n==========================================================\n";

    cv::selectROIs("MultiTracker", frame, bboxes, showCrossHair, fromCentre);

    if(bboxes.size() < 1)
        return 0;

    vector<Scalar> colors;
    getRandomColors(colors, bboxes.size());

    string trackerType = "CSRT";

    Ptr<MultiTracker> multiTracker = cv::MultiTracker::create();

    for(int i=0; i < bboxes.size(); i++)
        multiTracker->add(createTrackerByName(trackerType), frame, Rect2d(bboxes[i]));

    while(cap.isOpened())
    {
        cap >> frame;

        if(frame.empty()) break;

        multiTracker->update(frame);

        for(unsigned i = 0; i < multiTracker->getObjects().size(); i++) 
        {
            rectangle(frame, multiTracker->getObjects()[i], colors[i], 2, 1);
        }

        imshow("Multitracker", frame);

        if (waitKey(1) == 27) break;
    }






    return 0;
}