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

#define BHGRN "\e[1;92m"
#define BHRED "\e[1;91m"
#define RESET "\033[0m"
#define BHYEL "\e[1;93m"

pid_t pid_command;

int main(int argc, char * argv[]){

	int fd_mx_to_ins , fd_mz_to_ins, fd_command_to_ins;
	int ret;
	int pid_motor_x, pid_motor_z;
	char c_1;

	int position_x , position_z;

	struct timeval tv={0,0};

	fd_set rset;

	FILE *out = fopen("debug.txt", "a");
 
	static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    fd_command_to_ins = open(argv[3], O_RDONLY);

	fd_mx_to_ins = open(argv[4],O_RDONLY);

	fd_mz_to_ins = open(argv[5],O_RDONLY);

	if (fd_mx_to_ins == -1 || fd_mz_to_ins == -1 || fd_command_to_ins == -1){
		printf("Error opening mx to inspection fifo!");
		return 5;
	}


	pid_motor_x = atoi(argv[1]);
	pid_motor_z = atoi(argv[2]);

	printf("\n" BHGRN"  #############" RESET "\n");
	printf(BHGRN "  # INSPECTOR #" RESET "\n");
	printf(BHGRN"  #############" RESET "\n\n");

	read(fd_command_to_ins,&pid_command, sizeof(int));

	while(1){

		FD_ZERO(&rset);
        FD_SET(fd_mx_to_ins, &rset);
        FD_SET(fd_mz_to_ins, &rset);
        FD_SET(0,&rset);
        ret=select(FD_SETSIZE, &rset, NULL, NULL, &tv);

		if(ret==-1){
			printf("There's an error on select.");
			fflush(stdout);
		}
		else if(ret>0){

			if (FD_ISSET(fd_mx_to_ins, &rset)>0){
				read(fd_mx_to_ins, &position_x, sizeof(int));
			}

			if (FD_ISSET(fd_mz_to_ins, &rset)>0){
				read(fd_mz_to_ins, &position_z, sizeof(int));
			}

			if (FD_ISSET(0,&rset)>0){

				read(0, &c_1, sizeof(char));

				if(c_1=='s'){

					printf("\n"BHRED "  EMERGENCY STOP" RESET "\n");
					fflush(stdout);
					kill(pid_motor_x, SIGUSR1);
					kill(pid_motor_z, SIGUSR1);
					kill(pid_command,SIGUSR1);
					fprintf(out, "Stop button pressed.\n");
				}
				if(c_1=='r'){

					printf("\n"BHYEL "  RESET" RESET "\n");
					fflush(stdout);
					kill(pid_motor_x,SIGUSR2);
					kill(pid_motor_z,SIGUSR2);
					kill(pid_command,SIGUSR2);
					fprintf(out, "Reset button pressed.\n");
				}
			}
		}

		if(position_x==0 && position_z==0){
			kill(pid_command, SIGUSR1);
		}

		printf("\r  La posizione lungo x è: %d m, La posizione lungo z è: %d m", position_x, position_z);
		fflush(stdout);
  		fflush(out);
	}
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
	return 0;
}
