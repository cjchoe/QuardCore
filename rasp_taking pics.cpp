/* written by CJ Choe*/
/* 2017.05.16*/

#include <ctime>
#include <cv.h>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <raspicam/raspicam_cv.h>
using namespace std;
using namespace cv;

int main ( int argc,char **argv ) {


  Mat img_input, img_gray, img_result;
  Mat img_input2, img_gray2, img_result2;

int j=0;
while(j<2){
    time_t timer_begin,timer_end;
    raspicam::RaspiCam_Cv Camera;
    cv::Mat image;
    int nCount=100;
    //카메라 set
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC1 );
    //카메라 open
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    //영상 취득 시작
    cout<<"Capturing "<<nCount<<" frames ...."<<endl;
    time ( &timer_begin );
    for ( int i=0; i<nCount; i++ ) {
        Camera.grab();
        Camera.retrieve ( image);
        if ( i%5==0 )  cout<<"\r captured "<<i<<" images"<<std::flush;
    }
    cout<<"Stop camera..."<<endl;
    Camera.release();
    //timer 가동
    time ( &timer_end ); /* get current time; same as: timer = time(NULL)  */
    double secondsElapsed = difftime ( timer_end,timer_begin );
    cout<< secondsElapsed<<" seconds for "<< nCount<<"  frames : FPS = "<<  ( float ) ( ( float ) ( nCount ) /secondsElapsed ) <<endl;

    if(j==0)
    cv::imwrite("test1.jpg",image);
    else if (j==1) {
      cv::imwrite("test2.jpg",image);
    }
    j++;
  }



}
