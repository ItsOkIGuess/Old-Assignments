#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <string.h>
#include <regex.h>
#define MAX_INPUT_SIZE 80
#define MAX_NUM_TOKENS 7

/*
CSC 360 OPERATING SYSTEMS ASSIGNMENT #1
ROMNEY DUNCAN 
V#00895553
*/




/*
Function commandcheck looks in .sh360 file to see if the commands are valid
and converts the command into the full directory for the command ie ls -> /bin/ls
returns 1 on success, and -1 on failure
*/
int commandcheck(char *x, FILE *fp){
	int value;
	char line[MAX_INPUT_SIZE];
	char* temp; 
	while (fgets(line, MAX_INPUT_SIZE, fp) != NULL) {
		int length = strlen(line);
		
		if (line[length-1] == '\n'){
			line[length-1]  = '\0';
			}
		
		strcat(line,x);
		length = strlen(line);
		if (line[length-1] == '\n'){
			line[length-1]  = '\0';
			}
			if(access(line, X_OK) == 0){
				printf("here?");
				strcpy(x,line);
				return 1;
			}
			//close(fp);
			
		
		}		
    return -1;
	}
	



int main(int argc, char *argv[]){
	
	/*
	Setting Up Initial Variables 
	*/
	
	FILE *fp, *fpH, *fpT;
    char *t;
	int pid, fd, pid_head, pid_tail, pid_mid;
	int status;
	char *argsA[MAX_NUM_TOKENS][MAX_INPUT_SIZE];
	char *envp[] = { 0 };
	int block = 0;
	int fdd[2];
	int ft[3];
	
	printf("*****\n");
	
	/*******************************************************************************
	CODE SECTION 1: MODIFIED FROM APENDICIES A,E 
	This section of code does all the nessicary string parsing to get the commands
	check whether the formatting is valid, and if it is a OR, or PP command	
	*/
	
	//START PRIMARY LOOP
	for(;;){
		
	int valid = -1;
	fflush(stdout);
	printf("uvic %% ");
	
	char cmd[MAX_INPUT_SIZE];
	fgets(cmd, MAX_INPUT_SIZE, stdin);
	if (cmd[strlen(cmd) - 1] == '\n') {
        cmd[strlen(cmd) - 1] = '\0';
        }
	if(strcmp(cmd, "exit") == 0){
		exit(0);
	}
	
//Creating various Char arrays
	int num_tokens = 0;
    t = strtok(cmd, " ");
    char *argsA[MAX_NUM_TOKENS];
	char *argsAOR[MAX_NUM_TOKENS];
	char *argsAPPH[MAX_NUM_TOKENS];
	char *argsAPPT[MAX_NUM_TOKENS];
	char *argsAPPE[MAX_NUM_TOKENS];
	char* filename = NULL;

//Three file opens since I don't want multiple piped children using the same file

		fp = fopen(".sh360rc", "r");
	if (fp == NULL){
		printf("no file to open\n");
		exit(1);
	}
	
		fpH = fopen(".sh360rc", "r");
	if (fp == NULL){
		printf("no file to open\n");
		exit(1);
	}
		fpT = fopen(".sh360rc", "r");
	if (fp == NULL){
		printf("no file to open\n");
		exit(1);
	}
	
	
	
	//Getting CMD line args into appropriate Array
	while (num_tokens < MAX_NUM_TOKENS) {
		argsA[num_tokens] = t;
		num_tokens++;
        t = strtok(NULL, " ");
    }
	num_tokens = 0;
	
	
	//Filling Arrays with null so no garbage gets executed
	while (num_tokens < MAX_NUM_TOKENS) {
		argsAOR[num_tokens] = NULL;
		argsAPPH[num_tokens] = NULL;
		argsAPPT[num_tokens] = NULL;
		argsAPPE[MAX_NUM_TOKENS];
		num_tokens++;
	}
	
	
	//Formating PP arrays
		if(strcmp(argsA[0], "PP") == 0){
			block = 2;
			for(int i = 1;i<MAX_NUM_TOKENS;i++){
				if(argsA[i] != NULL){
					if(strcmp(argsA[i], "->") ==0){
						i++;
						int j = 0;
						while(i<MAX_NUM_TOKENS){
							if(argsA[i] != NULL){
								if(strcmp(argsA[i], "->") ==0){
									i++;
									int k = 0;
									block = 3;
									while(i<MAX_NUM_TOKENS){
										argsAPPE[k] = argsA[i];
										k++;
										i++;
										}
									break;
								}
							}
								argsAPPT[j] = argsA[i];
								j++;
								i++;
							
						}
						break;
					}
					argsAPPH[i-1] = argsA[i];
				}
			}
			
			
			
			
			//Checking For any input errors
			if(argsAPPH[0] == NULL){
				fprintf(stderr, "Missing command before ->\n");
				block = -1;
			}
			
			if(argsAPPT[0] == NULL){
				fprintf(stderr, "Missing ->, or Command after\n");
				block = -1;
			}
		}
		
		if(strcmp(argsA[0], "OR") ==0){
			block = 1;
			for(int i = 0;i<MAX_NUM_TOKENS;i++){
				if(argsA[i] != NULL){
					if(strcmp(argsA[i], "->") ==0){
						filename = argsA[i+1];
					}
				}
			}
			
			if(filename == NULL){
			fprintf(stderr, "Missing ->, or file\n");
			block = -1;
			}
		}
		
		
		
	/****************************************************************************
	CODE SECTION 2: MODIFIED FROM APENDICIES B,C,D
	This section sets up and runs all the forks, whichever block is ran is decided by parsing above
	Each fork runs commandcheck to ensure the command is valid
	If the command is invalid it will output Invalid Command
	*/
	
	
	
	//Generic command
	if(block == 0){	
		if ((pid = fork()) == 0){
			
			char command[MAX_INPUT_SIZE];
			strcpy(command,argsA[0]);
			valid = commandcheck(command,fp);
			argsA[0] = command;
			fclose(fp);
		
		if(valid == -1){
			printf("Invalid command\n");
			exit(1);
		}
			
			execve(argsA[0],argsA,envp);
			printf("Invalid Command.\n");
			exit(1);
		}
		
		waitpid(pid,&status,0);
	}
	
	
	
	//File Output
	else if(block == 1){
		if((pid = fork()) == 0){
			
			//Formating ARGSAOR array
			for(int i = 1;i<MAX_NUM_TOKENS;i++){
				if(argsA[i] != NULL){
				if(strcmp(argsA[i], "->") ==0){
						break;
						}
					}
				argsAOR[i-1] = argsA[i];
				}
			
			char command[MAX_INPUT_SIZE];
			strcpy(command,argsAOR[0]);
			valid = commandcheck(command,fp);
			argsAOR[0] = command;
			fclose(fp);
			
			if(valid == -1){
				printf("Invalid command\n");
				exit(1);
			}
			
			fd = open(filename, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
		if (fd == -1) {
			fprintf(stderr, "cannot open File for writing\n");
			exit(1);
			}
			dup2(fd, 1);
			dup2(fd, 2); 
			execve(argsAOR[0], argsAOR, envp);
			printf("child: SHOULDN'T BE HERE.\n");
		}
		
		waitpid(pid,&status,0);
	}
	
	
	
	//Single Pipe
	else if(block == 2){
		pipe(fdd);
		if ((pid_head = fork()) == 0) {
			//Pipe Managment
			dup2(fdd[1], 1);
			close(fdd[0]);
			
			//Command Managment
			char command[MAX_INPUT_SIZE];
			strcpy(command,argsAPPH[0]);
			valid = commandcheck(command,fpH);
			argsAPPH[0] = command;
			fclose(fpH);
		
			if(valid == -1){
				printf("Invalid command1\n");
				exit(1);
			}
			
			//Execution
			execve(argsAPPH[0], argsAPPH, envp);
			fprintf(stdout, "child (head): SHOULDN'T BE HERE.\n");
			exit(1);
		}

		if ((pid_tail = fork()) == 0) {
			
			//Pipe Managment
			dup2(fdd[0], 0);
			close(fdd[1]);
		
			//Command Managment
			char command[MAX_INPUT_SIZE];
			strcpy(command,argsAPPT[0]);
			valid = commandcheck(command,fpT);
			argsAPPT[0] = command;
			fclose(fpT);
			
			if(valid == -1){
				printf("Invalid command2\n");
				exit(1);
			}
			
			//Execution
			execve(argsAPPT[0], argsAPPT, envp);
			fprintf(stdout, "child (tail): SHOULDN'T BE HERE.\n");
			exit(1);
		}	
		
		//Cleaning Pipes
	    close(fdd[0]);
		close(fdd[1]);		
		
		//Executing Children
		waitpid(pid_head, &status, 0);
		waitpid(pid_tail, &status, 0);
	}



	//Double Pipe	
 	else if(block == 3){
		pipe(fdd);
		pipe(ft);
		
	if ((pid_head = fork()) == 0) {
			
			//Pipe Managment
			close(fdd[0]);
			close(ft[0]);
			close(ft[1]);
			dup2(fdd[1], 1);
			
			//Command Managment
			char command[MAX_INPUT_SIZE];
			strcpy(command,argsAPPH[0]);
			valid = commandcheck(command,fpH);
			argsAPPH[0] = command;
			fclose(fpH);
		
			if(valid == -1){
				printf("Invalid command\n");
				exit(1);
			}
			//Execution
			execve(argsAPPH[0], argsAPPH, envp);
			fprintf(stdout, "child (head): SHOULDN'T BE HERE.\n");
			exit(1);
		}
	
		if ((pid_mid = fork()) == 0) {
			//Pipe Managment
			close(fdd[1]);
			dup2(fdd[0], 0);
			close(ft[0]);
			dup2(ft[1],1);
			
			//Command Managment
			char command[MAX_INPUT_SIZE];
			strcpy(command,argsAPPT[0]);
			valid = commandcheck(command,fpT);
			argsAPPT[0] = command;
			fclose(fpT);
			
			if(valid == -1){
				printf("Invalid command\n");
				exit(1);
			}
			
			//Execution
			execve(argsAPPT[0], argsAPPT, envp);
			fprintf(stdout, "child (tail): SHOULDN'T BE HERE.\n");
			exit(1);
		}
		
		if ((pid_tail = fork()) == 0) {
			
			//Pipe Managment
			close(fdd[0]);
			close(fdd[1]);
			close(ft[1]);
			dup2(ft[0], 0);
			
			//Command Managment
			char command[MAX_INPUT_SIZE];
			strcpy(command,argsAPPE[0]);
			valid = commandcheck(command,fp);
			argsAPPE[0] = command;
			fclose(fp);
			
			if(valid == -1){
				printf("Invalid command\n");
				exit(1);
			}
		
			//Execution
			execve(argsAPPE[0], argsAPPE, envp);
			fprintf(stdout, "child (end): SHOULDN'T BE HERE.\n");
			exit(1);
		}
		
		//Clearing Pipes
		close(fdd[0]);
		close(fdd[1]);		
		close(ft[0]);
		close(ft[1]);
		
		//Executing Children
		waitpid(pid_head, &status, 0);
		waitpid(pid_mid, &status, 0);
		waitpid(pid_tail, &status, 0);
	}
	
	else{
		printf("Invalid Command\n");
	}
		
	block = 0;
	}
	
	return 1;
	
	
	
}
