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
int connfd[1];

// 엔진, 컨트롤러, 클러스터 인풋 데이터 저장소
char inmsg0[MAXLINE];

// 누적 주행거리 변수
//int curodo, odo;

// 보내기 버퍼
char s_buffer[1][100][MAXLINE];
int s_cur_index[1];
int s_save_index[1];

void sendData(char* input) {
        printf("[sendData]recv string %s : send to ecu\n",input);
	    sprintf(s_buffer[0][s_save_index[0]],"%s",input);
        s_save_index[0]++;
        if(s_save_index[0] >= 100) s_save_index[0] = 0;
        printf("[sendData]send to queue complete.\n");
}

void sendIntData(char* input,int vinput) {
        printf("[sendData]recv string %s int %d: send to ecu\n",input,vinput);
	    sprintf(s_buffer[0][s_save_index[0]],"%s%d",input,vinput);
        s_save_index[0]++;
        if(s_save_index[0] >= 100) s_save_index[0] = 0;
        printf("[sendData]send to queue complete.\n");
}

int readLine(int fd, char* str) {
        int n;
        do {
                n = read(fd, str, 1);
        } while(n > 0 && *str++ != NULL);
        return n;
}

// 전송
void *sender() {
	printf("sender run\n");
	int len=0;
	while(1) {
		if(connfd[0]==NULL) continue;
		if(s_cur_index[0] != s_save_index[0]) {


			len=write(connfd[0],s_buffer[0][s_cur_index[0]],strlen(s_buffer[0][s_cur_index[0]])+1);
			printf("sender send : %s\n",s_buffer[0][s_cur_index[0]]);
			s_cur_index[0]++;
			if(s_cur_index[0] >= 100) s_cur_index[0]=0;
		}

//		usleep(100*1000);

	}
}

// 엔진 변수 선언
//speed=40~315
//rpm=40~315
//count=275
int speed[160] = {6,6,10,12,13,14,16,18,20,23,24,24,25,26,27,28,29,29,29,30,10,11,12,13,13,13,14,15,15,15,16,17,18,19,19,19,20,21,22,23,23,24,25,25,25,26,27,28,29,30,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,15,16,16,16,17,17,17,18,18,18,18,19,20,20,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,15,16,17,18,19,20,21,22,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,49,50,53,56,59,61,63,65,67,69,71,72,73,74,75,76,77,78,79,80,80,80,80};

int pon=100,poff=200,parking=300,drive=400,accel=500,noaccel=600,brake=700,nobrake=800;
int none=900,back=1000;

int state=200;
int accel_pedal=600;
int brake_pedal=800;
int gear_bong=300;

// 0:엔진
void *receiver() {
        printf("engine recv run.\n");

        while(1) {
                if(connfd[0] == NULL) continue;
                memset(inmsg0, 0x00, sizeof(inmsg0));

                // 컨트롤러에서 받은 데이터 처리
                if (readLine(connfd[0],inmsg0)>0) {
					if( (strcmp(inmsg0,"onoff")==0) && (state==poff) ) {state=pon; sendData("pwr=on");}
					else if( (strcmp(inmsg0,"onoff")==0) && (state==pon) ) {state=poff; sendData("pwr=off");}
				
					if( (strcmp(inmsg0,"p")==0) ) gear_bong=parking;
					if( (strcmp(inmsg0,"r")==0) ) gear_bong=back;
					if( (strcmp(inmsg0,"d")==0) ) gear_bong=drive;
					if( (strcmp(inmsg0,"n")==0) ) gear_bong=none;

					if( (strcmp(inmsg0,"accelon")==0) ) accel_pedal=accel;
					if( (strcmp(inmsg0,"acceloff")==0) ) accel_pedal=noaccel;
					if( (strcmp(inmsg0,"brakeon")==0) ) brake_pedal=brake;
					if( (strcmp(inmsg0,"brakeoff")==0) ) brake_pedal=nobrake;
                }	
				printf("[state] %d\n",state);
				printf("[gear] %d\n",gear_bong);
				printf("[accel] %d\n",accel_pedal);
				printf("[brake] %d\n",brake_pedal);
        }
}

