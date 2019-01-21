#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define MAX 8000

int proc2id;
char decode_char;
int char_count = 0;
char sending_msg[MAX];

void check_print() {
   if (++char_count == 8) {
    write(1, &decode_char, 1);
    decode_char = '\0';
    char_count = 0;
  }
}

void sig1(int signum) {
  check_print();
}
void sig2(int signum) {
  decode_char = decode_char | (1 << (7-char_count));
  check_print();
}

void send_to_proc() {
  int i = 0;
  while (i<MAX) {
    unsigned char c = sending_msg[i];
    if (strcmp(&sending_msg[i], "\0") == 0) {
      break;
    }
    for (int j=7; j>=0; j--) {
      int bit = (c >> j) & 1;
      if (bit == 0) {
        kill(proc2id, SIGUSR1);
      } else if (bit == 1) {
        kill(proc2id, SIGUSR2);
      }
      usleep(50);
    }
    i++;
  }
  printf("\n");
  memset(sending_msg, '\0', i);
}

int main(void) {
  signal(SIGUSR1, sig1);
  signal(SIGUSR2, sig2);

  printf("Own PID: %d\n", getpid());
  scanf("%d", &proc2id);

  while(1) {
    scanf("%[^\n]", sending_msg);
    getchar();
    if(strcmp(&sending_msg[0], ".") == 0) {
      break;
    } else {
      send_to_proc();
    }
  }
  return 0;
}
