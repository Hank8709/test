#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLEN 8096


int main(int argc, char *argv[]){

    struct sockaddr_in clientInfo;
    struct sockaddr_in serverInfo;

    int sock_fd = socket(AF_INET , SOCK_STREAM , 0);
    int client_fd = 0;

    if(sock_fd == -1){
        exit(1);
    }

    serverInfo.sin_family = AF_INET;//選擇ipv6 還是 4
    serverInfo.sin_addr.s_addr = INADDR_ANY;//不在乎local ip,讓kernel決定
    serverInfo.sin_port = htons(1333);//選定port number

    bind(sock_fd , (struct sockaddr *)&serverInfo , sizeof(serverInfo));//告訴別人住哪裡
    listen(sock_fd , 10);//監聽別人的訪問


    char resBuffer[MAXLEN];//收到的訊息

    while(1){
        
        int size = sizeof(clientInfo);

        client_fd = accept( sock_fd , (struct sockaddr *)&clientInfo , &size);//規定位址 當案描述符 ,clientInfo:給一指標 ,size:此位置大小 ,接客人

        int res = recv(client_fd ,resBuffer,sizeof(resBuffer),0);//聽客人的請求

        printf("%s\n",resBuffer);//解析http協定

        for(int i=0 ; i<MAXLEN ; i++){
            if(resBuffer[i] == '\r' || resBuffer[i] == '\n'){
                resBuffer[i] = '\0';
                break;
            }
        }

        printf("%s\n",resBuffer);
        
        for(int i=5 ; i<MAXLEN ; i++){
            if(resBuffer[i] == ' '){
                resBuffer[i] = '\0';
                break;
            }
        }


        char *filename = &resBuffer[5];//當案名稱

        printf("%s\n",filename);
        
        int fileFd = open( filename , O_RDONLY);//要open才能讀此當案資料

        sprintf(resBuffer,"HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", "text/html");
        write(client_fd ,resBuffer ,strlen(resBuffer));//同意客人的請求,請求也可以寫到buffer

        while ((res = read(fileFd, resBuffer, MAXLEN))>0) {//回傳index,/重新利用
            write(client_fd,resBuffer,res);//把資料讀到buffer
        }
		close(client_fd);//送客
    }
	close(sock_fd);
    exit(1);
    
}
