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
  printf("Job List:\n");
  while(someJob != NULL){
    printf("Job %d: arrived at %d, length is %d, execution started at %d, and ended at %d.\n",
    someJob->id, someJob->arrival, someJob->length, someJob->executionStarted, someJob->executionEnded);
    someJob = someJob->next;
  }
}

void policy_FIFO(struct job *head)
{
  // TODO
  printf("analyze_SJF is not implemented yet.\n");
  return;
}

void analyze_FIFO(struct job *head)
{
  // TODO
  printf("analyze_SJF is not implemented yet.\n");
  return;
}

void policy_SJF(struct job * head){
  // TODO
  printf("policy_SJF is not implemented yet.\n");
  return;
}

void analyze_SJF(struct job * head){
  // TODO
  printf("analyze_SJF is not implemented yet.\n");
  return;
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

  printJobs();

  if (strcmp(policy, "FIFO") == 0 || strcmp(policy, "fifo") == 0)
  {
    policy_FIFO(head);
    if (analysis)
    {
      printf("Begin analyzing FIFO:\n");
      analyze_FIFO(head);
      printf("End analyzing FIFO.\n");
    }
    exit(EXIT_SUCCESS);
  }
  else if (strcmp(policy, "SJF") == 0 || strcmp(policy, "sjf") == 0)
  {
    policy_SJF(head);
    if (analysis){
      printf("Begin analyzing SJF:\n");
      analyze_SJF(head);
      printf("End analyzing SJF.\n");
    }
    exit(EXIT_SUCCESS);
  }

  // TODO: Add other policies
  // TODO: free memory (allocated for linked list)

  exit(EXIT_SUCCESS);
}