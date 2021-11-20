// Including libraries.

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

// Defining colors of the lines in the console.

#define BOLDBLACK "\033[1m\033[30m"     // Bold Black.
#define BOLDRED "\033[1m\033[31m"       // Bold Red.
#define BOLDGREEN "\033[1m\033[32m"     // Bold Green.
#define BOLDYELLOW "\033[1m\033[33m"    // Bold Yellow.
#define BOLDBLUE "\033[1m\033[34m"      // Bold Blue.
#define BOLDMAGENTA "\033[1m\033[35m"   // Bold Magenta.
#define BOLDCYAN "\033[1m\033[36m"      // Bold Cyan.
#define BOLDWHITE "\033[1m\033[37m"     // Bold White.
#define RESET "\033[0m"

// Defining CHECK() tool. We use this error checking method to make the
// code lighter and more fancy, using errno.

#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n",__LINE__,strerror(errno)); exit(-1);-1;}) : __val); })

// We decided to create the type bool to make the code easier to
// understand, as long as the bool type is really useful.

typedef enum {
    false,
    true
}bool;

// Declaring the variable reset, we will see later in the code why it is
// so important.

bool reset = false;

// Declaring sighandler() function, this function will be called whenever
// an emergency button has been pressed in the inspection konsole.
// SIGUSR2 is used to start the reset routine of the position of both axis.
// SIGUSR1 is used to stop the reset routine.

void sighandler(int sig){

    if(sig==SIGUSR2){
        reset = true;
        printf("\n" BOLDBLUE "  SYSTEM RESETTING" RESET "\n");      // BOLDBLUE, BOLDYELLOW, RESET
        fflush(stdout);                                             // are macros to color the text
        printf("\n" BOLDYELLOW "  COMMAND DISABLED" RESET "\n");    // on the consoles.
        fflush(stdout);
    }

    if(sig==SIGUSR1) reset = false;

}

// Creating main() function, where all the main tasks will take place.

