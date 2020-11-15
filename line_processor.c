#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h> // must link with -lm
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <assert.h>
#include <stddef.h>




/*
A program with a pipeline of 3 threads that interact with each other as producers and consumers.
- Input thread is the first thread in the pipeline. It gets input from the user and puts it in a buffer it shares with the next thread in the pipeline.
- Square root thread is the second thread in the pipeline. It consumes items from the buffer it shares with the input thread. It computes the square root of this item. It puts the computed value in a buffer it shares with the next thread in the pipeline. Thus this thread implements both consumer and producer functionalities.
- Output thread is the third thread in the pipeline. It consumes items from the buffer it shares with the square root thread and prints the items.

*/

// Size of the buffers
#define SIZE 50
#define MAXCHAR 1024

// Number of items that will be produced. This number is less than the size of the buffer. Hence, we can model the buffer as being unbounded.
#define NUM_THREADS 4

// Buffer 1, shared resource between input thread and square-root thread
char buffer_1[SIZE][MAXCHAR];
// Number of items in the buffer
int count_1 = 0;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
// Index where the square-root thread will pick up the next item
int con_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;


// Buffer 2, shared resource between square root thread and output thread
char buffer_2[SIZE][MAXCHAR];
// Number of items in the buffer
int count_2 = 0;
// Index where the square-root thread will put the next item
int prod_idx_2 = 0;
// Index where the output thread will pick up the next item
int con_idx_2 = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

// Buffer 3, shared resource between square root thread and output thread
char buffer_3[SIZE][MAXCHAR];
// Number of items in the buffer
int count_3 = 0;
// Index where the square-root thread will put the next item
int prod_idx_3 = 0;
// Index where the output thread will pick up the next item
int con_idx_3 = 0;
// Initialize the mutex for buffer 3
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 3
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//                                               Start and End Code                                                    |
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

/*
This fucntion is just made to display the creator of the program.
Has no functionality
*/
void start_program(){
    printf("\n******************"
    "************************************************");
    printf("\n\n\n\t****Multi-threaded Producer Consumer Pipeline****");
    printf("\n\n\t\t\t  -By: Adam Oberg-");
    printf("\n\n\n\n*******************"
    "**********************************************");
    // char* username = getenv("USER");
    // printf("\nUSER is: @%s", username);
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
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//                                               Start and End Code                                                    |
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//                                                                                                                     |
//                                                                                                                     |
//                                                                                                                     |
//----------------------------------------------------------------------------------------------------------------------
//                                                 Start Buffer 1                                                      |
//----------------------------------------------------------------------------------------------------------------------
/*
Get input from the user.
This function doesn't perform any error checking.
*/
char* get_user_input(){
  char* userInput;
  char *currLine = NULL;
  size_t max = MAXCHAR;
  userInput = calloc(MAXCHAR + 1, sizeof(char)); //Allocating space for userinput and clear userInput
  getline(&currLine, &max , stdin);


  if(strcmp(userInput, "\n") == 0 ){
    printf("Error: No input has been given.\n");
    fflush(stdout);
    exitProgram();
  }
  return userInput;
}

/*
 Put an item in buff_1
*/
void put_buff_1(char* item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_1);
  // Put the item in the buffer
  strcpy(buffer_1[prod_idx_1],item);
  // Increment the index where the next item will be put.
  prod_idx_1 = prod_idx_1 + 1;
  count_1++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_1);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
}

/*
 Function that the input thread will run.
 Get input from the user.
 Put the item in the buffer shared with the square_root thread.
*/
void *get_input(void *args){

    // Get the user input
    char* item = get_user_input();
    put_buff_1(item);
    return NULL;
}

/*
Get the next item from buffer 1
*/
char* get_buff_1(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_1);
  while (count_1 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_1, &mutex_1);
  char* item = buffer_1[con_idx_1];
  // Increment the index from which the item will be picked up
  con_idx_1 = con_idx_1 + 1;
  count_1--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
  // Return the item
  return item;
}
//----------------------------------------------------------------------------------------------------------------------
//                                                 End Buffer 1                                                        |
//----------------------------------------------------------------------------------------------------------------------
//                                                                                                                     |
//                                                                                                                     |
//                                                                                                                     |
//----------------------------------------------------------------------------------------------------------------------
//                                                 Start Buffer 2                                                      |
//----------------------------------------------------------------------------------------------------------------------
/*
 Put an item in buff_2
*/
void put_buff_2(char* item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_2);
  // Put the item in the buffer
  strcpy(buffer_2[prod_idx_2],item);
  // Increment the index where the next item will be put.
  prod_idx_2 = prod_idx_2 + 1;
  count_2++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_2);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
}

