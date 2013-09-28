#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <list>
#include <iterator>
#include "client.h"

using namespace std;


void init_sockaddr (struct sockaddr_in *name,
               const char *hostname,
               uint16_t port);
void closeAllSockets(list<Client*> client);
void closeAllFd(list<int> fd);
void addAllSockets(list<Client*> client, fd_set *readfd, int sock);
               
           
