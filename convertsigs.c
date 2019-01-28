#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/signalfd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>

#define MAXINPUT 800000       //Longest string of characters that can be sent in a single message
#define SLEEPTIME 500        //Microseconds between messages
#define PAUSETIME 500      //Microseconds between pings for single bit encoding
#define MAXDELAY 2000    //Maximum time spent waiting for double-bit encoding

int proc2id;
char decode_char;
int char_count = 0;
char sending_msg[MAXINPUT];
char rec_msg[MAXINPUT];


void check_print() {
  /*helper method for signal handler. Keeps track of bitcount for current character.
  if the current bitcount is equal to 8, then the current character has been built.
  Then, the character is ready to be interpreted, and this function prints it to the receiving process*/
   if (++char_count == 8) {
    write(1, &decode_char, 1);
    decode_char = '\0';
    char_count = 0;
   // printf("printing time\n");
  }
  return;
}

#if !defined(SINGLE) //this is for when we have 2-bits for signals
void sig1(int signum) {
  //sends a signal to encode a bit as 0
  check_print();
  return;
}
void send_to_proc() {
  //Encodes the characters into bits, then calls sig1 and sig2 depending on the bits.
  //This sends a signal to the other process.
  int i = 0;
  while (i<MAXINPUT) {    //Makes sure that we don't get out of bounds access from array
    unsigned char c = sending_msg[i];
    if (strcmp(&sending_msg[i], "\0") == 0) { //ends the loop if we hit a null character.

      break;
    }
    for (int j=7; j>=0; j--) { //Decodes a single character into individual bits.
      int bit = (c >> j) & 1;
      if (bit == 0) {
        kill(proc2id, SIGUSR1);
      } else if (bit == 1) {
        kill(proc2id, SIGUSR2);
      }
      usleep(SLEEPTIME);
    }
    i++;
  }
  memset(sending_msg, '\0', i); //Clears the array of the modified characters.
  return;
}

#endif

//The following is the code which runs when we only have a single bit of information we can send.

#if defined(SINGLE) //this is for when we have only 1-bit for signals
int remainingPause = MAXDELAY;
int firstCall = 1;
int pingCount = 0;
struct timeval milliseconds; //milliseconds since last killcall.
long int actualMilliseconds;
long int newActualMilliseconds;

void send_to_proc() {
  //Encodes the characters into bits, then calls sig1 and sig2 depending on the bits.
  //This sends a signal to the other process.
  int i = 0;

  while (i<MAXINPUT) {    //Makes sure that we don't get out of bounds access from array
    unsigned char c = sending_msg[i];
    if (strcmp(&sending_msg[i], "\0") == 0) { //ends the loop if we hit a null character.
      break;
    }
    for (int j=7; j>=0; j--) { //Decodes a single character into individual bits.
      int bit = (c >> j) & 1;
      if (bit == 0) {
        kill(proc2id, SIGUSR1);
      }
      else{
        kill(proc2id, SIGUSR1);
        usleep(PAUSETIME);
        kill(proc2id, SIGUSR1);
      }
      usleep(MAXDELAY * 2);
    }
    //printf("send one character\n");
    i++;
  }
  memset(sending_msg, '\0', i); //Clears the array of the modified characters.
  return;
}

void printcall(){
  if (pingCount >= 2){
    decode_char = decode_char | (1 << (7-char_count));
  }
  gettimeofday(&milliseconds, NULL);
  actualMilliseconds = (milliseconds.tv_sec * 1000) + (milliseconds.tv_usec/1000);
  check_print();
  pingCount = 0;
  return;
}

void sig1(int signum) {
  if (firstCall){
    gettimeofday(&milliseconds, NULL);
    actualMilliseconds = (milliseconds.tv_sec * 1000) + (milliseconds.tv_usec/1000);
    firstCall = 0;
  }
  gettimeofday(&milliseconds, NULL);
  newActualMilliseconds = (milliseconds.tv_sec * 1000) + (milliseconds.tv_usec/1000);
 /* printf("newActualMilliseconds = %li \nactualMilliseconds = %li\nFirst minus second = %li\n\n", newActualMilliseconds, 
                                                                                          actualMilliseconds,
                                                                                          (newActualMilliseconds - actualMilliseconds));
                                                                                          */
  if ((newActualMilliseconds - actualMilliseconds) > MAXDELAY/1000){
    printcall();
  }
  pingCount++;

  return;
}
#endif

void sig2(int signum) {
  //sends a signal to encode a bit as 1
  decode_char = decode_char | (1 << (7-char_count));
  check_print();
  return;
}



int main(void) {
  //Infinite loop function. Grabs user input then calls helper functions.
  //signal(SIGUSR1, sig1); //assigns sig1 to SIGUSR1
  signal(SIGUSR1, sig1);
  signal(SIGUSR2, sig2); //assigns sig2 to SIGUSR2
  printf("Own PID: %d\nEnter partner's PID: ", getpid());  //prints own pid
  scanf("%d", &proc2id);              //asks for other process's pid
  while(1) {  //infinite loop until we get a .
    scanf("%[^\n]", sending_msg);
    getchar();
    sending_msg[strlen(sending_msg)] = '\n';
    sending_msg[strlen(sending_msg)] = '\n';
    if(strcmp(&sending_msg[0], ".") == 0) {
      break;
    } else {
      send_to_proc();
    }
  }
  return 0;
}
