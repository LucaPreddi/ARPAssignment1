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

#define BOLDBLACK "\033[1m\033[30m" /* Bold Black */
#define BOLDRED "\033[1m\033[31m" /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m" /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m" /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m" /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m" /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m" /* Bold White */
#define RESET "\033[0m"


int main(int argc, char *argv[]){   

    int c_1 , c_2 , c_3;
    int fd_c_to_mx, fd_c_to_mz;
    int right=1;
    int left=2;
    int xstop=3;
    int up = 4;
    int down = 5;
    int zstop = 6;

    pid_t pid_wd;

    pid_wd = atoi(argv[1]);

    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    
    fd_c_to_mx=open("fifo_command_to_mot_x", O_WRONLY);
    
    if (fd_c_to_mx==-1){
        printf("Error while trying to open the pipe");
    }
    fd_c_to_mz=open("fifo_command_to_mot_z", O_WRONLY);
    if (fd_c_to_mz == -1){
        printf("Error while trying to open the fifo");
    }

    printf("\n" BOLDRED "  Welcome to you my friend, this is a simulator of a hoist robot!" RESET "\n");    
    printf(BOLDYELLOW "  Here there's a list of commands:" RESET "\n");
    printf(BOLDGREEN "  If you want to move, press right arrow!" RESET "\n");
    printf(BOLDCYAN "  If you want to move back, press left arrow!" RESET "\n");
    printf(BOLDBLUE "  If you want to move down, press up arrow!" RESET "\n");
    printf(BOLDBLUE "  If you want to move up, press down arrow!" RESET "\n");
    printf(BOLDWHITE"  To stop the movement of the two axis, you can press X or Z!" RESET "\n\n");

    while(1){

        c_1=getchar();

        switch(c_1){

            case 120: // x pressed

                printf("\n  ho letto : %c\n", c_1);
                write(fd_c_to_mx, &xstop, sizeof(int));
                kill(pid_wd, SIGUSR1);

            break;

            case 122: // z pressed 

                printf("\n  ho letto: %c\n", c_1);
                write(fd_c_to_mz, &zstop, sizeof(int));
                kill(pid_wd, SIGUSR1);

            break;

            case 27:

                c_2=getchar();
                c_3=getchar();

                switch(c_3){

                    case 65:

                    	printf("\n  Freccetta in alto\n");
                        write(fd_c_to_mz, &up, sizeof(int));
                        kill(pid_wd, SIGUSR1);

                    break;

                    case 66:

                    	printf("\n  Freccetta in basso\n");
                        write(fd_c_to_mz, &down, sizeof(int));
                        kill(pid_wd, SIGUSR1);

                    break;

                    case 67:

                    	printf("\n  Freccetta a destra\n");
                        write(fd_c_to_mx, &right, sizeof(int));
                        kill(pid_wd, SIGUSR1);

                    break;

                    case 68:

                    	printf("\n  Freccetta_a_sinistra\n");
                        write(fd_c_to_mx, &left, sizeof(int));
                        kill(pid_wd, SIGUSR1);
                        
                    break;
                }
            break; 
            }
        }
    close(fd_c_to_mx);
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}