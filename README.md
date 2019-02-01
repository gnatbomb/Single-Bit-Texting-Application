# 379Assignment1

Plan for encoding:
	Once a connection is established, the user enters their message.
	The message is split into each of its characters and each is sent individually.
	Each character is sent a single bit at a time.
	For the double signal solution:
		Associate siguser1 with 1 and siguser 2 with 0. Then build the strings using bitwise operations to recreate the \
		character.
	
	For the single signal solution:
		Send a couple signals in a small timeframe. Sends 3 signals if the next bit is 0, or 8 signals if the next bit is 1.
		The receiving process interprets 5 or more received signals in this timeframe as a 1, and fewer than 5 as a 0.
		
		The recieving process then ignores it if it recieves a 1, or it logs an error if it receives a 0. 
		This completes the confirmation and sending of a bit in 60ms, which should result in 60*8 = 480 ms/char. 60/0.48 = 125 
		cpm.
		When a message is finished sending, we simply stop sending signals. After a certain delay since the last signal, the 
		second process assumes that 
		the message has ended, and it prints the message to the screen. If it detected an error during the sending, it prefaces 
		it with a question mark. If it did not, then it prefaces the message with an exclamation mark.


		We then use this same solution with the double solution.

	Assuming we need to get at least 80 cpm, that means we have 80/60 = 1.333333 characters per second. Each character is 8 bits, which means we have 
	0.166 seconds for each bit. Thus we decided that our single solution should use the timeframe of 20 ms to send.
	
	
	
Question 1)
The program breaks each message into its individual characters and sends them one at a time. The characters are then broken into their binary representation, so we can send 8 1's or 0's per character. The recieving program then interprets these characters and prints them.
To determine whether a 1 or a 0 is next:
Send a couple signals in a small timeframe. Sends 3 signals if the next bit is 0, or 8 signals if the next bit is 1.
The receiving process interprets 5 or more received signals in this timeframe as a 1, and fewer than 5 as a 0.
When a message is finished sending, we simply stop sending signals. After a certain delay since the last signal, the second process assumes that the message has ended, and it prints the message to the screen.

Question 2)
The program breaks each message into its individual characters and sends them one at a time. The characters are then broken into their binary representation, so we can send 8 1's or 0's per character. The recieving program then interprets these characters and prints them. This part is the same as my explanation in question 1.
The way the programs communicate is using SIGUSR1 and SIGUSR2. If a program receives a SIGUSR1, it treats this as receiving a 0, and keeps note of it. Same goes for SIGUSR2, but with a 1 instead.

Question 7)
We tried using SIGALARM while implementing the solution for the single signal problem. We chose not to use it because alarm() only allows the use of integers, and therefore entire seconds, but this proved far too slow for our needs.
For the single signal problem I also tried using sigaction() to get the pid of the sending process. I had originally tried working around the "single signal" problem by forking a child process and having the child send a signal. Effectively, if a process received a SIGUSR1, it would check the pid of the sending process. If the pid matched the pid of its partner, it interpreted that as a 1, and if the pid didn't match the partner's pid (meaning the forked child sent it), it interpreted that as a 0. I, however, had difficulty implementing this solution and gave up on the idea.
									
									


Work Balance section:
Created github, and makefile together during Jan 14 Lab.
Muhammed created main(), as well as the double signal solution.
Discussed the single signal solution together during Jan 21 Lab.
Nicholas created the single signal solution and pushed to github on January 28.
Discussed error handling and avoidance together in January 28 lab.
Nicholas answered questions 1, 2, and 7 on the readme.
