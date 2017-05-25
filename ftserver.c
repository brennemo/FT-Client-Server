//start on host A

//validate command line parameters

//repeatedly wait on <PORTNUM> for client (until terminated by SIGINT)

//establish TCP control connection on <SERVER_PORT>, i.e., connection P

//wait on connection P for command from ftclient 

	//receive command on connection P 

		//if ftclient sent invalid command

			//error message to ftclient on connection P

		//else 
			//initiate TCP data connection with ftclient on <DATA_PORT>, i.e., connection Q 
				
				//if command from ftclient == "-g <FiLENAME>"
					//validate FILENAME 
					//send error message on connection P 
					//send contents of FILENAME on connection Q 

			//close connection Q
	
	//close connection P and terminate
	