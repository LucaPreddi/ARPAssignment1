#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <errno.h>

#define BOLDBLACK "\033[1m\033[30m" /* Bold Black */
#define BOLDRED "\033[1m\033[31m" /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m" /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m" /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m" /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m" /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m" /* Bold White */
#define RESET "\033[0m"

#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n",__LINE__,strerror(errno)); exit(-1);-1;}) : __val); })

typedef enum {
    false,
    true
}bool;

bool reset = false;

void sighandler(int sig){

    if(sig==SIGUSR2){
        reset = true;
        printf("\n" BOLDBLUE "  SYSTEM RESETTING" RESET "\n");
        fflush(stdout);
        printf("\n" BOLDYELLOW "  COMMAND DISABLED" RESET "\n");
        fflush(stdout);
    }
    if(sig==SIGUSR1){
        reset = false;
    }
}


int main(int argc, char *argv[]){   

    FILE *out = fopen("debug.txt", "a");
    if(out == NULL){
        printf("ERRROR OPEN FILE");
    }

    int c_1 , c_2 , c_3;
    int fd_c_to_mx, fd_c_to_mz, fd_c_to_ins, fd_c_to_wd;

    int right=1;
    int left=2;
    int xstop=3;
    int up = 4;
    int down = 5;
    int zstop = 6;

    pid_t pid_command;

    pid_command = getpid();

    pid_t pid_wd;

    pid_wd = atoi(argv[1]);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler=&sighandler;
    sa.sa_flags=SA_RESTART;
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGUSR1, &sa,NULL);

    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    fd_c_to_ins=CHECK(open(argv[2] ,O_WRONLY));
    fd_c_to_mx=CHECK(open(argv[3], O_WRONLY));
    fd_c_to_mz=CHECK(open(argv[4], O_WRONLY));
    fd_c_to_wd=CHECK(open(argv[5], O_WRONLY));

    printf("\n" BOLDMAGENTA"  ###################" RESET "\n");
    printf(BOLDMAGENTA "  # COMMAND KONSOLE #" RESET "\n");
    printf(BOLDMAGENTA"  ###################" RESET "\n\n");

    printf(BOLDRED "  Welcome to you my friend, this is a simulator of a hoist robot!" RESET);    
    printf("\n" BOLDRED "  Created by Matteo Carlone and Luca Predieri." RESET "\n\n");    
    printf(BOLDYELLOW "  Here there's a list of commands:" RESET "\n");
    printf(BOLDGREEN "  If you want to move, press right arrow!" RESET "\n");
    printf(BOLDCYAN "  If you want to move back, press left arrow!" RESET "\n");
    printf(BOLDBLUE "  If you want to move down, press up arrow!" RESET "\n");
    printf(BOLDBLUE "  If you want to move up, press down arrow!" RESET "\n");
    printf(BOLDWHITE"  To stop the movement of the two axis, you can press X or Z!" RESET "\n\n");

    write(fd_c_to_ins, &pid_command, sizeof(int));

    write(fd_c_to_wd, &pid_command, sizeof(int));

    while(1){

        c_1=getchar();

        switch(reset){

            case true: //reset

                if(c_1 != 0){
                    printf("\n" BOLDRED " SYSTEM RESETTING, WAIT UNTIL IT FINISHES!!!" RESET "\n");
                    fflush(stdout);
                }



            break;

            case false:

                switch(c_1){

                    case 120: // x pressed

                        printf("\n" BOLDRED "  X Axis Stopped" RESET "\n");
                        fflush(stdout);

                        write(fd_c_to_mx, &xstop, sizeof(int));
                        kill(pid_wd, SIGUSR1);

                        fprintf(out, "Pressed x, x axis stopped.\n");
                        fflush(out);

                    break;

                    case 122: // z pressed 

                        printf("\n" BOLDRED "  Z Axis Stopped" RESET "\n");
                        fflush(stdout);

                        write(fd_c_to_mz, &zstop, sizeof(int));
                        kill(pid_wd, SIGUSR1);
                        fprintf(out, "Pressed z, z axis stopped. \n");
                        fflush(out);
          

                    break;

                    case 27:

                        c_2=getchar();
                        c_3=getchar();

                        switch(c_3){

                            case 65:

                            	printf("\n" BOLDGREEN "  UP Arrow" RESET "\n");
                                fflush(stdout);
                         
                                write(fd_c_to_mz, &up, sizeof(int));
                                kill(pid_wd, SIGUSR1);
                                fprintf(out, "Pressed up arrow, z axis increasing.\n");
                                fflush(out);
                        

                            break;

                            case 66:

                            	printf("\n" BOLDGREEN"  DOWN Arrow" RESET "\n");
                                fflush(stdout);
                   
                                write(fd_c_to_mz, &down, sizeof(int));
                                kill(pid_wd, SIGUSR1);
                                fprintf(out, "Pressed down arrow, z axis decreasing.\n");
                                fflush(out);
                                

                            break;

                            case 67:

                            	printf("\n" BOLDGREEN "  RIGHT Arrow" RESET "\n");
                                fflush(stdout);

                                write(fd_c_to_mx, &right, sizeof(int));
                                kill(pid_wd, SIGUSR1);
                                fprintf(out, "Pressed right arrow, x axis increasing.\n");
                                fflush(out);
                  
                            break;

                            case 68:

                            	printf("\n"BOLDGREEN"  LEFT ARROW"RESET"\n");
                                fflush(stdout);

                                write(fd_c_to_mx, &left, sizeof(int));
                                kill(pid_wd, SIGUSR1);
                                fprintf(out, "Pressed left arrow, z axis decreasing.\n");
                                fflush(out);
                            
                                
                             break;
                        }

                    break; 
            

                    default:

                        printf("\n"BOLDRED "  Command Not Allowed" RESET "\n");
                        fflush(stdout);

                        
                    break;
                }

            break;
        }

    }

    close(fd_c_to_mx);
    close(fd_c_to_mz);
    close(fd_c_to_ins);
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}