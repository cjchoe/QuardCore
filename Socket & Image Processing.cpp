/* written by CJ Choe*/
/* 2017.06. 05*/
/* Socket Programming & Opencv를 이용한 convolution Highpass filtering 및 MSE,PSNR값 유도 */

#include <ctime>
#include <cv.h>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <raspicam/raspicam_cv.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
using namespace std;
using namespace cv;


/* 소켓 클라이언트 생성 및 접속 */
void error(char *msg) {
    perror(msg);
    exit(0);
}
// 서버에 메시지 전송.
void sendData( int sockfd, int x ) {
  int n;

  char buffer[32];
  sprintf( buffer, "%d\n", x );
  if ( (n = write( sockfd, buffer, strlen(buffer) ) ) < 0 )
      error( const_cast<char *>( "ERROR writing to socket") );
  buffer[n] = '\0';
}

int getData( int sockfd ) {
  char buffer[32];
  int n;

  if ( (n = read(sockfd,buffer,31) ) < 0 )
       error( const_cast<char *>( "ERROR reading from socket") );
  buffer[n] = '\0';
  return atoi( buffer );
}

int main ( int argc,char **argv ) {


  Mat img_input, img_gray, img_result;
  Mat img_input2, img_gray2, img_result2;

  /* socket 클라이언트 구현 */

  int sockfd, portno = 51717, n;
  char serverIp[] = "192.168.137.44"; // 현재 라즈베리파이 고정 IP 설정값.
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char buffer[256];
  int data;

  if (argc < 3) {
    // error( const_cast<char *>( "usage myClient2 hostname port\n" ) );
    printf( "contacting %s on port %d\n", serverIp, portno );
    // exit(0);
  }
  if ( ( sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 )
      error( const_cast<char *>( "ERROR opening socket") );

  if ( ( server = gethostbyname( serverIp ) ) == NULL )
      error( const_cast<char *>("ERROR, no such host\n") );

  bzero( (char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);
  if ( connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
      error( const_cast<char *>( "ERROR connecting") );


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

          //그레이스케일링
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


  //라플라시안 마스크 여기서 PSNR 비교할 때 unsharp 마스크로 구현된 라플라시안 마스크를 사용하는게 좀 더 정확한 움직임 감지가 된다.
  int mask1[3][3] = { {-1,-1,-1},  //에지 검출 Laplacian Mask
                      { -1,8,-1 },
                      { -1,-1,-1 }};
/* Mask2로 콘볼루션 수행했을 때 PSNR값은 떨어지지만 움직임 관찰 더 용이하다!!!!!*/

  int mask2[3][3] = { { -1,-1,-1 }, //영상 선명하게 High-Boost Laplacian Mask
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
                  //sum += img_gray.at<uchar>(y + i, x + j) * mask1[masksize / 2 + i][masksize / 2 + j];
                  sum += img_gray.at<uchar>(y + i, x + j) * mask2[masksize / 2 + i][masksize / 2 + j];
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
                  //sum += img_gray2.at<uchar>(y + i, x + j) * mask1[masksize / 2 + i][masksize / 2 + j];
                  sum += img_gray2.at<uchar>(y + i, x + j) * mask2[masksize / 2 + i][masksize / 2 + j];
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


/* 실험 결과 PSNR값이 보통 22 이상이면 움직임이 없는 것으로 판단...*/

  if(PSNR>=22)
    cout<<"Car is not moving"<<endl;
  else
    cout<<"Car is moving!"<<endl;

  //system("pause");
  cvWaitKey(0);
  int moving = 1;
  int non_moving = 2;


  //움직임이 있을 때
  if(PSNR<22){
    sendData( sockfd, moving );
    data = getData( sockfd );
  }
  //움직임이 없을 때
  else{
    sendData( sockfd, non_moving );
    data = getData( sockfd );
  }

  close( sockfd );


  return EXIT_SUCCESS;




}
