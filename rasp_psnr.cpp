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




  //화면에 결과 이미지를 보여준다.
  //imshow("입력 영상", img_input);
  //imshow("입력 그레이스케일 영상", img_gray);
  imshow("결과 영상", img_result);
  imshow("결과 영상2", img_result2);

  //아무키를 누르기 전까지 대기
  while (cvWaitKey(0) == 0);

  //결과를 파일로 저장
  //imwrite("img_gray.jpg", img_gray);
  imwrite("img_result.jpg", img_result);
  imwrite("img_result2.jpg", img_result2);


  /* 획득한 두 장의 정지영상의 MSE값 검출 후 PSNR값 계산*/


  IplImage *src1= cvLoadImage("img_result.jpg");
  IplImage *src2= cvLoadImage("img_result2.jpg");

  long long int sigma = 0;
  long long int squre = 0;
  double MSE = 0.0;
  double SNR = 0.0;
  double PSNR = 0.0;
  int frameSize = src1->height*src1->width*3;
  int blue1=0, blue2=0;
  int green1=0, green2=0;
  int red1=0, red2=0;

  // width x height -> [height][width]
  for(int i=0;i<src1->height;i++){
      for(int j=0;j<src1->widthStep;j=j+3){
          blue1=(int)(uchar)src1->imageData[i*src1->widthStep+j];//Blue
          green1=(int)(uchar)src1->imageData[i*src1->widthStep+j+1];//Green
          red1=(int)(uchar)src1->imageData[i*src1->widthStep+j+2];//Red
          blue2=(int)(uchar)src2->imageData[i*src2->widthStep+j];//Blue
          green2=(int)(uchar)src2->imageData[i*src2->widthStep+j+1];//Green
          red2=(int)(uchar)src2->imageData[i*src2->widthStep+j+2];//Red
          sigma+=(blue1-blue2)*(blue1-blue2)+
          (green1-green2)*(green1-green2)+
          (red1-red2)*(red1-red2);
          squre += blue1*blue1 + green1*green1 + red1*red1;
      }
  }
  MSE=sigma/(double)frameSize;
  PSNR=10*log10(255*255/MSE);
  SNR = 10*log10(squre/sigma);

  //cout<<"sigma: "<<sigma<<endl;;
  cout<<"MSE: "<<MSE<<endl;;
  cout<<"PSNR: "<<PSNR<<endl;;
  //cout<<"SNR: "<<SNR<<endl;;

  if(PSNR>=22)
    cout<<"Car is not moving"<<endl;
  else
    cout<<"Car is moving!"<<endl;

  //system("pause");
  cvWaitKey(0);
  return EXIT_SUCCESS;




}
