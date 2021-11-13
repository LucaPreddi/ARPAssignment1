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

FILE *out = fopen("debug.txt", "w");
fprintf(out, "################# DEBUG FILE ################# \n\n");
fflush(out);

char * f_com_to_mx = "/tmp/fifo_command_to_mot_x";
char * f_com_to_mz = "/tmp/fifo_command_to_mot_z";
char * f_e_pos_x = "/tmp/fifo_est_pos_x";
char * f_e_pos_z = "/tmp/fifo_est_pos_z";
char * f_com_to_wd = "/tmp/fifo_command_to_wd";
char * f_com_to_ins = "/tmp/fifo_command_to_ins";

char * arg_list_1[] = { "./motor_x", f_com_to_mx ,f_e_pos_x, NULL };
char * arg_list_2[] = { "./motor_z", f_com_to_mz,f_e_pos_z, NULL };
pid_motor_x = spawn("./motor_x", arg_list_1); 
pid_motor_z = spawn("./motor_z", arg_list_2);

sprintf(pid_motor_x_a, "%d", pid_motor_x);
sprintf(pid_motor_z_a, "%d", pid_motor_z);

char * arg_list_5[] = {"./watchdog",f_com_to_wd,pid_motor_x_a,pid_motor_z_a, NULL, NULL };
pid_wd = spawn("./watchdog", arg_list_5);

sprintf(pid_wd_a, "%d", pid_wd);

char * arg_list_3[] = { "/usr/bin/konsole",  "-e", "./command", pid_wd_a,
                        f_com_to_ins,f_com_to_mx, 
                        f_com_to_mz,f_com_to_mz,
                        (char*)NULL };

pid_command = spawn("/usr/bin/konsole", arg_list_3);

char * arg_list_4[] = { "/usr/bin/konsole",  "-e", "./inspection", pid_motor_x_a, pid_motor_z_a,
                        f_com_to_ins,f_e_pos_x,f_e_pos_z,(char*)NULL };

pid_inspection = spawn("/usr/bin/konsole", arg_list_4);


create_fifo(f_com_to_mx);
create_fifo(f_com_to_mz);
create_fifo(f_e_pos_x);
create_fifo(f_e_pos_z);
create_fifo(f_com_to_ins);
create_fifo(f_com_to_wd);

wait(NULL);

fclose(out);

fflush(stdout);
unlink(f_com_to_mx);
unlink(f_com_to_mz);
unlink(f_e_pos_x);
unlink(f_e_pos_z);
unlink(f_com_to_ins);
unlink(f_com_to_wd);
return 0;

}