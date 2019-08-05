Group: nbombard, usman1

This C program allows two concurrent users to send text messages back and forth with one another, connecting the two using only a 1-bit pipe or 2 1-bit pipes.
We accomplish this in a simple manner, following these steps in order:
	1) User types their message and hits enter, this prevents new messages from being sent until after the current message is done being received.
	2) Each character is encoded into 8-bit binary numbers based on their ASCII values. 
	3) Once encoded, we send the bits one by one to the other user, whose computer decodes them. Since the bits can sometimes fail, we ensure accuracy as follows:


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
	
With all of that out of the way, the message should appear on the receiver's computer accurately and quickly.
	
	
	
Question 1)
The program breaks each message into its individual characters and sends them one at a time. The characters are then broken into their binary representation, so we can send 8 1's or 0's per character. The recieving program then interprets these characters and prints them.
To determine whether a 1 or a 0 is next:
Send a couple signals in a small timeframe. Sends 3 signals if the next bit is 0, or 8 signals if the next bit is 1.
The receiving process interprets 5 or more received signals in this timeframe as a 1, and fewer than 5 as a 0.
When a message is finished sending, we simply stop sending signals. After a certain delay since the last signal, the second process assumes that the message has ended, and it prints the message to the screen.

Question 2)
The program breaks each message into its individual characters and sends them one at a time. The characters are then broken into their binary representation, so we can send 8 1's or 0's per character. The recieving program then interprets these characters and prints them. This part is the same as my explanation in question 1.
The way the programs communicate is using SIGUSR1 and SIGUSR2. If a program receives a SIGUSR1, it treats this as receiving a 0, and keeps note of it. Same goes for SIGUSR2, but with a 1 instead.

Question 3) A message boundary is represented as a C macro that specifies the length of an
input. A message longer than this many bytes as defined by the macro would not work. The
input mechanism only fetches upto a newline, and then a newline is appended by the code
itself.

Question 4) The intent was to compute a numerical sum of the chars of the input stream
and send this with signals at the end of the actual translated bit-stream by the
sending process, which the receiving process would then use to compare the received
sum with its own computed sum.

Question 5) If the above mechanism were implemented it would come with the complication that
the actual sum that was sent was not read correctly, but the entire input stream was
correctly decoded. This would result in a false positive. Similarly, a false negative
could occur in the same manner.

Question 6) The inputs are processed sequentially and if message streams are being sent while
one is being decoded and printed out by the complementary process, those message streams
are queued and will be sent immediately afterwards. This ensures that the decoded
messages do not get intertwined with fast pased input streams.

Question 7)
We tried using SIGALARM while implementing the solution for the single signal problem. We chose not to use it because alarm() only allows the use of integers, and therefore entire seconds, but this proved far too slow for our needs.
For the single signal problem I also tried using sigaction() to get the pid of the sending process. I had originally tried working around the "single signal" problem by forking a child process and having the child send a signal. Effectively, if a process received a SIGUSR1, it would check the pid of the sending process. If the pid matched the pid of its partner, it interpreted that as a 1, and if the pid didn't match the partner's pid (meaning the forked child sent it), it interpreted that as a 0. I, however, had difficulty implementing this solution and gave up on the idea.

Question 8) The program uses time precision to the microsecond so it is difficult to
micro-manage time efficiency. However, because of the scale of precision in time,
most unnecessary CPU cycles are negligible and we minimized wait times as much as
possible to deliver fast performance. For example there are only a couple of milliseconds
for the single-signal solution to send multiple SIGUSR1 signals to encode a '1' bit.
									
									


Work Balance section:
Created github, and makefile together during Jan 14 Lab.
Muhammed created main(), as well as the double signal solution.
Discussed the single signal solution together during Jan 21 Lab.
Nicholas created the single signal solution and pushed to github on January 28.
Discussed error handling and avoidance together in January 28 lab.
Nicholas answered questions 1, 2, and 7 on the readme.
Muhammad revised the submission code for readability and answered the remaining questions on February 1.