/*
[engine]recv : onoff
[engine]recv : brakeon
[engine]recv : brakeoff
[engine]recv : accelon
[engine]recv : acceloff
[engine]recv : p
[engine]recv : r
[engine]recv : n
[engine]recv : d
*/
/*
ON 100
OFF 200
PARKING 300
DRIVE 400
ACCEL 500
NOACCEL 600
BRAKE 700
NOBRAKE 800
NONE 900
BACK 1000
PARK 1100
*/

int cur_speed = 0;
int cur_rpm = 0;
int drive_state = 300;

void *engine() {

	int temp_speed=0,temp_rpm=0;

	while (1) {
		if (state==pon) {
			if (cur_rpm < 6) cur_rpm=6;
			if (gear_bong==parking) {
				cur_speed=0;
				if ((accel_pedal==accel) && (cur_rpm < 80)) cur_rpm+=6; 
				if ((accel_pedal==noaccel) && (cur_rpm > 6)) cur_rpm-=6;
				drive_state=parking;
			}

			if (gear_bong==none) {
				if (cur_speed > 0) cur_speed--;
				if ((accel_pedal==accel) && (cur_rpm < 80)) cur_rpm+=6;
				if ((accel_pedal==noaccel) && (cur_rpm > 6)) cur_rpm-=6;
				if ((brake_pedal==brake) && (cur_speed > 0)) cur_speed-=3;
				drive_state=none;
			}

			if (gear_bong==drive) {
				if((accel_pedal==accel) && (cur_speed < 159)) {cur_speed++; cur_rpm=speed[cur_speed];}
				if((accel_pedal==noaccel)&& (cur_speed > 0)) {cur_speed--; cur_rpm=speed[cur_speed];}

			//	if((brake_pedal==nobrake) && (cur_speed < 180)) {cur_speed++; cur_rpm=speed[cur_speed];}
				if((brake_pedal==brake)&& (cur_speed > 0)) {cur_speed-=3; cur_rpm=speed[cur_speed];}
				if(cur_speed<0) cur_speed=0;
				drive_state=drive;
			}

			if (gear_bong==back) {
				if(drive_state!=back) cur_speed=0;
				if((accel_pedal==accel) && (cur_speed < 159)) {cur_speed++; cur_rpm=speed[cur_speed];}
				if((accel_pedal==noaccel)&& (cur_speed > 0)) {cur_speed--; cur_rpm=speed[cur_speed];}
				if((brake_pedal==brake)&& (cur_speed > 0)) {cur_speed-=3; cur_rpm=speed[cur_speed];}
				if(cur_speed<0) cur_speed=0;
				drive_state=back;
			}
		}
		if (state==poff) {cur_speed=0; cur_rpm=0;}

//		char data_rpm[100],data_speed[100],string_rpm[100]="rpm=",string_speed[100]="speed=";
		//itoa(cur_rpm,data_rpm,10);
//		sprintf(data_rpm,"%d",cur_rpm);
		//itoa(cur_speed,data_speed,10);
//		sprintf(data_speed,"%d",cur_speed);

		//	string_rpm = "rpm=";
		//	string_speed = "speed=";
//		strcat(string_rpm,data_rpm);
//		strcat(string_speed,data_speed);

		if (temp_speed != cur_speed) { sendIntData("speed=",cur_speed); temp_speed = cur_speed; }
		if (temp_rpm != cur_rpm) { sendIntData("rpm=",cur_rpm); temp_rpm = cur_rpm; }

		usleep(200*1000);
	}
}


int curodo, odo;
FILE *fp;


