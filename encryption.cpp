#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main()
{
    int tmpInt[16];
    char tmpChar[16];
    int i;
    int ran = 10+rand()%1000; //10~999사이의 난수 생성.
    char mac[] = "a1:b2:c3:d4:e5"; // 특정 어드레스 예시.

    /* 암호화 과정*/
    /* 최초에 서버로 부터 받은 사용자의 주소의 스트링 값을 아스키 값으로 표현 후 난수를 더해준다. 그 후 다시 캐릭터 형으로 바꿔주는 방식으로 암호화 진행 */
    /* 최종적으로 서버에서는 암호화된 스트링과 난수를 비콘스캐너에 전송해준다.*/
    for(i=0;mac[i] != '\0';i++){
    tmpInt[i] = (int)mac[i];
    tmpInt[i] += ran;
    tmpChar[i] = (char)tmpInt[i];
    }
    tmpChar[i+1] = '\0';

    printf("%s\n",tmpChar);


    /* 암호해독 과정으로 서버로 부터 받은 스트링 주소와 난수 값을 이용한다.*/
    /* 받은 스트링값을 아스키 코드로 변환 후 난수를 빼주고 다시 스트링 형으로 변환해준다.*/
    for(i=0;mac[i] != '\0';i++){
    tmpInt[i] = (int)tmpChar[i];
    tmpInt[i] -= ran;
    tmpChar[i] = (char)tmpInt[i];
    }
    tmpChar[i+1] = '\0';

    printf("%s\n",tmpChar);
}
