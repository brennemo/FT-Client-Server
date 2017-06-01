http://beej.us/guide/bgnet/examples/server.c


https://docs.python.org/2/library/socket.html
https://docs.python.org/2/howto/sockets.html
https://docs.python.org/2/tutorial/inputoutput.html
https://docs.python.org/2/library/filesys.html
https://docs.python.org/2/library/os.html



gcc -o ftserver ftserver.c

./ftclient localhost 40027 -g shortestfile.txt 55
./ftclient localhost 40027 -g shorterfile.txt 45
./ftclient localhost 40027 -g shortfile.txt 45

./ftclient localhost 40027 -g longfile.txt 45
./ftclient localhost 40027 -g longestfile.txt 45