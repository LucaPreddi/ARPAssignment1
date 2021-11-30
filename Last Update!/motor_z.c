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

// Defining CHECK() tool. We use this error checking method to make the
// code lighter and more fancy, using errno.

#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n",__LINE__,strerror(errno)); exit(-1);-1;}) : __val); })


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
		reset = false;
		value=6;
	}
	if(sig==SIGUSR2){

		reset = true;
	}
}

int main(int argc, char * argv[]){

	int fd_c_to_mz, fd_mz_to_ins;
	int ret;
	int err;

	struct timeval tv={0,0};

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler=&sighandler;
	sa.sa_flags=SA_RESTART;
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);

	fd_set rset;

	while(1){

		// Opening FIFOs.

		fd_c_to_mz = CHECK(open(argv[1],O_RDONLY));
		fd_mz_to_ins = CHECK(open(argv[2], O_WRONLY));

		err = (rand()%(1)) - (rand()%(1));

		FD_ZERO(&rset);
        FD_SET(fd_c_to_mz, &rset);
        ret = select(FD_SETSIZE, &rset, NULL, NULL, &tv);

		if(ret>=0){
			if (FD_ISSET(fd_c_to_mz, &rset)>0){
				CHECK(read(fd_c_to_mz, &value, sizeof(int)));
			}
		}

		switch(reset){

			case true: //reset running

				if(position>step){
					position -= step+err;
					usleep(10000);
					CHECK(write(fd_mz_to_ins, &position, sizeof(int)));		
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
						//value = 0;
					break;
				}

			if (position>6000) position=6000;
			if (position<0) position=0;
			
			CHECK(write(fd_mz_to_ins, &position, sizeof(int)));		

			break;
		}
	
	CHECK(close(fd_mz_to_ins));
	CHECK(close(fd_c_to_mz));			  
	}
}