#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char **argv) {
    
    //daemon(0,0);
    
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
                
                puts("Welcome message sent successfully");
                
                //add new socket to array of sockets
                for (i = 0; i < 5; i++)
                {
                    //if position is empty
                    if( client_socket[i] == 0 )
                    {
                        client_socket[i] = ch;
                        printf("Adding to list of sockets as %d\n" , i);
                        recv(ch, users[i], 19, 0);
                        printf("Connected user: %s ", users[i]);
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
                        char tosendBuf[60];

                        strcpy(tosendBuf, users[i]);
                        
                        strcat(tosendBuf, " : ");
                        strncat(tosendBuf, buffer, valread);

                       // printf("sizeof: %lu valread: %d \n", sizeof(buffer), valread);
                         buffer[valread+strlen(users[i]) + 2] = '\0';
                        int j;

                        for (j = 0; j < 5; j++){
                            send(client_socket[j] , tosendBuf , strlen(tosendBuf), 0 );
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
        
    }
    else {
        
    }
}
