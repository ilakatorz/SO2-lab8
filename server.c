#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <arpa/inet.h>


int exists(char** argv)
{
    DIR *proc = opendir("/proc");
    if(proc ==NULL)
    {
        printf("Opendir failed\n");
        exit(1);
    }
    struct dirent *inProc= readdir(proc);
    char prock[6] = "/proc/";
    char* comm = malloc(sizeof(char)*5);
    comm="/comm";
    char* serv= malloc(sizeof(char)*6);
    serv = "server";
    char* self = malloc(sizeof(char)*4);
    self = "self";
    char* tself = malloc(sizeof(char)*11);
    tself="thread-self";
    
    void* buff[6];
    size_t nbyte=6;
    int i;
    pid_t myPID = getpid();
    while(inProc)
    {
        char* file =malloc(strlen(prock)+strlen(comm)+strlen(inProc->d_name)*sizeof(char));
        strcat(file, prock);
        strcat(file, inProc->d_name);
        strcat(file, comm);
        int handle = open(file,O_RDONLY);
        read(handle, buff, nbyte);
        char* cmp = (char *) buff;
        if(!strcmp(cmp,serv))
        {
            if((myPID != atoi(inProc->d_name)) && (strcmp(inProc->d_name,self) != 0) && (strcmp(inProc->d_name,tself)!=0))
            {
                printf("Found\n");
                printf("%s\n", inProc->d_name);
                kill(atoi(inProc->d_name),SIGKILL);
                printf("Killed\n");
                return 0;
                
            }
        }
        free(file);
        close(handle);
        inProc= readdir(proc);
    }
    free(serv);
    free(self);
    free(tself);
    
}

