#include<stdio.h>
#include <termios.h>              
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define BHRED "\e[1;91m"
#define BHWHT "\e[1;97m"
#define BHYEL "\e[1;93m"
#define BHMGN "\033[1m\033[35m"
#define RESET "\033[0m"

// Defining CHECK() tool. We use this error checking method to make the
// code lighter and more fancy, using errno.

#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n",__LINE__,strerror(errno)); exit(-1);-1;}) : __val); })

pid_t pid_command;

int main(int argc, char * argv[]){

	int fd_mx_to_ins , fd_mz_to_ins, fd_command_to_ins;
	int position_x , position_z;
	int pid_motor_x, pid_motor_z;
	int ret;
	
	char c_1;

	time_t current_time;
	struct timeval tv={0,0};

	fd_set rset;

	FILE *out = fopen("debug.txt", "a");
 
 	if(out == NULL){
        printf("ERRROR OPEN FILE");
    }

	static struct termios oldt, newt;
    CHECK(tcgetattr( STDIN_FILENO, &oldt));
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          
    CHECK(tcsetattr( STDIN_FILENO, TCSANOW, &newt));

    fd_command_to_ins = CHECK(open(argv[3], O_RDONLY));
	fd_mx_to_ins = CHECK(open(argv[4],O_RDONLY));
	fd_mz_to_ins = CHECK(open(argv[5],O_RDONLY));

	pid_motor_x = atoi(argv[1]);
	pid_motor_z = atoi(argv[2]);

	printf("\n" BHMGN"  ######################" RESET "\n");
	printf(BHMGN "  # INSPECTION KONSOLE #" RESET "\n");
	printf(BHMGN"  ######################" RESET "\n\n");

	read(fd_command_to_ins,&pid_command, sizeof(int));

	while(1){

		FD_ZERO(&rset);
        FD_SET(fd_mx_to_ins, &rset);
        FD_SET(fd_mz_to_ins, &rset);
        FD_SET(0,&rset);
        ret = CHECK(select(FD_SETSIZE, &rset, NULL, NULL, &tv));

        time(&current_time);

		if(ret>0){

			if (FD_ISSET(fd_mx_to_ins, &rset)>0){
				CHECK(read(fd_mx_to_ins, &position_x, sizeof(int)));
			}

			if (FD_ISSET(fd_mz_to_ins, &rset)>0){
				CHECK(read(fd_mz_to_ins, &position_z, sizeof(int)));
			}

			if (FD_ISSET(0,&rset)>0){

				CHECK(read(0, &c_1, sizeof(char)));

				if(c_1=='s'){

					printf("\n"BHRED "  EMERGENCY STOP" RESET "\n");
					fflush(stdout);
					CHECK(kill(pid_motor_x, SIGUSR1));
					CHECK(kill(pid_motor_z, SIGUSR1));
					CHECK(kill(pid_command, SIGUSR1));
					fprintf(out, "Stop button pressed. ");
					fprintf(out, "Time:  %s", ctime(&current_time));
				}
				if(c_1=='r'){

					printf("\n"BHYEL "  RESET" RESET "\n");
					fflush(stdout);
					CHECK(kill(pid_motor_x,SIGUSR2));
					CHECK(kill(pid_motor_z,SIGUSR2));
					CHECK(kill(pid_command,SIGUSR2));
					fprintf(out, "Reset button pressed.");
					fprintf(out, "Time:  %s", ctime(&current_time));
				}
			}
		}

		if(position_x==0 && position_z==0){
			CHECK(kill(pid_command, SIGUSR1));
		}

		printf(BHWHT "\r  La posizione lungo x è: %d m, La posizione lungo z è: %d m" RESET, position_x, position_z);
		fflush(stdout);
  		fflush(out);
	}
	CHECK(tcsetattr( STDIN_FILENO, TCSANOW, &oldt));
	return 0;
}
