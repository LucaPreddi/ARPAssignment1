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

void sighandler(int sig){
	if(sig==SIGUSR1){
		alarm(60);
	}
	if(sig==SIGALRM){  // per ora killa dopo 60 sec 
		kill(pid_motor_x, SIGKILL);
		kill(pid_motor_z,SIGKILL);
	}
}

int main(int argc, char *argv[])
{

	pid_motor_x = atoi(argv[1]);
	pid_motor_z = atoi(argv[2]);

	struct sigaction sa;
	memset(&sa,0,sizeof(sa));
	sa.sa_handler=&sighandler;
	sa.sa_flags=SA_RESTART;

	alarm(60);
	sigaction(SIGUSR1,&sa,NULL);
	sigaction(SIGALRM,&sa,NULL);

	while(1){


		sleep(1);


	}

	return 0;
}