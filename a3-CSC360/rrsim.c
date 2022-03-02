#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include "linkedlist.h"
#define MAX_BUFFER_LEN 80

taskval_t *event_list = NULL;

/*
Round Robin CPU Scheduler SimSave
For CSC 360 Assignment 3
By Romney Duncan
V#00895553
*/




void print_task(taskval_t *t, void *arg) {
    printf("task %03d: %5d %3.2f %3.2f\n",
        t->id,
        t->arrival_time,
        t->cpu_request,
        t->cpu_used
    );  
}


void increment_count(taskval_t *t, void *arg) {
    int *ip;
    ip = (int *)arg;
    (*ip)++;
}

/*
get_save function:
opens the savefile, or creates a new one if none exists
gets the current number in the save, and replaces it with number + 1
returns the number for the next file to output to
*/


int get_save(){

	FILE *save;
	int num = 0;

	save = fopen("SimSave.txt", "r+");
	if(save == NULL){
		save = fopen("SimSave.txt", "w+");
		fclose(save);
		save = fopen("SimSave.txt", "r+");
	}
	
	fscanf(save,"%d",&num);
	num++;
	
	printf("This simulation will output to Sim_%d.txt\n",num);
	
	freopen(NULL,"w+",save);
	fprintf(save,"%d\n",num);
	fclose(save);
	
	return(num);
}

/*
check_arrival function:
Called after every simulated CPU tick
Checks if the next task in the event_list is ready to be added to the ready_q
If it is it removes the task from the event_list and adds it to the ready_q and sets arrival_tick to its next value
*/

taskval_t* check_arrival(int curr_tick, int* arrival_tick, taskval_t *t){
	taskval_t *curr;
	taskval_t *temp =  new_task();

	if(curr_tick >= *arrival_tick){
		if(peek_front(event_list) != NULL){
		curr = peek_front(event_list);
		temp->arrival_time = curr->arrival_time;
		temp->id = curr->id;
		temp->cpu_request = curr->cpu_request;
		temp->cpu_used = curr->cpu_used;
		temp->finish_time = curr->finish_time;
		t = add_end(t,temp);
		if(t == NULL){
			printf("looks like i need to return T\n");
		}
		event_list = remove_front(event_list);
		end_task(curr);
		if(peek_front(event_list) != NULL){
			curr = peek_front(event_list);
			*arrival_tick = curr->arrival_time;
			return t;
			}
		}
		return t;
	}
	return t;
}

/*
run_simulation function:
Runs the simulated RR CPU scheduler
Other than some variable, and output file setup this functions as a large loop
That lasts as long as there are tasks in either the ready_q, or the event_list
*/

