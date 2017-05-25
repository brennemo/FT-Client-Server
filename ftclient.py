# Morgan Brenner
# brennemo@oregonstate.edu 
# CS 372 Program 2

import signal 
import socket
import sys


if __name__ == "__main__":
	#start on host B 

	#validate command line parameters 

	#establish TCP control connection on <SERVER_PORT>, i.e., connection P

	#send command (-l (list) or -g <FILENAME> (get)) on connection P 

		#invalid command: display error message from connection P

		#valid command: TCP data connection with ftserver on <DATA_PORT>, i.e., connection Q 

				#receive error message on connection P and display 
				#save file received on connection Q in current default directory 
					#handle duplicate file names
					#display 'transfer complete' message 