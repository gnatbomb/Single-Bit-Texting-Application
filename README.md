# 379Assignment1

Plan for encoding:
	Once a connection is established, the user enters their message.
	The message is split into each of its characters and each is sent individually.
	For the double solution:
		Associate siguser1 with 1 and siguser 2 with 0. Then build the strings using ascii encoding.
	
	For the single solution:
		Send a signal and wait for response. Upon recieving response, a short timeframe, say 20ms, begins. During that time,
		the recieving process counts the number of times it recieves siguser1.
			If it recieves 2 instances of the signal, it is a 1.
			If it recieves 1 instance of the signal, it is a 0.
			If it recieves any other number of signals, it is an error and logs it.
		After the process recieves the bit, it sends the bit back to the sending process to signal which character it recieved.
		The original sending process then compares the bit it recieved to the bit it sent. If they match, it sends a 1, otherwise, it sends a 0.
		The recieving process then ignores it if it recieves a 1, or it logs an error if it receives a 0. 
		This completes the confirmation and sending of a bit in 60ms, which should result in 60*8 = 480 ms/char. 60/0.48 = 125 cpm.
		When a message is finished sending, we simply stop sending signals. After a delay of 200ms with no signal, the second process assumes that 
		the message has ended, and it prints the message to the screen. If it detected an error during the sending, it prefaces it with a question
		mark. If it did not, then it prefaces the message with an exclamation mark.



		We then use this same solution with the double solution.

	Assuming we need to get at least 80 cpm, that means we have 80/60 = 1.333333 characters per second. Each character is 8 bits, which means we have 
	0.166 seconds for each bit. Thus we decided that our single solution should use the timeframe of 20 ms to send.
	
									
									


Work Balance section:
Created github, and makefile together during Jan 14 Lab.