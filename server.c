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
    #include <sys/stat.h>
    #include <sys/ioctl.h>
    #include <fcntl.h>
    #include <dirent.h>
    #include <signal.h>
    #include <arpa/inet.h>


int kill_living(char** argv)
{
    DIR *dir_proc = opendir("/proc");
    if(dir_proc == NULL)
        exit(1);
    
    
    char procPath[6] = "/proc/";
    char* comm = malloc(sizeof(char)*5);
    comm="/comm";
    
    
    struct dirent *intoP = readdir(dir_proc);
    
    char* path_part_thread = malloc(sizeof(char)*11);
    path_part_thread="thread-self";
    char* path_part_server= malloc(sizeof(char)*6);
    path_part_server = "server";
    char* path_part_self = malloc(sizeof(char)*4);
    path_part_self = "self";

    
    size_t nbyte=6;
    void* bufferino[6];
    
    pid_t myPID = getpid();
    int i;
    
    while(intoP)
    {
        char* file =malloc(strlen(procPath)+strlen(comm)+strlen(intoP->d_name)*sizeof(char));
        sprintf(file, "%s%s%s", procPath, intoP->d_name, comm);

        int handle;
        if(handle = open(file,O_RDONLY) == -1) {
            printf("Open fail!\n");
                exit(1);
        }

        if (read(handle, bufferino, nbyte) < 0) {
         perror("Read Error");
         exit(1);
        }
        ;
        char* cmp = (char *) bufferino;
        if(!strcmp(cmp,path_part_server))
        {
            if((myPID != atoi(intoP->d_name)))
            {
                if (strcmp(intoP->d_name,path_part_self) != 0) {
                    if (strcmp(intoP->d_name,path_part_thread)!=0) {
                        kill(atoi(intoP->d_name),SIGKILL);
                        return 0;
                    }
                }   
            }
        }
        free(file);
        close(handle);
        intoP= readdir(dir_proc);
    }
    free(path_part_server);
    free(path_part_self);
    free(path_part_thread);
}

int main(int argc, char **argv) {

       
 /* DAEMONIZING */
    pid_t pid;
    int foldie;
        // DAEMONIZED ALREADY
       // if(getppid() == 1) return 1; 

        //TERMINATE PARENTS MAKE CHILD RUN IN THE BACK
        pid = fork();
        if(pid<0)
            exit(EXIT_FAILURE);
        if(pid>0)
            exit(EXIT_SUCCESS);
        

        //NEW SESSION
        pid_t session_id= setsid();
        if(session_id<0)
            exit(EXIT_FAILURE);

        // CHANGE WORKING DIR OF DAEMON
        if((chdir("/")) <0)
            exit(EXIT_FAILURE);

        // CLOSE FILE DESCRIPTORS
        if(foldie = open("/dev/null", O_RDWR,0) != -1)
        {
            dup2(foldie,STDOUT_FILENO);
            
            dup2(foldie, STDIN_FILENO);
            dup2(foldie,STDERR_FILENO);
            if(foldie>2)
                close(foldie);
        }

        // NEW FILE PERMISSIONS
        umask(0);
        
/* DAEMONIZING END */

        int pflag = 0, qflag=0, ret;
        

        // SERVER OPTIONS -P FOR RUN -Q FOR KILLL
        while ((ret=getopt(argc, argv, "pq")) != -1) {
            switch (ret) {
                case 'p': pflag = 1; break;
                case 'q': qflag = 1; break;
                case '?': return 1;
                default: abort();
            }
            if (pflag && !qflag) {

                fd_set readfds;

            // LOGIN TABLE START
            char **users = (char**) malloc(10*sizeof(char*));   // MAX 10 USERS
            int i;
            for(i = 0; i < 10; i++)
            {
                users[i] = (char*) malloc(10*sizeof(char));       //  NAME MAX 19 LONG
                users[i][0] = 0;
            }
            
            // LOGIN TABLE END
            
            char buffer[61];
            
            unsigned short PORT = atoi(argv[2]);
            struct sockaddr_in sa, ca;
            int caLen, sd, max_sd, activity;;
            char msg[61];

            /* CONFIG FOR SOCKET START */

            //SWITCH THE 5 IF U HAVE MONEY
            int client_socket[5], ch, sh = socket(AF_INET, SOCK_STREAM, 0);
            if(sh == -1){
                perror("");
                return 1;
            }
        
            // HERE WE GET THE GUYS ID'S
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
            /* NOT THE END BUT MAINLY DONE */
            int k;
            while (1) {

                //CLEAR SET
                FD_ZERO(&readfds);

                // ADD DESCRIPTOR TO A SET
                FD_SET(sh, &readfds);
                max_sd = sh;
                
                for ( i = 0 ; i < 5 ; i++)
                {
                    //SAKET DESCRIPTOR
                    sd = client_socket[i];
                    
                    //ADD TO READ LIST IF VALID DESCRIPTOR
                    if(sd > 0)
                        FD_SET( sd , &readfds);
                    
                    //the BIGGUER FD NR, FOR SELECT FUNC
                    if(sd > max_sd)
                        max_sd = sd;
                }
                
                activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
                if (FD_ISSET(sh, &readfds)) {

                    ch=accept(sh, (struct sockaddr*)&ca, &caLen);
                    
                    
                    
                    //SOCKER NUMBER
                    printf("New connection \n");
                    
                    
                    // NEW SOCKET TO ARRAY OF SOCKETS
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
                    
                    
                }
                // HERE IF IN-OUT OPERATION ON OTHER SOCKET
                for (i = 0; i < 5; i++)
                {
                    sd = client_socket[i];
                    
                    if (FD_ISSET( sd , &readfds))
                    {
                        int valread;
                        // WAS IT FOR CLOSING?

                        if ((valread = read( sd , buffer, 30)) == 0)
                        {
                            // SB GONNA OUT
                            int addrlen = sizeof(sa);
                            getpeername(sd , (struct sockaddr*)&sa , \
                                (socklen_t*)&addrlen);
                            printf("Host disconnected");
                            
                            // CLOSE SOCKET MAKE IT FREE FOR NEW GUYS
                            close( sd );
                            client_socket[i] = 0;
                        }
                        
                        // SCREAM BACK THE MESSAGE
                        else
                        {

                            int j;

                            char tosendBuf[60];

                            // WHO IS INSIDE 
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

                            // LET ME WHISPER TO YA
                            else if (strncmp(buffer, "/", 1) == 0) {
                                //printf("jest petla\n");
                                char user_pw[19];
                                char* pdest = memccpy( user_pw, buffer, ' ', 19);

                                *pdest = '\0';
                                // printf("po memie: %s \n", user_pw);
                                char* toCompare = user_pw;
                                toCompare++;
                                // printf("to compare: %s", toCompare);

                                strcpy(tosendBuf, users[i]);
                                strcat(tosendBuf, " ||");
                                strcat(tosendBuf, strchr(buffer, ' '));


                                for (j = 0; j < 5; j++){

                                    if(strncmp(toCompare, users[j], strlen(users[j])) == 0) {
                                        send(client_socket[j] , tosendBuf , strlen(tosendBuf), 0 );
                                    }
                                }

                                
                            // LETS TALK ABOUT BULLS*IT
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
            } // ALL ELEMENTS OUT
            free(users);

            
        }
            else if (!pflag && qflag) {

            // KILL LIKE THANOS
            kill_living(argv);
            return 1;
        }
            else {

            }

        }

}
