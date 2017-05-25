# Morgan Brenner
# brennemo@oregonstate.edu 
# CS 372 Program 2

import signal 
import socket
import sys


if __name__ == "__main__":

	#if len(sys.argv) < 3:
	#print "USAGE: ./chatserve <port #>"
	#exit(1)

	#https://docs.python.org/2/library/socket.html
	HOST = sys.argv[1]
	PORT = int(sys.argv[len(sys.argv)-1])   
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST, PORT))
	s.sendall('Hello, world')
	data = s.recv(1024)
	s.close()
	print 'Received', repr(data)
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