/*
-Function: line_separator
-Description: This is a fucntion to start to replace every line separator in the input by a space.
- Input: NULL
- OUTPUT: NULL
*/
void *line_separator(void *args){
    char* item;
    item = get_buff_1();
    int i = 0;
    int sizeofbuffer1 = strlen(item);
    while(i != sizeofbuffer1){
      if(item[i] == '\n'){
        item[i] = ' ';      //Loop through the string arrray untill newline then replace with ' ' and continue
      }
      i++;
    }
    put_buff_2(item);
    return NULL;
}

/*
Get the next item from buffer 2
*/
char* get_buff_2(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_2);
  while (count_2 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_2, &mutex_2);
  char* item = buffer_2[con_idx_2];
  // Increment the index from which the item will be picked up
  con_idx_2 = con_idx_2 + 1;
  count_2--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
  // Return the item
  return item;
}
//----------------------------------------------------------------------------------------------------------------------
//                                                 End Buffer 2                                                        |
//----------------------------------------------------------------------------------------------------------------------
//                                                                                                                     |
//                                                                                                                     |
//                                                                                                                     |
//----------------------------------------------------------------------------------------------------------------------
//                                                 Start Buffer 3                                                      |
//----------------------------------------------------------------------------------------------------------------------
/*
 Put an item in buff_3
*/
void put_buff_3(char* item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_3);
  // Put the item in the buffer
  strcpy(buffer_3[prod_idx_3],item);
  // Increment the index where the next item will be put.
  prod_idx_3 = prod_idx_3 + 1;
  count_3++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_3);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_3);
}


/*
-Function: plus_sign
-Description: This is a fucntion that replaces every instance of ++ with ^
- Input: NULL
- OUTPUT: NULL
*/
void* plus_sign(){
  char* item;
  item = get_buff_2();
  int i = 0;
  int j;
  int itemlen = strlen(item);
  char temp;
  while(i != itemlen){
    if(item[i] == '+' && item[i + 1] == '+'){
      item[i] = '^';      //Loop through the string arrray untill newline then replace with ' ' and continue
      j = i + 1;
      while(j != itemlen){
        temp = item[j + 1];
        item[j] = temp;
        j++;
      }
      item[j+1] = '\0';
    }
    i++;
  }

  put_buff_3(item);
  return NULL;
}
/*
Get the next item from buffer 3
*/
char* get_buff_3(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_3);
  while (count_3 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_3, &mutex_3);
  char* item = buffer_3[con_idx_3];
  // Increment the index from which the item will be picked up
  con_idx_3 = con_idx_3 + 1;
  count_3--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_3);
  // Return the item
  return item;
}
//----------------------------------------------------------------------------------------------------------------------
//                                                 End Buffer 3                                                        |
//----------------------------------------------------------------------------------------------------------------------



/*
 Function that the output thread will run.
 Consume an item from the buffer shared with the square root thread.
 Print the item.
*/
void *write_output(void *args){
    char* item;
    item = get_buff_3();
    int itemLen = strlen(item);
    int numLines = itemLen / 80;
    int numChar = numLines * 80;
    while (strncmp(item, "STOP\n", strlen("STOP\n")) != 0) {
    if(itemLen >= 80){
      for (size_t j = 0; j < numChar ; j++) {
        printf("%c",item[j]);
        if ((j + 1) % 80 == 0 && j != 0) {
          printf("\n");
        }
      }
    }
  }

    printf("\n");
    return NULL;
}
int main()
{
    srand(time(0));
    // Create the threads
    start_program();
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int result_code, index;
    for (index = 0; index < NUM_THREADS; ++index) {
      // Create all threads one by one
      thread_args[index] = index;
      //----------------------------------------------------------------------------------------------
      switch(index){
        case 0:
          //Case for thread 1 to run the function: input_thread();
          result_code = pthread_create(&threads[index], NULL, get_input, NULL);
          break;
        case 1:
          //Case for thread 2 to run the function:
          result_code = pthread_create(&threads[index], NULL, line_separator, (void *) &thread_args[index]);
          break;
        case 2:
          //Case for thread 3 to run the function:
          result_code = pthread_create(&threads[index], NULL, plus_sign, (void *) &thread_args[index]);
          break;
        case 3:
          //Case for thread 4 to run the function:
          result_code = pthread_create(&threads[index], NULL, write_output, (void *) &thread_args[index]);

          break;
        default:
          printf("It should never get here\n");
      }
      assert(0 == result_code);
      //----------------------------------------------------------------------------------------------
      }

      // Wait for each thread to complete
      for (index = 0; index < NUM_THREADS; ++index){
        result_code = pthread_join(threads[index], NULL);
        assert(0 == result_code);
      }
    exitProgram();
    return EXIT_SUCCESS;
}