void run_simulation(int qlen, int dlen) {
	FILE *output;
	
	int savenum = get_save();
    int cpu_counter = 0;

	taskval_t *curr_task = peek_front(event_list);
	taskval_t *ready_q = NULL; 	

	int next_arrival = curr_task->arrival_time;
	int start_counter = curr_task->arrival_time;

	float cycles_req = curr_task->cpu_request;
	float cycles_used = curr_task->cpu_used;

	int id = curr_task->id;

	bool gravy = true;

	char ch[16];
	char filename[30] = "Sim_";
	
	sprintf(ch,"%d",savenum);
	strcat(filename,ch);
	strcat(filename,".txt");
	printf("%s\n",filename);
	output = fopen(filename, "w+");
	
	//Primary Loop
	while(true){
		//End Condition
		if(peek_front(ready_q)== NULL && peek_front(event_list) == NULL){
			break;
		}
		//Idle Loop
		while(cpu_counter < next_arrival && ready_q == NULL){
			//No Idea why this is here, but I don't want to risk changing it		
			if(ready_q != NULL){
				return;
			}
			
			printf("[%05d] IDLE\n", cpu_counter);
			cpu_counter++;
			ready_q = check_arrival(cpu_counter, &next_arrival, ready_q);
		}
		
		//Dispatch and Cycle Loop
		if(cpu_counter >= start_counter){
			//Dispatcher
			for(int i = 0; i <dlen;i++){
				printf("[%05d] DISPATCHING\n",cpu_counter);
				cpu_counter++;
				ready_q = check_arrival(cpu_counter,&next_arrival,ready_q);
				
			}
			
			for(int j = 0; j <qlen;j++){
				//Exit Case for a task
				if(cycles_used >= cycles_req){
					curr_task->finish_time = cpu_counter + (cycles_req - cycles_used);
					float TAT = curr_task->finish_time*1.0 - curr_task->arrival_time*1.0 + 1;
					float WT = TAT - cycles_req*1.0;
					printf("[%05d] id=%05d EXIT w=%1.2f ta=%1.2f\n",cpu_counter,id,WT,TAT);
					//Outputing result to file
					fprintf(output,"[%05d] id=%05d EXIT w=%1.2f ta=%1.2f\n",cpu_counter,id,WT,TAT);
					gravy = false;
					end_task(curr_task);
					ready_q = remove_front(ready_q);
					break;
					
					}
				//Normal Case for a task
				printf("[%05d] id=%05d req=%1.2f used=%1.2f\n",cpu_counter,id,cycles_req,cycles_used);
				cycles_used++;
				cpu_counter++;
				ready_q = check_arrival(cpu_counter,&next_arrival,ready_q);
				gravy = true;
				
				}
				
			
		}
		//Adding current task to end of list, and removing it from the front
		if(gravy){
			curr_task->cpu_used = cycles_used;
			ready_q = remove_front(ready_q);
			ready_q = add_end(ready_q,curr_task);
			}
		
		//Setting current task to the front of the list
		if(peek_front(ready_q)!= NULL){
			curr_task = peek_front(ready_q);
			}
		//Setting the variables to the current task
		start_counter = curr_task->arrival_time;
		cycles_req = curr_task->cpu_request;
		cycles_used = curr_task->cpu_used;
		id = curr_task->id;
		//End of CPU Scheduler Loop	
		}
		
	fclose(output);
	//End Of Function
}

//Only Thing I changed here was I made it to the program ends if no tasks are added before run_simulation is called

int main(int argc, char *argv[]) {
    char   input_line[MAX_BUFFER_LEN];
    int    i;
    int    task_num;
    int    task_arrival;
    float  task_cpu;
    int    quantum_length = -1;
    int    dispatch_length = -1;

    taskval_t *temp_task;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--quantum") == 0 && i+1 < argc) {
            quantum_length = atoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "--dispatch") == 0 && i+1 < argc) {
            dispatch_length = atoi(argv[i+1]);
        }
    }

    if (quantum_length == -1 || dispatch_length == -1) {
        fprintf(stderr, 
            "usage: %s --quantum <num> --dispatch <num>\n",
            argv[0]);
        exit(1);
    }


    while(fgets(input_line, MAX_BUFFER_LEN, stdin)) {
        sscanf(input_line, "%d %d %f", &task_num, &task_arrival,
            &task_cpu);
        temp_task = new_task();
        temp_task->id = task_num;
        temp_task->arrival_time = task_arrival;
        temp_task->cpu_request = task_cpu;
        temp_task->cpu_used = 0.0;
        event_list = add_end(event_list, temp_task);
    }

#ifdef DEBUG
    int num_events;
    apply(event_list, increment_count, &num_events);
	if(num_events == 0){
		printf("No Events Scheduled Quiting Program\n");
		return(1);
	}
    printf("DEBUG: # of events read into list -- %d\n", num_events);
    printf("DEBUG: value of quantum length -- %d\n", quantum_length);
    printf("DEBUG: value of dispatch length -- %d\n", dispatch_length);
#endif

    run_simulation(quantum_length, dispatch_length);
	
    return (0);
}
