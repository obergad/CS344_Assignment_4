/* Program Name: line_processor.c
** Author: Adam Oberg

Learning outcomes:

Describe what is mutual exclusion and why is it an important property to maintain when developing programs that may concurrently access shared resources (MLO 2)
Describe the API you can use to create threads and wait for the termination of a thread (MLO 4)
Describe what are condition variables and the API related to their use (MLO 4)

Thread 1, called the Input Thread, reads in lines of characters from the standard input.
Thread 2, called the Line Separator Thread, replaces every line separator in the input by a space.
Thread, 3 called the Plus Sign thread, replaces every pair of plus signs, i.e., "++", by a "^".
Thread 4, called the Output Thread, writes this processed data to standard output as lines of exactly 80 characters.
*/
#include <pthread.h>
#include <unistd.h>
#include <math.h> // must link with -lm
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <readline/readline.h>
#include <readline/history.h>
/*
A program with a pipeline of 3 threads that interact with each other as producers and consumers.
- Input thread is the first thread in the pipeline. It gets input from the user and puts it in a buffer it shares with the next thread in the pipeline.
- Square root thread is the second thread in the pipeline. It consumes items from the buffer it shares with the input thread. It computes the square root of this item. It puts the computed value in a buffer it shares with the next thread in the pipeline. Thus this thread implements both consumer and producer functionalities.
- Output thread is the third thread in the pipeline. It consumes items from the buffer it shares with the square root thread and prints the items.
A large chunk of this code is interpreted from  https://repl.it/@cs344/62threadexamplec
*/

#define NUM_THREADS 5

/*
This fucntion is just made to display the creator of the program.
Has no functionality
*/
void start_shell(){
    printf("\n******************"
    "************************************************");
    printf("\n\n\n\t****Multi-threaded Producer Consumer Pipeline****");
    printf("\n\n\t\t\t  -By: Adam Oberg-");
    printf("\n\n\n\n*******************"
    "**********************************************");
    char* username = getenv("USER");
    printf("\nUSER is: @%s", username);
    printf("\n");
    fflush(stdout);
    sleep(1);
}
//Function to exit the program.
void exitProgram(){
  int i = 0;
  printf("Exiting");
  fflush(stdout);
  while (i != 3) {
    printf(".");
    sleep(1);             //This is for the meme
    fflush(stdout);
    i++;
  }
  sleep(1);
  printf("\n");
  fflush(stdout);
  exit(0);

}
/*
 The function that each thread will call
*/
void* perform_work(void* argument){
  pthread_t pw_tid = pthread_self();
  int passed_in_value;
  // We cast the void* to int*, and then dereference the int* to get the int value the pointer is pointing to
  passed_in_value = *((int *) argument);
  printf("It's me, thread with argument %d!\n", passed_in_value);
  return NULL;
}

int main(void){
  pthread_t threads[NUM_THREADS];
  int thread_args[NUM_THREADS];
  int result_code, index;
  for (index = 0; index < NUM_THREADS; ++index) {
    // Create all threads one by one
    thread_args[index] = index;
    printf("In main: creating thread %d\n", index);
    result_code = pthread_create(&threads[index], NULL, perform_work, (void *) &thread_args[index]);
    assert(0 == result_code);
  }

  // Wait for each thread to complete
  for (index = 0; index < NUM_THREADS; ++index){
    result_code = pthread_join(threads[index], NULL);
    printf("In main: thread %d has completed\n", index);
    assert(0 == result_code);
  }
  printf("In main: All threads completed successfully\n");
  exit(EXIT_SUCCESS);
}