int main(int argc, char *argv[]){   

    // Opening and checking errors the debug file in "a" mode, which means 
    // append. This parameter is fundamental because if it wasn't existing 
    // it wouldn't there be the other lines written in the other processes.  
    // We would have the problem of overwriting the file.

    FILE *out = fopen("debug.txt", "a");
    if(out == NULL){
        printf("ERRROR OPEN FILE");
    }

    // Declaring local integer variables, they are grouped to get a better  
    // look of the code and making it easier to modify or fix.

    int c_1 , c_2 , c_3;
    int fd_c_to_mx, fd_c_to_mz, fd_c_to_ins, fd_c_to_wd;
    int right = 1, left = 2, xstop=3, up = 4, down = 5, zstop = 6;

    // Declaring and assigning values of the PIDs of the command and the 
    // inspection consoles.

    pid_t pid_command = getpid();
    pid_t pid_wd = atoi(argv[1]);

    // declaring the two termios structs.

    static struct termios oldt, newt;

    // Declaring and building the use of the signals.

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler=&sighandler;
    sa.sa_flags=SA_RESTART;
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGUSR1, &sa,NULL);

    // This is a method to not use the enter button on the two consoles.
    // The tcgetattr() function shall get the parameters associated with 
    // the terminal referred to by the first argument and store them in 
    // the termios structure referenced by the second argument.
    // The first argument is an open file descriptor associated with a 
    // terminal. STDIN_FILENO is the default standard input fd (0).

    CHECK(tcgetattr( STDIN_FILENO, &oldt));
    newt = oldt;
    newt.c_lflag &= ~(ICANON);                              // This makes the standard input taking values     
    CHECK(tcsetattr( STDIN_FILENO, TCSANOW, &newt));        // without waiting for enter to be pressed.

    // Now we want to open the fifos and use them to make command process
    // communicating with the other processes.

    fd_c_to_ins = CHECK(open(argv[2] ,O_WRONLY));
    fd_c_to_mx = CHECK(open(argv[3], O_WRONLY));
    fd_c_to_mz = CHECK(open(argv[4], O_WRONLY));
    fd_c_to_wd = CHECK(open(argv[5], O_WRONLY));

    // Printing on the console informations about the simulator.

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

    // Passing the command PID to inspection and watchdog process.

    CHECK(write(fd_c_to_ins, &pid_command, sizeof(int)));
    CHECK(write(fd_c_to_wd, &pid_command, sizeof(int)));

    // Now we are going to study the loop part of command process.

    while(c_1 = getchar()){

        // Reading the input on the command console.

        switch(reset){

            // When the process is hit by SIGUSR2 the command can't take no
            // more inputs.

            case true:

                if(c_1 != 0){
                    printf("\n" BOLDRED " SYSTEM RESETTING, WAIT UNTIL IT FINISHES!" RESET "\n");
                    fflush(stdout);
                }

                if(c_1 == 32){
                    exit(1);
                }

            break;

            // When the process is hit by SIGUSR1 the command process is set
            // to the default tasks.

            case false:

                switch(c_1){

                    // Case when 'x' button is pressed on the keyboard, the x axis does
                    // not increase no more. It just stops by sending a signal to the
                    // motor x process.

                    case 120:

                        printf("\n" BOLDRED "  X Axis Stopped" RESET "\n");
                        fflush(stdout);

                        CHECK(write(fd_c_to_mx, &xstop, sizeof(int)));
                        CHECK(kill(pid_wd, SIGUSR1));

                        fprintf(out, "Pressed x, x axis stopped.\n");
                        fflush(out);

                    break;

                    // Case when 'z' button is pressed on the keyboard, the z axis does
                    // not increase no more. It just stops by sending a signal to the
                    // motor z process.

                    case 122:

                        printf("\n" BOLDRED "  Z Axis Stopped" RESET "\n");
                        fflush(stdout);

                        CHECK(write(fd_c_to_mz, &zstop, sizeof(int)));
                        CHECK(kill(pid_wd, SIGUSR1));

                        fprintf(out, "Pressed z, z axis stopped. \n");
                        fflush(out);
    
                    break;

                    case 27:

                        // Reading the other 

                        c_2=getchar();
                        c_3=getchar();

                        switch(c_3){

                            // UP Arrow, in ASCII code three different inputs 27, 91, 65. We want to pass
                            // to the motor z process a code to start the motor of the z axis. We pass a 
                            // signal to the watchdog to make the alarm() function restart.

                            case 65:

                            	printf("\n" BOLDGREEN "  UP Arrow" RESET "\n");
                                fflush(stdout);

                                CHECK(write(fd_c_to_mz, &up, sizeof(int)));
                                CHECK(kill(pid_wd, SIGUSR1));

                                fprintf(out, "Pressed up arrow, z axis increasing.\n");
                                fflush(out);

                            break;

                            // DOWN Arrow, in ASCII code three different inputs 27, 91, 66. We want to pass
                            // to the motor z process a code to start reversed the motor of the z axis. We 
                            // pass a signal to the watchdog to make the alarm() function restart. 

                            case 66:

                            	printf("\n" BOLDGREEN"  DOWN Arrow" RESET "\n");
                                fflush(stdout);

                                CHECK(write(fd_c_to_mz, &down, sizeof(int)));
                                CHECK(kill(pid_wd, SIGUSR1));

                                fprintf(out, "Pressed down arrow, z axis decreasing.\n");
                                fflush(out);
                                

                            break;

                            // RIGHT Arrow, in ASCII code three different inputs 27, 91, 67. We want to pass
                            // to the motor x process a code to start the motor of the x axis. We 
                            // pass a signal to the watchdog to make the alarm() function restart. 

                            case 67:

                            	printf("\n" BOLDGREEN "  RIGHT Arrow" RESET "\n");
                                fflush(stdout);

                                CHECK(write(fd_c_to_mx, &right, sizeof(int)));
                                CHECK(kill(pid_wd, SIGUSR1));

                                fprintf(out, "Pressed right arrow, x axis increasing.\n");
                                fflush(out);
                  
                            break;

                            // LEFT Arrow, in ASCII code three different inputs 27, 91, 68. We want to pass
                            // to the motor x process a code to start reversed the motor of the x axis. We 
                            // pass a signal to the watchdog to make the alarm() function restart. 

                            case 68:

                            	printf("\n"BOLDGREEN"  LEFT ARROW"RESET"\n");
                                fflush(stdout);

                                CHECK(write(fd_c_to_mx, &left, sizeof(int)));
                                CHECK(kill(pid_wd, SIGUSR1));

                                fprintf(out, "Pressed left arrow, z axis decreasing.\n");
                                fflush(out);
                            
                            break;

                        }

                    break; 


                    case 32:

                        exit(1);

                    break;

                    // If none of the right keys are pressed, we let the user know
                    // that he's not pressing the right keys.

                    default:

                        printf("\n"BOLDRED "  Command Not Allowed" RESET "\n");
                        fflush(stdout);

                    break;
                }

            break;
        }

    }

    // Then we close all the file descriptors.

    CHECK(close(fd_c_to_mx));
    CHECK(close(fd_c_to_mz));
    CHECK(close(fd_c_to_ins));

    // With this command we make the command console to 
    // take inputs again with pressing enter.

    CHECK(tcsetattr( STDIN_FILENO, TCSANOW, &oldt));

    // Returning value.

    return 0;
}