
#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include<string.h>
#include<conio.h>

using namespace cv;
using namespace std;

static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}

int main(int argc, const char *argv[]) {
    // Check for valid command line arguments, print usage
    // if no arguments were given.
  /*  if (argc != 4) {
        cout << "usage: " << argv[0] << " </path/to/haar_cascade> </path/to/csv.ext> </path/to/device id>" << endl;
        cout << "\t </path/to/haar_cascade> -- Path to the Haar Cascade for face detection." << endl;
        cout << "\t </path/to/csv.ext> -- Path to the CSV file with the face database." << endl;
        cout << "\t <device id> -- The webcam device id to grab frames from." << endl;
        exit(1);
    }*/
    // Get the path to your CSV:
    string fn_haar = "C:\\Users\\Vardaan\\Documents\\Visual Studio 2010\\Projects\\Face_Recognizer\\haarcascade_frontalface_alt.xml"; //string(argv[2]);
    string fn_csv = "C:\\Users\\Vardaan\\Documents\\Visual Studio 2010\\Projects\\Face_Recognizer\\csv.txt"; //string(argv[2]);
    int deviceId =  0;//atoi(argv[3]);
    // These vectors hold the images and corresponding labels:
    vector<Mat> images;
    vector<int> labels;
	FILE *fp=fopen("names.txt","r");
	if(!fp)
	{
		cout<<"Could not open names file"<<endl;
	}
    // Read in the data (fails if no valid input filename is given, but you'll get an error message):
    try {
        read_csv(fn_csv, images, labels);
    } catch (cv::Exception& e) {
        cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
		_getch();
        // nothing more we can do
        exit(1);
    }
    // Get the height from the first image. We'll need this
    // later in code to reshape the images to their original
    // size AND we need to reshape incoming faces to this size:
    int im_width = images[0].cols;
    int im_height = images[0].rows;
    // Create a FaceRecognizer and train it on the given images:
    Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
    model->train(images, labels);
    // That's it for learning the Face Recognition model. You now
    // need to create the classifier for the task of Face Detection.
    // We are going to use the haar cascade you have specified in the
    // command line arguments:
    //
    CascadeClassifier haar_cascade;
    haar_cascade.load(fn_haar);
    // Get a handle to the Video device:
    VideoCapture cap(deviceId);
	//cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	//cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	
    // Check if we can use this device at all:
    if(!cap.isOpened()) {
		
        cerr << "Capture Device ID " << deviceId << " cannot be opened." << endl;
		_getch();
        return -1;
    }
    // Holds the current frame from the Video device:
    Mat frame;
    for(;;) {
		cap >> frame;

		while (frame.empty()) {
			cap >> frame;
			            		}
      //  cap >> frame;
		 if (frame.empty())
    {
        cout << "Failed to grab frame (Try to move object away from camera)" << endl;
		_getch();
        break;
		 }
        // Clone the current frame:
        Mat original = frame.clone();
        // Convert the current frame to grayscale:
        Mat gray;
        cvtColor(original, gray, CV_BGR2GRAY);
        // Find the faces in the frame:
        vector< Rect_<int> > faces;
        haar_cascade.detectMultiScale(gray, faces);
        // At this point you have the position of the faces in
        // faces. Now we'll get the faces, make a prediction and
        // annotate it in the video. Cool or what?
		
        for(int i = 0; i < faces.size(); i++) {
            // Process face by face:
            Rect face_i = faces[i];
            // Crop the face from the image. So simple with OpenCV C++:
            Mat face = gray(face_i);
            // Resizing the face is necessary for Eigenfaces and Fisherfaces. You can easily
            // verify this, by reading through the face recognition tutorial coming with OpenCV.
            // Resizing IS NOT NEEDED for Local Binary Patterns Histograms, so preparing the
            // input data really depends on the algorithm used.
            //
            // I strongly encourage you to play around with the algorithms. See which work best
            // in your scenario, LBPH should always be a contender for robust face recognition.
            //
            // Since I am showing the Fisherfaces algorithm here, I also show how to resize the
            // face you have just found:
            Mat face_resized;
            cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
            // Now perform the prediction, see how easy that is:
            int prediction = model->predict(face_resized);
            // And finally write all we've found out to the original image!
            // First of all draw a green rectangle around the detected face:
            rectangle(original, face_i, CV_RGB(0, 255,0), 1);
            // Create the text we will annotate the box with:
			string identity;
			fseek(fp,0,SEEK_SET);
			char num=getc(fp);
			if(num!=EOF)
			{//	cout<<"inside while"<<endl;
				while(num-'0' !=prediction && num!=EOF)
				{//	cout<<"trying to match label"<< num<<endl;
					num=getc(fp);
				}
				//cout<<"found matching label"<<endl;
				num=getc(fp);
				num=getc(fp);

				 while ( num != ' ' && num!='\n')
				{	
					//cout<<"Storing name"<<endl;
					//cout<<"got "<<num<<endl;
					
					identity = identity + num;
					num = getc(fp);
					
				}
			}
			fseek(fp,0,SEEK_SET);
			//	cout<<"Got final name "<<identity<<endl;
				
		/*	switch(prediction)
			{
			case 0: identity="Unknown";
					break;
			case 1: identity="Vardaan Daddy";
					break;
			case 2: identity="Sindhura";
					break;
			}; */

		//	cout<<identity;

            string box_text = format("Prediction = %s", identity);
            // Calculate the position for annotated text (make sure we don't
            // put illegal values in there):
            int pos_x = std::max(face_i.tl().x - 10, 0);
            int pos_y = std::max(face_i.tl().y - 10, 0);
            // And now put it into the image:
            putText(original, identity, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 2.0, CV_RGB(0,255,0), 2.0);
        }
        // Show the result:
        imshow("face_recognizer", original);
        // And display it:
        char key = (char) waitKey(20);
        // Exit this loop on escape:
        if(key == 27)
            break;
    }
/*    return 0;
}
int main(int argc, const char** argv)
{*/
CascadeClassifier face_cascade;
//body_cascade.load("haarcascade_mcs_upperbody.xml");
printf("%d",face_cascade.load("haarcascade_frontalface_alt.xml"));
VideoCapture captureDevice;
captureDevice.open(0);

Mat captureFrame;
Mat grayscaleFrame;

namedWindow("outputCapture", 1);

//create a loop to capture and find faces
while(true)
{
    //capture a new image frame
    captureDevice>>captureFrame;

    //convert captured image to gray scale and equalize
    cvtColor(captureFrame, grayscaleFrame, CV_BGR2GRAY);
    equalizeHist(grayscaleFrame, grayscaleFrame);

    //create a vector array to store the face found
    std::vector<Rect> faces;

    //find faces and store them in the vector array
    face_cascade.detectMultiScale(grayscaleFrame, faces, 1.1, 3,                                                                    
    CV_HAAR_SCALE_IMAGE, Size(30,30));    
    //draw a rectangle for all found faces in the vector array on the original image
    for(int i = 0; i < faces.size(); i++)
    {
        Point pt1(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
        Point pt2(faces[i].x, faces[i].y);

        rectangle(captureFrame, pt1, pt2, cvScalar(0, 255, 0, 0), 1, 8, 0);
    }

    //print the output
    imshow("outputCapture", captureFrame);

    //pause for 33ms
    waitKey(33);
}

return 0;
}