#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define DEFAULT_PROTOCOL 0
#define MAXLINE 100


#define ENGINE 0
#define CONTROLLER 1
#define CLUSTER 2

// 3개 소켓 생성
int connfd[4];

// 엔진, 컨트롤러, 클러스터 인풋 데이터 저장소
char inmsg0[MAXLINE], inmsg1[MAXLINE], inmsg2[MAXLINE];

// 누적 주행거리 변수
char odo[1000000];

// 보내기 버퍼
char s_buffer[4][100][MAXLINE];
int s_cur_index[4];
int s_save_index[4];

// 데이터 보내기
void sendData(int toward,char* input) {
        printf("[sendData]recv string %s : send to %d\n",input,toward);
		
		// qt에서는 줄바꿈으로 문자 인식을 함으로 개행문자 선언
        if(toward==2 || toward==3) sprintf(s_buffer[toward][s_save_index[toward]],"%s\n",input);

		// 엔진이나 컨트롤러로 데이터 보낼 시 일반적으로 사용
        else sprintf(s_buffer[toward][s_save_index[toward]],"%s",input);
        s_save_index[toward]++;
        
		// 큐가 100개 임으로 100이면 0으로 초기화
		if(s_save_index[toward] >= 100) s_save_index[toward] = 0;
        printf("[sendData]send to queue complete.\n");
}

// 소켓 데이터 한 줄씩 읽기
int readLine(int fd, char* str) {
        int n;
        do {
                n = read(fd, str, 1);
        } while(n > 0 && *str++ != NULL);
        return n;
}

// 문자 자르기
void cutdata(char* input,char* out1,char* out2) {
        char* pos;
        char* pos2;
        char cutchar[MAXLINE];

		// 문자열을 복사한다 (strtok 특성)
        strcpy(cutchar,input);
//      printf ("cutdata : %s\n",input);
        pos = strtok(cutchar,"=");
        sprintf(out1, "%s", pos);
//      printf ("arg 1 : %s\n",out1);
        pos2 = strtok(NULL,"=");
        sprintf(out2, "%s", pos2);
//      printf ("arg 2 : %s\n",out2);
}

// 전송 쓰레드
void *sender() {
        printf("sender run\n");
        int len=0;
        while(1) {
                for(int i=0;i<4;i++){

						// 연결이 되지 않았다면 다시 for 문 진행
                        if(connfd[i]==NULL) continue;

						// 큐에 save 인덱스 값과 cur 값이 같아질 때 까지 전송
                        if(s_cur_index[i] != s_save_index[i]) {

							// qt 로 전송할 경우 +1 필요없음
                            if (i==2 || i==3) len=write(connfd[i],s_buffer[i][s_cur_index[i]],strlen(s_buffer[i][s_cur_index[i]]));
                            else len=write(connfd[i],s_buffer[i][s_cur_index[i]],strlen(s_buffer[i][s_cur_index[i]])+1);
                            printf("sender send : %s : to %d\n",s_buffer[i][s_cur_index[i]],i);
                            s_cur_index[i]++;
                            if(s_cur_index[i] >= 100) s_cur_index[i]=0;
                        }
                }
        }
}


// 0:엔진,1:컨트롤러,2:클러스터
void *engine() {
        printf("engine recv run.\n");

        char from[100],data[100];


        while(1) {
                if(connfd[0] == NULL) continue;
                memset(inmsg0, 0x00, sizeof(inmsg0));

                // 엔진에서 받은 데이터 처리
                if (readLine(connfd[0],inmsg0)>0) {

                        memset(from, 0x00, sizeof(from));
                        memset(data, 0x00, sizeof(data));

                        cutdata(inmsg0,from,data);
                        printf("engine recv : [%s] %s\n",from,data);
			// 엔진값 클러스터 모두 전송
                        sendData(2,inmsg0);

                }
        }
}

