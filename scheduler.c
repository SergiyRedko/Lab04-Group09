#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include "job.h"

// This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;

/*Function to append a new job to the list.*/
void append(int id, int arrival, int length)
{
  // create a new struct and initialize it with the input data
  struct job *tmp = (struct job *)malloc(sizeof(struct job));

  // tmp->id = numofjobs++;
  tmp->id = id;
  tmp->length = length;
  tmp->arrival = arrival;
  tmp->executionStarted = -1;
  tmp->executionEnded = -1;

  // the new job is the last job
  tmp->next = NULL;

  // Case: job is first to be added, linked list is empty
  if (head == NULL)
  {
    head = tmp;
    return;
  }

  struct job *prev = head;

  // Find end of list
  while (prev->next != NULL)
  {
    prev = prev->next;
  }

  // Add job to end of list
  prev->next = tmp;
  return;
}

/*Function to read in the workload file and create job list.*/
void read_workload_file(char *filename)
{
  int id = 0;
  FILE *fp;
  size_t len = 0;
  ssize_t read;
  char *line = NULL,
       *arrival = NULL,
       *length = NULL;

  struct job **head_ptr = malloc(sizeof(struct job *));

  if ((fp = fopen(filename, "r")) == NULL)
    exit(EXIT_FAILURE);

  while ((read = getline(&line, &len, fp)) > 1)
  {
    arrival = strtok(line, ",\n");
    length = strtok(NULL, ",\n");

    // Make sure neither arrival nor length are null.
    assert(arrival != NULL && length != NULL);

    append(id++, atoi(arrival), atoi(length));
  }

  fclose(fp);

  // Make sure we read in at least one job
  assert(id > 0);

  return;
}

/*Function to print out the list of jobs and currently available information.*/
const void printJobs(){
  struct job * someJob = head;
  printf("\nJob List:\n");
  while(someJob != NULL){
    printf("Job %d: arr %d, len %d, %d >>> %d.\n",
    someJob->id, someJob->arrival, someJob->length, someJob->executionStarted, someJob->executionEnded);
    someJob = someJob->next;
  }
  printf("\n");
}

int min(int a, int b){
  if(a < b)
    return a;
  else
    return b;
}

int max(int a, int b){
  if(a > b)
    return a;
  else
    return b;
}

void policy_FIFO(struct job *head)
{
  printf("Execution trace with FIFO:\n");

  int nextValidTime = 0;
  struct job * someJob = head;
  while(someJob != NULL){
    (*someJob).executionStarted = max(nextValidTime, someJob->arrival);
    (*someJob).executionEnded = someJob->executionStarted + someJob->length;
    nextValidTime = someJob->executionEnded;

    printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n",
            someJob->executionStarted, someJob->id, someJob->arrival, someJob->length);

    someJob = someJob->next;
  }

  printf("End of execution widht FIFO.\n");
  return;
}

struct job * findShortestAvailableJob(int timeLimit){
  struct job * result = NULL;
  struct job * jobIterator = head;

  // Scan through the list and find the shortest job that is already available.
  while(jobIterator != NULL){
    if(jobIterator->arrival <= timeLimit // No point checking this job otherwise. It hasn't arrived yet.
       && jobIterator->executionStarted == -1){ //I.e., job hasn't been scheduled yet.

      if(result == NULL) // Anything will do to begin with.
        result = jobIterator;
      else if(jobIterator->length < result->length) // If a shorter job is found.
        result = jobIterator;
    }

    jobIterator = jobIterator->next;
  }

  return result;
}

int findNextJobsTime(){
  struct job * jobIterator = head;
  int nextJobsTime = -1;

  // Scan through the list and find the shortest job that is already available.
  while(jobIterator != NULL){
    if(jobIterator->executionStarted == -1){ //I.e., job hasn't been scheduled yet.

      if(nextJobsTime == -1) // Anything will do to begin with.
        nextJobsTime = jobIterator->arrival;
      else if(jobIterator->arrival < nextJobsTime) // If a shorter job is found.
        nextJobsTime = jobIterator->arrival;
    }

    jobIterator = jobIterator->next;
  }

  return nextJobsTime;
}

void policy_SJF(struct job * head){
  printf("Execution trace with SJF:\n");

  int nextValidTime = 0;

  // Keep scanning through jobs continuously.
  while(1){
    struct job * nextJob = findShortestAvailableJob(nextValidTime);

    if(nextJob == NULL){ // Processor is idling, since there are no jobs to execute at this time. Move up the timeline.
      printf("Processor is idling.\n");

      nextValidTime = findNextJobsTime();

      if(nextValidTime == -1) // All jobs have executed.
        break;
      else // If this executes, we know that at least one job will be found.
        nextJob = findShortestAvailableJob(nextValidTime);
    }

    // Now that we have our next job, asisgn it.

    (*nextJob).executionStarted = nextValidTime;
    (*nextJob).executionEnded = nextJob->executionStarted + nextJob->length;
    nextValidTime = nextJob->executionEnded;

    printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n",
            nextJob->executionStarted, nextJob->id, nextJob->arrival, nextJob->length);
  }

  printf("End of execution widht SJF.\n");
  return;
}

void analyze(struct job * head){
  float averageResponseTime = 0;
  float averageTurnaroundTime = 0;
  int numJobs = 0;
  
  struct job * jobIterator = head;

  // Scan through the list and print statistics for each job.
  while(jobIterator != NULL){
    numJobs++;
    int responseTime = jobIterator->executionStarted - jobIterator->arrival;
    int turnaroundTime = jobIterator->executionEnded - jobIterator->arrival;

    averageResponseTime += responseTime;
    averageTurnaroundTime += turnaroundTime;

    printf("Job %d -- Response time: %d Turnaround: %d Wait: %d\n",jobIterator->id , responseTime, turnaroundTime, responseTime);

    jobIterator = jobIterator->next;
  }

  averageResponseTime /= numJobs;
  averageTurnaroundTime /= numJobs;

  printf("Average -- Response: %.2f Turnaround %.2f Wait %.2f\n", averageResponseTime, averageTurnaroundTime, averageResponseTime);
  
  return;
}

void freeJobList(){
  struct job * jobIterator = head;

  while(jobIterator != NULL){
    struct job * doomedJob = jobIterator;
    jobIterator = jobIterator->next;
    free(doomedJob);
  }
}

int main(int argc, char **argv)
{

  if (argc < 4)
  {
    fprintf(stderr, "missing variables\n");
    fprintf(stderr, "usage: %s analysis-flag policy workload-file\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int analysis = atoi(argv[1]);
  char *policy = argv[2],
       *workload = argv[3];

  // Note: we use a global variable to point to
  // the start of a linked-list of jobs, i.e., the job list
  read_workload_file(workload);

  //printJobs();

  if (strcmp(policy, "FIFO") == 0 || strcmp(policy, "fifo") == 0)
  {
    policy_FIFO(head);
    if (analysis)
    {
      printf("Begin analyzing FIFO:\n");
      analyze(head);
      printf("End analyzing FIFO.\n");
    }
  }
  else if (strcmp(policy, "SJF") == 0 || strcmp(policy, "sjf") == 0)
  {
    policy_SJF(head);
    if (analysis){
      printf("Begin analyzing SJF:\n");
      analyze(head);
      printf("End analyzing SJF.\n");
    }
  }

  //printJobs();

  freeJobList();

  exit(EXIT_SUCCESS);
}