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

pid_t pid_motor_x;
pid_t pid_motor_z;
pid_t pid_command;

// Defining CHECK() tool. We use this error checking method to make the
// code lighter and more fancy, using errno.

#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n",__LINE__,strerror(errno)); exit(-1);-1;}) : __val); })


void sighandler(int sig){
	if(sig==SIGUSR1){
		alarm(60);
	}
	if(sig==SIGALRM){  // per ora killa dopo 60 sec 

		CHECK(kill(pid_command, SIGUSR2));
		CHECK(kill(pid_motor_x, SIGUSR2));
		CHECK(kill(pid_motor_z, SIGUSR2));
		
	}
}

int main(int argc, char *argv[])
{	

	int fd_c_to_wd;

	fd_c_to_wd = CHECK(open(argv[1], O_RDONLY));
	pid_motor_x = atoi(argv[2]);
	pid_motor_z = atoi(argv[3]);

	struct sigaction sa;
	memset(&sa,0,sizeof(sa));
	sa.sa_handler=&sighandler;
	sa.sa_flags=SA_RESTART;

	alarm(60);
	sigaction(SIGUSR1,&sa,NULL);
	sigaction(SIGALRM,&sa,NULL);

	CHECK(read(fd_c_to_wd, &pid_command, sizeof(int)));
	CHECK(close(fd_c_to_wd));
	
	while(1){

		sleep(1);

	}

	return 0;
}