void *controller() {
        printf("contoller recv run.\n");

        char from[100],data[100];

        while(1) {
                if(connfd[1] == NULL) continue;
                memset(inmsg1, 0x00, sizeof(inmsg1));

                // 컨트롤러에서 받은 데이터 처리
                if (readLine(connfd[1],inmsg1)>0) {
						// 메모리 초기화
                        memset(from, 0x00, sizeof(from));
                        memset(data, 0x00, sizeof(data));
						
						// = 기준으로 데이터 짤라 from, data 로 저장
                        cutdata(inmsg1,from,data);
                        printf("[controller]recv : [%s] %s\n",from,data);
			
						// 컨트롤러의 값 전송
                        if(strcmp(from,"control")==0) {

								// 컨트롤러 값 엔진 모두 전송
                                sendData(0,data);

								// 컨트롤러 값 기어 정보 클러스터 전송
                                if(strcmp(data,"p")==0) sendData(2,inmsg1);
                                if(strcmp(data,"r")==0) sendData(2,inmsg1);
                                if(strcmp(data,"n")==0) sendData(2,inmsg1);
                                if(strcmp(data,"d")==0) sendData(2,inmsg1);
                        }
			
						// 핸들 값 전송
                        if(strcmp(from,"handle")==0) {

								// 핸들의 온오프 값 엔진 전송
                                if(strcmp(data,"onoff")==0) sendData(0,data);

								// 핸들의 모든 정보 클러스터 전송
                                sendData(2,inmsg1);
								sendData(3,inmsg1);
                        }
                }

                //sleep(1);
        }
}

// 클러스터 리시버 (딱히 받는 것은 없다)
void *cluster() {
        printf("cluster recv run.\n");
        while(1) {
                if(connfd[2] == NULL) continue;
                //printf("cluster recv connected.\n");
                memset(inmsg2, 0x00, sizeof(inmsg2));
                if (readLine(connfd[2],inmsg2)>0) {
                        printf("cluster recv : %s\n", inmsg2);
                        s_save_index[2]++;
                        if(s_save_index[2] >= 100) s_save_index[2] = 0;
                }

        }
}


// 메인 함수
int main (int argc, char* argv[])
{
        int listenfd, port, clientlen;
        struct sockaddr_in serveraddr, clientaddr;
        struct hostent *hp;
        char *haddrp;

        signal(SIGCHLD, SIG_IGN);
		// 포트 설정
        port = atoi("8000");
        listenfd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
        printf("[ecu] started...\n");

        // socket reuse addr
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
                error("setsockopt(SO_REUSEADDR) failed");

		// 바인드, 리슨
        bzero((char *) &serveraddr, sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serveraddr.sin_port = htons((unsigned short)port);
        bind(listenfd, &serveraddr, sizeof(serveraddr));
        listen(listenfd, 10);

		// 쓰레드 설정
        int thr_id[4];
        pthread_t thread[4];

		// 쓰레드 생성
        thr_id[0] = pthread_create(&thread[0],NULL,sender,NULL);
        thr_id[1] = pthread_create(&thread[1],NULL,engine,NULL);
        thr_id[2] = pthread_create(&thread[2],NULL,controller,NULL);
        thr_id[3] = pthread_create(&thread[3],NULL,cluster,NULL);
		

		// 소켓 연결 3개 수립
        for(int index=0;index<4;index++) {
                clientlen = sizeof(clientaddr);
                connfd[index] = accept(listenfd, &clientaddr, &clientlen);

                /* 클라이언트의 도메인 이름과 IP 주소 결정 */
                hp = gethostbyaddr((char *)&clientaddr.sin_addr.s_addr,
                                sizeof(clientaddr.sin_addr.s_addr), AF_INET);
                haddrp = inet_ntoa(clientaddr.sin_addr);
                printf("connfd %d connected.\n",index);
                sleep(1);
        }

        pthread_join(thr_id[0],NULL);
        pthread_join(thr_id[1],NULL);
        pthread_join(thr_id[2],NULL);
        pthread_join(thr_id[3],NULL);

        pause();
        //exit(0);
}
