http://beej.us/guide/bgnet/examples/server.c


https://docs.python.org/2/library/socket.html
https://docs.python.org/2/howto/sockets.html
https://docs.python.org/2/tutorial/inputoutput.html
https://docs.python.org/2/library/filesys.html
https://docs.python.org/2/library/os.html



gcc -o ftserver ftserver.c

./ftclient localhost 60488 -g shortestfile.txt 55
./ftclient localhost 60488 -g shorterfile.txt 45
./ftclient localhost 40524 -g shortfile.txt 45