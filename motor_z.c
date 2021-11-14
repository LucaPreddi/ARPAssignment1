#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

typedef enum {
	false,
	true
}bool;

bool reset = false;

int value;

int position=0;
int step=1;

void sighandler(int sig){
	if(sig==SIGUSR1){
		value=6;
	}
	if(sig==SIGUSR2){

		reset = true;
	}
}

int main(int argc, char * argv[]){

	int fd_c_to_mz, fd_mz_to_ins;
	int ret;
	int err = (rand()%(5)) - (rand()%(5));

	struct timeval tv={0,0};

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler=&sighandler;
	sa.sa_flags=SA_RESTART;
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);

	fd_set rset;
	fd_c_to_mz = open(argv[1],O_RDONLY);
	fd_mz_to_ins = open(argv[2], O_WRONLY);

	if(fd_c_to_mz == -1){
		printf("Error opening command to motor z fifo!");
		return(5);
	}
	if(fd_mz_to_ins == -1){
		printf("Error opening motor z to inspection fifo!");
		return(5);
	}

	while(1){

		FD_ZERO(&rset);
        FD_SET(fd_c_to_mz, &rset);
        ret=select(FD_SETSIZE, &rset, NULL, NULL, &tv);

		if(ret==-1){
			printf("There's an error opening the fifo. MOTOR Z\n");
			fflush(stdout);
		}
		else if(ret>=0){
			if (FD_ISSET(fd_c_to_mz, &rset)>0){
				read(fd_c_to_mz, &value, sizeof(int));
			}
		}

		switch(reset){

			case true: //reset running

				if(position>step){

					if(value==6){
						
						reset = false;
						value = 0;
					}
		

					position -= step+err;
					usleep(10000);
					write(fd_mz_to_ins, &position, sizeof(int));		
				}

				if (position<=step){
					position=0;
					value = 6;
					reset = false;
				}

			break;


			case false:

				switch(value){

					case 4:
						if (position>=6000){
						}
						else{
							position+= step+err;
						}
						usleep(10000);

					break;

					case 5:

						if (position<=0){
						}
						else {
							position-=step+err;
						}
						usleep(10000);

					break;

					case 6:
						usleep(10000);
						value = 0;
					break;
				}

			if (position>6000) position=6000;
			if (position<0) position=0;
			write(fd_mz_to_ins, &position, sizeof(int));		

			break;
		}
							  
	}
	return 0;
}