void *odo_cal () {
	//FILE *fp;

	int internal_state=200;
	int internal_pwron=100;
	int internal_pwroff=200;

	int count=0;

//	fp=open("./data","r+");
//	printf("open\n");

	int temp_curodo=0,temp_odo=0;

	while (1) {
		if ((state==pon) && (internal_state==internal_pwroff)) {
			fp=fopen("data","r");
			if (fp == NULL) { printf("null\n"); }
			printf("open\n");
			fscanf(fp,"%d",&odo);
			printf("fscanf\n");
			fclose(fp);
			printf("fclose\n");
			internal_state=internal_pwron;
			sendIntData("odo=",odo);
			sendIntData("curodo=",curodo);
		}
		else if(state==pon) {
			if(count>10) {count=0; curodo+=cur_speed/10; odo+=curodo;} 
			count++;

			if(temp_odo!=curodo) { sendIntData("odo=",odo); temp_odo=curodo; };
			if(temp_curodo!=curodo) { sendIntData("curodo=",curodo); temp_curodo=curodo; }
		}
		if ((state==poff) && (internal_state==internal_pwron)) {
			internal_state=internal_pwroff;
			
			fp=fopen("data","w");
			printf("open1\n");
			fprintf(fp,"%08d",odo);
			printf("fprintf\n");
			fclose(fp);
			printf("fclose1\n");
			
		}
		sleep(1);
	}

}


int cel_oil_state=100;
int cel_oil_pwron=200,cel_oil_pwroff=100;




void cel_oil() {
//	char string_cel[100], data_cel[100],string_oil[100],data_oil[100];
	int oil=10, cel=0;
//	FILE *fp;
//	fp = fopen(&fp,"./data",w+);

	int temp_oil=0,temp_cel=0;

	int count=0;
	int tempodo=0;

	while (1) {
		if (state==pon) {
			if(cel_oil_state==cel_oil_pwroff) { cel=0; tempodo=curodo; sendIntData("oil=",oil); sendIntData("cel=",cel); cel_oil_state=cel_oil_pwron; }
			if(curodo-tempodo>10) {oil--; tempodo=curodo; }
			if(count > 300 && cel < 7) { count=0; cel++; }
			count++;
			if (oil != temp_oil) {
				sendIntData("oil=",oil);
				temp_oil = oil;
			}
			if (cel != temp_cel) {
				sendIntData("cel=",cel);
				temp_cel = cel;
		}
		if (state==poff) {
			if(cel_oil_state==cel_oil_pwron) {
				sendData("oil=0"); sendData("cel=0"); cel_oil_state=cel_oil_pwroff;
			}
		}
		usleep(100*1000);
	}
}
}

//FILE *fp;
//fp = fopen(&fp,"./data",w+);

// 메인 함수
int main (int argc, char* argv[])
{
        int port, clientlen;
        struct sockaddr_in serveraddr, clientaddr;
        struct hostent *hp;
        char *haddrp, *host;

        signal(SIGCHLD, SIG_IGN);

        port = atoi("8000");
        host = "192.168.100.136";
        connfd[0] = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
        printf("[engine] started...\n");

        // socket reuse addr
        //if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
        //      error("setsockopt(SO_REUSEADDR) failed");

        if ((hp = gethostbyname(host)) == NULL)
                perror("gethostbyname error"); // 호스트 찾기 오류


        bzero((char *) &serveraddr, sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;

        bcopy((char *)hp->h_addr_list[0],(char *)&serveraddr.sin_addr.s_addr, hp->h_length);
        serveraddr.sin_port = htons(port);

        int result;
        do { /* 연결 요청 */
                result = connect(connfd[0], &serveraddr,  sizeof(serveraddr));
                if (result == -1) sleep(1);
        } while (result == -1);

        int thr_id[5];
        pthread_t thread[5];

        thr_id[0] = pthread_create(&thread[0],NULL,sender,NULL);
        thr_id[1] = pthread_create(&thread[1],NULL,engine,NULL);
        thr_id[2] = pthread_create(&thread[2],NULL,receiver,NULL);

		//FILE *fp = open("./data",w+);

		thr_id[3] = pthread_create(&thread[3],NULL,odo_cal,NULL);
		thr_id[4] = pthread_create(&thread[4],NULL,cel_oil,NULL);

        pthread_join(thr_id[0],NULL);
        pthread_join(thr_id[1],NULL);
        pthread_join(thr_id[2],NULL);
		pthread_join(thr_id[3],NULL);
		pthread_join(thr_id[4],NULL);

//		close(fp);
		pause();
        close(connfd[0]);
}


