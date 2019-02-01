#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define MAXINPUT 4096         // Max input stream length
#define SLEEPTIME 500         // Microseconds between messages
#define PAUSETIME 50          // Microseconds between pings for single bit encoding
#define MAXDELAY 2000         // Maximum time spent waiting for double-bit encoding
#define ONEVALUE 6            // Number of pings to represent a 1

#define SINGLESIG false
#if defined(SINGLE)           // Set SINGLESIG depending on compilation flag
#undef SINGLESIG
#define SINGLESIG true
#endif

int proc2id;
char decode_char;             // Character currently being decoded by receiving process
int char_count = 0;           // Current character count in decoding process
char sending_msg[MAXINPUT];   // Input stream for sending process

// Single signal flags
bool first_call = true;       // Used to initialize timer
int ping_count = 0;           // Tracks # of signals sent for precision
struct timeval curr_time;     // Milliseconds since last killcall.
long old_ms, new_ms;


/*
  Helper method for signal handler. Keeps track of bitcount for
  current character. If the current bitcount is equal to 8, then
  the current character has been built. Then, the character is
  ready to be interpreted, and this function prints it to the
  receiving process
*/
void check_print() {
  if (++char_count == 8) {
    write(1, &decode_char, 1);
    decode_char = '\0';
    char_count = 0;
  }
}

/*
  Encodes the character buffer/stream into bits, while
  simultaneously signalling to a receiving process the
  corresponding signal message
*/
void send_to_proc() {
  int i = 0;
  while (i<MAXINPUT) {
    unsigned char c = sending_msg[i];
    // Null character termination check
    if (strcmp(&sending_msg[i], "\0") == 0) {
      break;
    }
    
    if (!SINGLESIG) {
      // For each bit--
      for (int j=7; j>=0; j--) {
	int bit = (c >> j) & 1;
	if (bit == 0) {
	  kill(proc2id, SIGUSR1);
	} else if (bit == 1) {
	  kill(proc2id, SIGUSR2);
	}
	usleep(SLEEPTIME);
      }
      i++;
      
    } else if (SINGLESIG) {
      for (int j=7; j>=0; j--) {
	int bit = (c >> j) & 1;
	// Each bit sends multiple sigusr1s: bit 0 sends a few,
	// bit 1 sends many. The receiving process deciphers
	// the bit depending on the ping count.
	if (bit == 0) {
	  for(int i = 0; i < (ONEVALUE / 2); i++){
	    usleep(PAUSETIME);
	    kill(proc2id, SIGUSR1);
	  }
	}
	else{
	  for(int i = 0; i < (2 * ONEVALUE); i++){
	    usleep(PAUSETIME);
	    kill(proc2id, SIGUSR1);
	  }
	}
	usleep(MAXDELAY * 2);
      }
      i++;
    }
  }
  // Clear the input stream
  memset(sending_msg, '\0', i);
  return;
}

void sig1(int signum) {
  if (!SINGLESIG) {
    // Update only char_count
    check_print();
    return;
    
  } else if (SINGLESIG) {
    if (first_call) {
      // Set initial base time
      gettimeofday(&curr_time, NULL);
      old_ms  = (curr_time.tv_sec * 1000) + (curr_time.tv_usec/1000);
      first_call = false;
    }
    // Get current time to check if the bit delay (MAXDELAY) has elapsed
    gettimeofday(&curr_time, NULL);
    new_ms = (curr_time.tv_sec * 1000) + (curr_time.tv_usec/1000);
    if ((new_ms - old_ms) > MAXDELAY/1000) {
      if (ping_count >= ONEVALUE){
	decode_char = decode_char | (1 << (7-char_count));
      }
      // Reset base time and ping count
      gettimeofday(&curr_time, NULL);
      old_ms = (curr_time.tv_sec * 1000) + (curr_time.tv_usec/1000);
      check_print();
      ping_count = 0;
    }
    ping_count++;
    return;
  }
}

void sig2(int signum) {
  // Append a bit 1 to the current char being constructed
  decode_char = decode_char | (1 << (7-char_count));
  check_print();
  return;
}

int main(void) {
  signal(SIGUSR1, sig1);
  signal(SIGUSR2, sig2);
  
  printf("Own PID: %d\nEnter partner's PID: ", getpid());
  scanf("%d", &proc2id);
  sleep(1);
  
  while(1) {
    // Scan until a newline
    scanf("%[^\n]", sending_msg);
    getchar();

    // Message termination check
    if(strcmp(&sending_msg[0], ".") == 0)
      break;

    // Append a newline and offload to send_to_proc
    sending_msg[strlen(sending_msg)] = '\n';
    if (SINGLESIG)
      sending_msg[strlen(sending_msg)] = '\n';
    send_to_proc();
  }
  return 0;
}
