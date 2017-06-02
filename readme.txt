http://beej.us/guide/bgnet/examples/server.c


https://docs.python.org/2/library/socket.html
https://docs.python.org/2/howto/sockets.html
https://docs.python.org/2/tutorial/inputoutput.html
https://docs.python.org/2/library/filesys.html
https://docs.python.org/2/library/os.html



gcc -o ftserver ftserver.c

./ftclient localhost 40259 -g shortestfile.txt 55
./ftclient localhost 50014 -g shorterfile.txt 45
./ftclient localhost 50014 -g shortfile.txt 45

./ftclient localhost 50014 -g longfile.txt 45
./ftclient localhost 50014 -g longestfile.txt 45

./ftclient flip1.engr.oregonstate.edu 31022 