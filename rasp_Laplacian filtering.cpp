#include <ctime>
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
    //set camera params
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC1 );
    //Open camera
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    //Start capture
    cout<<"Capturing "<<nCount<<" frames ...."<<endl;
    time ( &timer_begin );
    for ( int i=0; i<nCount; i++ ) {
        Camera.grab();
        Camera.retrieve ( image);
        if ( i%5==0 )  cout<<"\r captured "<<i<<" images"<<std::flush;
    }
    cout<<"Stop camera..."<<endl;
    Camera.release();
    //show time statistics
    time ( &timer_end ); /* get current time; same as: timer = time(NULL)  */
    double secondsElapsed = difftime ( timer_end,timer_begin );
    cout<< secondsElapsed<<" seconds for "<< nCount<<"  frames : FPS = "<<  ( float ) ( ( float ) ( nCount ) /secondsElapsed ) <<endl;
    //save image
    if(j==0)
    cv::imwrite("test1.jpg",image);
    else if (j==1) {
      cv::imwrite("test2.jpg",image);
    }
    j++;
  }

  /*캡쳐한 이미지 하이패스 필터링*/

  img_input = imread("test1.jpg", IMREAD_COLOR);
  img_input2 = imread("test2.jpg", IMREAD_COLOR);

  if (img_input.empty())
  {
      cout << "파일을 읽어올수 없습니다." << endl;
      exit(1);
  }

  //입력영상을 그레이스케일 영상으로 변환
  img_gray = Mat(img_input.rows, img_input.cols, CV_8UC1);
  img_gray2 = Mat(img_input2.rows, img_input2.cols, CV_8UC1);

  for (int y = 0; y < img_input.rows; y++)
  {
      for (int x = 0; x < img_input.cols; x++)
      {
          //img_input으로부터 현재 위치 (y,x) 픽셀의
          //blue, green, red 값을 읽어온다.
          uchar blue = img_input.at<Vec3b>(y, x)[0];
          uchar green = img_input.at<Vec3b>(y, x)[1];
          uchar red = img_input.at<Vec3b>(y, x)[2];

          //blue, green, red를 더한 후, 3으로 나누면 그레이스케일이 된다.
          uchar gray = (blue + green + red) / 3.0;

          //Mat타입 변수 img_gray에 저장한다.
          img_gray.at<uchar>(y, x) = gray;
      }
  }

  for (int y = 0; y < img_input2.rows; y++)
  {
      for (int x = 0; x < img_input2.cols; x++)
      {
          //img_input으로부터 현재 위치 (y,x) 픽셀의
          //blue, green, red 값을 읽어온다.
          uchar blue = img_input2.at<Vec3b>(y, x)[0];
          uchar green = img_input2.at<Vec3b>(y, x)[1];
          uchar red = img_input2.at<Vec3b>(y, x)[2];

          //blue, green, red를 더한 후, 3으로 나누면 그레이스케일이 된다.
          uchar gray = (blue + green + red) / 3.0;

          //Mat타입 변수 img_gray에 저장한다.
          img_gray2.at<uchar>(y, x) = gray;
      }
  }


  //라플라시안 마스크
  int mask1[3][3] = { {-1,-1,-1},  //에지 검출
                      { -1,8,-1 },
                      { -1,-1,-1 }};

  int mask2[3][3] = { { -1,-1,-1 }, //영상 선명하게
                      { -1,9,-1 },
                      { -1,-1,-1 } };


  long int sum;
  img_result = Mat(img_input.rows, img_input.cols, CV_8UC1);
  img_result2 = Mat(img_input2.rows, img_input2.cols, CV_8UC1);
  int masksize = 3;

  for (int y = 0 + masksize / 2; y < img_input.rows - masksize / 2; y++)
  {
      for (int x = 0 + masksize / 2; x < img_input.cols - masksize / 2; x++)
      {
          sum = 0;
          for (int i = -1 * masksize / 2; i <= masksize / 2; i++)
          {
              for (int j = -1 * masksize / 2; j <= masksize / 2; j++)
              {
                  sum += img_gray.at<uchar>(y + i, x + j) * mask1[masksize / 2 + i][masksize / 2 + j];
                  //sum += img_gray.at<uchar>(y + i, x + j) * mask2[masksize / 2 + i][masksize / 2 + j];
              }
          }

          //0~255 사이값으로 조정
          if (sum > 255) sum = 255;
          if (sum < 0) sum = 0;

          img_result.at<uchar>(y, x) = sum;
      }
  }

  for (int y = 0 + masksize / 2; y < img_input2.rows - masksize / 2; y++)
  {
      for (int x = 0 + masksize / 2; x < img_input2.cols - masksize / 2; x++)
      {
          sum = 0;
          for (int i = -1 * masksize / 2; i <= masksize / 2; i++)
          {
              for (int j = -1 * masksize / 2; j <= masksize / 2; j++)
              {
                  sum += img_gray.at<uchar>(y + i, x + j) * mask1[masksize / 2 + i][masksize / 2 + j];
                  //sum += img_gray2.at<uchar>(y + i, x + j) * mask2[masksize / 2 + i][masksize / 2 + j];
              }
          }

          //0~255 사이값으로 조정
          if (sum > 255) sum = 255;
          if (sum < 0) sum = 0;

          img_result2.at<uchar>(y, x) = sum;
      }
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




}
