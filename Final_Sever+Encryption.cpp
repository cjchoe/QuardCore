/* CABStone Project */
/* QuardCore */
/* 2017. 06.19 */
/* wrriten by cjchoe */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#define DATASIZE 20 // UID, Max주소 20자리 이하.

/* 함수 선언부 */
static int ran = 10+rand()%1000; //10~999사이의 암호화 난수 생성.

void error( char *msg ) {
  perror(  msg );
  exit(1);
}

int func( int a ) {
   return 2 * a;
}

/* 현재 sendData함수를 통해 클라이언트(라즈베리파이)로 구매자의 UID및 Mac주소를 송신해준다.
여기서는 MAC주소를 암호화해서 전송해주는데, 암호화 하는 과정을 따로 함수로 구현 않고 이 함수 안에서 실행해준다.*/

void sendData( int sockfd, int x ) {
  int n;
  char buffer[32]; // 데이터의 저장
  char mac[]="a1:b2:c3:d4:e5"; // 구매자의 맥 주소
  int tmpInt[DATASIZE]; // 암호화 변수1
  char tmpChar[DATASIZE]; // 암호화 변수2
  int i;

  for(i=0;i<DATASIZE;i++){
    tmpInt[i] = 0;
  } // 초기화

  /* 암호화 과정*/
  /* 최초에 서버로 부터 받은 사용자의 주소의 스트링 값을 아스키 값으로 표현 후 난수를 더해준다. 그 후 다시 캐릭터 형으로 바꿔주는 방식으로 암호화 진행 */
  /* 최종적으로 서버에서는 암호화된 스트링과 난수를 비콘스캐너에 전송해준다.*/
  for(i=0;mac[i] != '\0';i++){
        tmpInt[i] = (int)mac[i];
        tmpInt[i] += ran;
        buffer[i] = (char)tmpInt[i];
    }
    buffer[i+1] = '\0';

  sprintf( buffer, "%d\n", x );
  if ( (n = write( sockfd, buffer, strlen(buffer) ) ) < 0 )
    error( const_cast<char *>( "ERROR writing to socket") );
  buffer[n] = '\0';
}
//데이터 수신함수
int getData( int sockfd ) {
  char buffer[32];
  int n;

  if ( (n = read(sockfd,buffer,31) ) < 0 ) //receive from read로 불러오기.
    error( const_cast<char *>( "ERROR reading from socket") );
  buffer[n] = '\0';
  return atoi( buffer );
}

int main(int argc, char *argv[]) {
     int sockfd, newsockfd, portno = 51717, clilen; // 포트 넘버 51717
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     int data;
     int count;
     printf( "using port #%d\n", portno );

         /* 소켓 열기 */
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
         error( const_cast<char *>("ERROR opening socket") );
     bzero((char *) &serv_addr, sizeof(serv_addr));

     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons( portno );
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) //바인딩 에러 시
       error( const_cast<char *>( "ERROR on binding" ) );
     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     //--- 계속해서 클라이언트의 접속을 기다린다. 무한루프 ---
     while ( 1 ) {
        printf( "waiting for new client...\n" );
        if ( ( newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen) ) < 0 )
            error( const_cast<char *>("ERROR on accept") );
        printf( "opened new communication with client\n" );
        sendData( newsockfd, ran); // 암호화된 사용자의 특정 아이디와 암호화난수 전송.
        /* 클라이언트와 접속이 시작됐을 때, 라즈베리파이의 감지 결과를 실시간으로 계쏙 받아낸다.*/
        while ( 1 ) {
             //---- wait for a number from client ---
             data = getData( newsockfd );
             //data1 PSNR 값이 기준치 이하 일 때, 비콘인증 되지 않은 상황에서 차가 움직이는 경우.
             if(data == 1){
               count++;
               printf("////////Warning////////\n");
               printf("////////Warning////////\n");
               printf("Car is moving Alert!!!!\n");
               printf("////////Warning////////\n");
               printf("////////Warning////////\n");
               printf("How many undefined: %d\n\n\n",count); // 몇 회 이상 인증되지 않았는지 표시 기준치 이상 시 페널티 부여.
             }
             else if(data==2){
               printf("Cars is not moving Safe\n");
             } //PSNR 값 기준치 이상 비콘 인증은 되지 않았지만, 차량 현재 움직임 없음.

            // if ( data < 0 )
            //    break;
             data = func( data );


             //printf( "sending back %d\n", data );

        }
        //close( newsockfd );

        // data -2 받을 시 연결종료.
      //  if ( data == -2 )
        //  break;
     }
     return 0;
}
