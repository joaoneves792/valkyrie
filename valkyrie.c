#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <signal.h>

pid_t forked_pid;

void term(int signum){
		kill(-(forked_pid), signum); //Pass the signal to all children
}

int main(int argc, char** argv){
  //Set this process as child subreaper
	prctl(PR_SET_CHILD_SUBREAPER, 1, NULL, NULL, NULL);

  //Register signal handler
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term;
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGINT, &action, NULL);

  //Fork and exec
	if(!(forked_pid = fork())){
		setpgid(0, 0); // Place the forked process in a new group
		execv(argv[1], argv+1);
	}

  //Wait for child termination
	int returnStatus = -1;
	pid_t terminated_pid = 0;
	while(1){
		while( (terminated_pid = waitpid(-1, &returnStatus, WNOHANG)) > 0 ){
			if(terminated_pid == forked_pid){ //If we got the immediate child
				kill(-(forked_pid), SIGKILL); //Atempt to kill all remaining grandchildren
				sleep(5); //Make sure all grandchildren are reparented to us
				while( waitpid(-1, &returnStatus, WNOHANG) > 0);
				return 0;
			}
		}
		sleep(5);
	}
}
