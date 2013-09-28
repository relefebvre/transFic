#include <string.h>
#include <stdio.h>
#include <errno.h>

class Client {
	
		int sock, fd;
		char filename[512];
		bool first;
		
	public :
		static int maxSock;
	
		Client(int newSock);
		~Client();
		
		void setSock(int newSock);
		int getSock();
		
		void setFilename(char *filename);
		
		void setFirst();
		bool getFirst();
		
		void setFd(int newFd);
		int getFd();
};
