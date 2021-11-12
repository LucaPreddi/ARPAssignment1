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

pid_t pid_command, pid_motor_x, pid_motor_z, pid_inspection, pid_wd;
char pid_motor_x_a[20], pid_motor_z_a[20], pid_command_a[20], pid_wd_a[20];


int spawn(const char * program, char ** arg_list) {
  pid_t child_pid = fork();
  if (child_pid != 0)
    return child_pid;

  else {
    execvp (program, arg_list);
    perror("exec failed");
    return 1;
  }
}

void create_fifo (const char * name){ 
    if(mkfifo(name, 0666)==-1){
        if (errno != EEXIST){
          perror("Error creating named fifo\n");
          exit (1);
        }
    }
}

int main() {

char * arg_list_1[] = { "./motor_x", "fifo_command_to_mot_x", NULL };
char * arg_list_2[] = { "./motor_z", "fifo_command_to_mot_z", NULL };
pid_motor_x = spawn("./motor_x", arg_list_1); 
pid_motor_z = spawn("./motor_z", arg_list_2);

sprintf(pid_motor_x_a, "%d", pid_motor_x);
sprintf(pid_motor_z_a, "%d", pid_motor_z);

char * arg_list_5[] = {"./watchdog",pid_motor_x_a,pid_motor_z_a, NULL, NULL };
pid_wd = spawn("./watchdog", arg_list_5);

sprintf(pid_wd_a, "%d", pid_wd);

char * arg_list_3[] = { "/usr/bin/konsole",  "-e", "./command", pid_wd_a , (char*)NULL };
pid_command = spawn("/usr/bin/konsole", arg_list_3);
sprintf(pid_command_a, "%d", pid_command);

char * arg_list_4[] = { "/usr/bin/konsole",  "-e", "./inspection", pid_motor_x_a, pid_motor_z_a, (char*)NULL };
pid_inspection = spawn("/usr/bin/konsole", arg_list_4);


create_fifo("fifo_command_to_mot_x");
create_fifo("fifo_command_to_mot_z");
create_fifo("fifo_est_pos_x");
create_fifo("fifo_est_pos_z");

wait(NULL);

fflush(stdout);
unlink("fifo_command_to_mot_x");
unlink("fifo_command_to_mot_z");
unlink("fifo_est_pos_x");
unlink("fifo_est_pos_z");
return 0;

}