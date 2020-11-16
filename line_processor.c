/* Program Name: line_processor.c
** Author: Adam Oberg
** Description: In this assignment, you'll write a program that will get you
** familiar with the use of threads, mutual exclusion and condition variables.


Assignment Goals:
================================================================================
      XX -Thread 1, called the Input Thread, reads in lines of characters from |
         -the standard input.                                                  |
      XX -Thread 2, called the Line Separator Thread, replaces every line      |
         -separator in the input by a space.                                   |
      XX -Thread, 3 called the Plus Sign thread, replaces every pair of plus   |
         -signs,  i.e., "++", by a "^".                                        |
      XX -Thread 4, called the Output Thread, writes this processed data to    |
         -standard output as lines of exactly 80 characters.                   |
================================================================================

*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <assert.h>
#include <stddef.h>


// Size of the buffers
#define SIZE 50
#define MAXCHAR 1024

// Number of items that will be produced. This number is less than the size of the buffer.
//Hence, we can model the buffer as being unbounded.
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



//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//                                               Start and End Code            |
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

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
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//                                Start and End Code                           |
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//                                                                             |
//                                                                             |
//                                                                             |
//------------------------------------------------------------------------------
//                                  Start Buffer 1                             |
//------------------------------------------------------------------------------
/*
Get input from the user.
This function doesn't perform any error checking.
*/
char* get_user_input(){
  char* userInput;
  char *currLine = NULL;
  size_t max = MAXCHAR;
  userInput = calloc(MAXCHAR + 1, sizeof(char));
  //Allocating space for userinput and clear userInput
  getline(&currLine, &max , stdin);


  strcpy(userInput,currLine);
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
  int neverstop = 1;
  while (neverstop == 1) {

    char* item = get_user_input();
    put_buff_1(item);
    if (strncmp(item, "STOP\n", strlen("STOP\n")) == 0) {
      break;
    }
  }
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
//------------------------------------------------------------------------------
//                                    End Buffer 1                             |
//------------------------------------------------------------------------------
//                                                                             |
//                                                                             |
//                                                                             |
//------------------------------------------------------------------------------
//                                  Start Buffer 2                             |
//------------------------------------------------------------------------------
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
-Description: This is a fucntion to start to replace every line separator in the
- input by a space.
- Input: NULL
- OUTPUT: NULL
*/
void *line_separator(void *args){
    char* item;
    int neverstop = 1;
  while (neverstop == 1) {
      item = get_buff_1();
      int i = 0;
      int sizeofbuffer1 = strlen(item);
      while(i != sizeofbuffer1){
        if(item[i] == '\n'){
          item[i] = ' ';
          //Loop through the string arrray untill newline then replace with ' '
          //and continue
        }
        i++;
      }
      put_buff_2(item);
      if (strncmp(item, "STOP ", strlen("STOP ")) == 0) {
          break;
      }
    }

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
//------------------------------------------------------------------------------
//                                  End Buffer 2                               |
//------------------------------------------------------------------------------
//                                                                             |
//                                                                             |
//                                                                             |
//------------------------------------------------------------------------------
//                                 Start Buffer 3                              |
//------------------------------------------------------------------------------
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
  int neverstop = 1;
  while (neverstop == 1) {
    item = get_buff_2();
    int i = 0;
    int j;
    int itemlen = strlen(item);
    char temp;
    while(i != itemlen){
      if(item[i] == '+' && item[i + 1] == '+'){
        item[i] = '^';      //Loop through the string arrray untill newline then
                          //replace with ' ' and continue
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
    if (strncmp(item, "STOP ", strlen("STOP ")) == 0) {
      break;
    }
  }



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
//------------------------------------------------------------------------------
//                                  End Buffer 3                               |
//------------------------------------------------------------------------------


/*
 Function that the output thread will run.
 Consume an item from the buffer shared with the square root thread.
 Print the item.
*/
void *write_output(void *args){
    char* item;
    int neverstop = 1;
    char* masterItem = calloc(sizeof(MAXCHAR + 1), sizeof(char));
    int p;
    while ( neverstop == 1 ) {
      item = get_buff_3();
      int itemLen = strlen(item);
      for (int j = 0; j < itemLen; j++) {
        // printf("item[j]: %c\n", item[j]);
        masterItem[p] = item[j];    //Copy charactes into master string
        p++;
        // printf("masterItem[j]: %c\n", masterItem[j]);
        if(p == 80) {  // Check to see if len of master is 80
          for (int i = 0; i < 80; i++) { //Print if 80 char
            printf("%c", masterItem[i]);
          }
          printf("\n");
          p = 0;

        }

      }

      if (strncmp(item, "STOP ", strlen("STOP ")) == 0) {
        break;
      }

      // get string, concat into master string, loop if count mastersting > 80, print 80 remove



    //   int itemLen = strlen(item);   // Get the length of the string
    //   int numLines = itemLen / 80;  // Find how many lines to print
    //   int numChar = numLines * 80;  // find the ammount of chars to print
    //   if (strncmp(item, "STOP ", strlen("STOP ")) == 0) {
    //     break;
    //   }
    //   else if(itemLen >= 80){
    //     for (size_t j = 0; j < numChar ; j++) {
    //       printf("%c",item[j]);   //print each char
    //       if ((j + 1) % 80 == 0 && j != 0) {
    //      printf("\n");
    //     }
    //   }
    // }
  }
    return NULL;
}
int main()
{
  srand(time(0));
  pthread_t input_t, line_separator_t, plus_sign_t, output_t;
  // Create the threads
  pthread_create(&input_t, NULL, get_input, NULL);
  pthread_create(&line_separator_t, NULL, line_separator, NULL);
  pthread_create(&plus_sign_t, NULL, plus_sign, NULL);
  pthread_create(&output_t, NULL, write_output, NULL);

  // Wait for the threads to terminate
  pthread_join(input_t, NULL);
  pthread_join(line_separator_t, NULL);
  pthread_join(plus_sign_t, NULL);
  pthread_join(output_t, NULL);

  return EXIT_SUCCESS;
    exitProgram();
    return EXIT_SUCCESS;
}
