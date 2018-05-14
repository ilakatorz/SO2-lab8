/**********************
 ./client 127.0.0.1
 ***********************/
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv){
    
    struct sockaddr_in sa;
    struct hostent *he;
    
    char ip_addr[10], username[19];
    
    char buf[21], msg[61];
    strncpy(ip_addr, argv[1], 10);
    strncpy(username, argv[3], 19);
    unsigned short PORT = atoi(argv[2]);
    
    
    he = gethostbyname(ip_addr);
    
    int sh = socket(AF_INET, SOCK_STREAM, 0);
    if(sh == -1){
        perror("");
        return 1;
    }
    sa.sin_family = AF_INET;
    memcpy(&sa.sin_addr.s_addr, he->h_addr, he->h_length);
    sa.sin_port = htons(PORT);
    
    if( connect(sh, (struct sockaddr*)&sa, sizeof(sa)) != -1) {
        
        send(sh,username,strlen(username),0);
        int k;
        pid_t pid;
        pid = fork();
        while(1)
        {
            
            if (pid == 0) {
                fgets(msg,31,stdin);
                if(strncmp(msg,"end",3)==0)
                    break;
                
              //  printf("client 1st: %s size: %lu \n", msg, sizeof(msg));
                k=send(sh,msg,strlen(msg),0);
                if(k==-1)
                {
                    printf("Error in sending");
                    exit(1);
                }
            }
            else {
                
                k=recv(sh,buf,65,0);
                if(k==-1)
                {
                    printf("Error in receiving");
                    exit(1);
                }
                printf("%s", buf);
                memset(buf, 0, sizeof(buf));
            }
        }
    }
    close(sh);
}