int main(int argc, char **argv) {

    //daemon(0,0);

    pid_t pid,sid;
    int fd;
    if(getppid() == 1) return 1; //is deamon alrready
    pid = fork();
    if(pid<0)
        exit(EXIT_FAILURE);
    if(pid>0)
        exit(EXIT_SUCCESS);
    
    sid= setsid();
    if(sid<0)
        exit(EXIT_FAILURE);
    if((chdir("/")) <0)
        exit(EXIT_FAILURE);
    fd = open("/dev/null", O_RDWR,0);
    if(fd != -1)
    {
        dup2(fd, STDIN_FILENO);
        dup2(fd,STDOUT_FILENO);
        dup2(fd,STDERR_FILENO);
        if(fd>2)
            close(fd);
    }
    umask(0);
    
    int pflag = 0, qflag=0, ret;
    
    while ((ret=getopt(argc, argv, "pq")) != -1)
        switch (ret) {
            case 'p': pflag = 1; break;
            case 'q': qflag = 1; break;
            case '?': return 1;
            default: abort();
        }
        if (pflag && !qflag) {

            fd_set readfds;

        // tablica na loginy start
        char **users = (char**) malloc(10*sizeof(char*));   // max 10 userow
        int i;
        for(i = 0; i < 10; i++)
        {
            users[i] = (char*) malloc(10*sizeof(char));       // login max 19 znakow
            users[i][0] = 0;
        }
        
        // tablica na loginy end
        
        char buffer[61];
        
        unsigned short PORT = atoi(argv[2]);
        struct sockaddr_in sa, ca;
        int caLen, sd, max_sd, activity;;
        char msg[61];
        int client_socket[5], ch, sh = socket(AF_INET, SOCK_STREAM, 0);
        if(sh == -1){
            perror("");
            return 1;
        }
        
        //        if( setsockopt(sh, SOL_SOCKET, SO_REUSEADDR, (char *)1,
        //                       sizeof(1)) < 0 )
        //        {
        //            perror("setsockopt");
        //            exit(EXIT_FAILURE);
        //        }
        
        for (i = 0; i < 5; i++)
        {
            client_socket[i] = 0;
        }
        
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = INADDR_ANY;
        sa.sin_port = htons(PORT);
        
        
        if(bind(sh, (struct sockaddr*)&sa, sizeof(sa)) < 0){
            perror("");
            return 2;
        }
        if(listen(sh, 5) == -1){
            perror("");
            return 3;
        }
        //
        int k;
        while (1) {

            FD_ZERO(&readfds);
            
            FD_SET(sh, &readfds);
            max_sd = sh;
            
            for ( i = 0 ; i < 5 ; i++)
            {
                //socket descriptor
                sd = client_socket[i];
                
                //if valid socket descriptor then add to read list
                if(sd > 0)
                    FD_SET( sd , &readfds);
                
                //highest file descriptor number, need it for the select function
                if(sd > max_sd)
                    max_sd = sd;
            }
            
            activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
            if (FD_ISSET(sh, &readfds)) {

                ch=accept(sh, (struct sockaddr*)&ca, &caLen);
                
                
                
                //inform user of socket number - used in send and receive commands
                printf("New connection");
                
                //send new connection greeting message
                // if( send(ch, msg, strlen(msg), 0) != strlen(msg) )
                // {
                //     perror("send");
                // }
                
                //add new socket to array of sockets
                for (i = 0; i < 5; i++)
                {
                    //if position is empty
                    if( client_socket[i] == 0 )
                    {
                        client_socket[i] = ch;
                        printf("Adding to list of sockets as %d\n" , i);
                        recv(ch, users[i], 19, 0);
                        break;
                    }
                }
                
                //                k=recv(ch, msg, 100, 0);
                //                if(k==-1)
                //                {
                //                    printf("Error in receiving");
                //                    exit(1);
                //                }
                //
                //                printf("Message got from client is : %s",msg);
                //                printf("\nEnter data to be send to client: ");
                //
                //                //fgets(msg,100,stdin);
                //                if(strncmp(msg,"end",3)==0)
                //                    break;
                //
                //                k=send(ch,msg,100,0);
                //                if(k==-1)
                //                {
                //                    printf("Error in sending");
                //                    exit(1);
                //                }
                
            }
            //else its some IO operation on some other socket
            for (i = 0; i < 5; i++)
            {
                sd = client_socket[i];
                
                if (FD_ISSET( sd , &readfds))
                {
                    int valread;
                    //Check if it was for closing , and also read the
                    //incoming message
                    if ((valread = read( sd , buffer, 30)) == 0)
                    {
                        //Somebody disconnected , get his details and print
                        int addrlen = sizeof(sa);
                        getpeername(sd , (struct sockaddr*)&sa , \
                            (socklen_t*)&addrlen);
                        printf("Host disconnected");
                        
                        //Close the socket and mark as 0 in list for reuse
                        close( sd );
                        client_socket[i] = 0;
                    }
                    
                    //Echo back the message that came in
                    else
                    {
                        //set the string terminating NULL byte on the end
                        //of the data read
                        //printf("server 1st %s  size: %lu \n", buffer, sizeof(buffer));

                        int j;



                        char tosendBuf[60];

                        if (strncmp(buffer, "/list", 5) == 0) {
                            strcpy(tosendBuf, "Users online: ");
                            for (j = 0; j <5; j++) {
                                if (users[j] != 0)
                                    strncat(tosendBuf, users[j], strlen(users[j]));
                                    strncat(tosendBuf, " ", 1);
                            }
                            tosendBuf[strlen(tosendBuf)] = '\n';

                            printf("%s", tosendBuf);
                            send(client_socket[i] , tosendBuf , strlen(tosendBuf), 0 );
                        }
                        else if (strncmp(buffer, "/", 1) == 0) {
                            printf("jest petla\n");
                            char user_pw[19];
                            char* pdest = memccpy( user_pw, buffer, ' ', 19);

                            *pdest = '\0';
                            printf("po memie: %s \n", user_pw);
                            char* toCompare = user_pw;
                            toCompare++;
                            printf("to compare: %s", toCompare);

                            strcpy(tosendBuf, users[i]);
                            strcat(tosendBuf, " ||");
                            strcat(tosendBuf, strchr(buffer, ' '));


                            for (j = 0; j < 5; j++){

                                if(strncmp(toCompare, users[j], strlen(users[j])) == 0) {
                                    send(client_socket[j] , tosendBuf , strlen(tosendBuf), 0 );
                                }
                            }

                            

                        }
                        else {
                            strcpy(tosendBuf, users[i]);

                            strcat(tosendBuf, " : ");
                            strncat(tosendBuf, buffer, valread);

                       // printf("sizeof: %lu valread: %d \n", sizeof(buffer), valread);
                            buffer[valread+strlen(users[i]) + 2] = '\0';


                            for (j = 0; j < 5; j++){
                                send(client_socket[j] , tosendBuf , strlen(tosendBuf), 0 );
                            }
                        }
                        memset(tosendBuf, 0, sizeof(tosendBuf));
                    }
                }
            }
        }
        for(i = 0; i < 10; i++) {
            free(users[i]);
        } // usuwanie każdego elementu tabeli
        free(users);
    }
    else if (!pflag && qflag) {
        exists(argv);
        return 1;
    }
    else {

    }
}
