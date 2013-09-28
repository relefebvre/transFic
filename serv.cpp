#include "serv.h"

void init_sockaddr (struct sockaddr_in *name,
               const char *hostname,
               uint16_t port)
{
  struct hostent *hostinfo; 

  name->sin_family = AF_INET;   /* Adresses IPV4 (Internet) */
  name->sin_port = htons (port); /* On gère le little/big Endian */
  hostinfo = gethostbyname (hostname); /* appeler les fonctions de résolution de nom de la libC */
  if (hostinfo == NULL) /* Si ne peut transformer le nom de la machine en adresse IP */
    {
      fprintf (stderr, "Unknown host %s.\n", hostname);
      exit (EXIT_FAILURE);
    }
  name->sin_addr = *(struct in_addr *) hostinfo->h_addr_list[0]; /* Le dernier champs de la structure est garni */
}

void closeAllSockets(list<Client*> client)
{
	for(list<Client*>::iterator i=client.begin() ; i != client.end() ; ++i)
	{
		if (close((*i)->getSock()) == -1)
		{
			perror("Fermeture socket ");
			exit(1);
		}
	}
}

void closeAllFd(list<int> fd)
{
	for(list<int>::iterator i=fd.begin(); i != fd.end() ; ++i)
	{
		if (close(*i) == -1)
		{
			perror("Fermeture fd ");
			exit(1);
		}
	}
}

void addAllSockets(list<Client*> client, fd_set *readfd, int sock)
{
	FD_ZERO(readfd);
	FD_SET(sock, readfd);
	
	for(list<Client*>::iterator i=client.begin() ; i != client.end() ; ++i)
	{
		printf("Ajout de la socket : %d\n",(*i)->getSock());
		FD_SET((*i)->getSock(), readfd);
	}
}

int main(int argc, char **argv)
{
	char buf[4096];
	int sock=socket(AF_INET,SOCK_STREAM,0);
	int nbLu=1, retval, tbuf;
	unsigned port;
	struct sockaddr_in sin, csin;
	socklen_t taille=sizeof(csin);
	list<Client*> client; 
	list<int> fd;
	fd_set readfd;

    if(argc >= 2 )
        if ( sscanf(argv[1],"%u",&port) != 1  )
        {
            fprintf(stderr,"Numéro de port invalide\n");
            close(sock);
            exit(1);
        }

    if (argc < 2)
	{
        port = 1025 ;
	}
	
	
	if (sock+1 > Client::maxSock)
		Client::maxSock = sock+1;
	
	init_sockaddr(&sin,"0.0.0.0",port);
	
	if (bind(sock,(struct sockaddr *) &sin, sizeof(sin)) != 0)
	{
		perror("Erreur lors du bind de la socket") ;
		close(sock) ;
		exit(1) ;
	}
	
	
	if(listen(sock, 5) != 0)
	{
		perror("Listen non réussi ") ;
		close(sock) ;
		exit(1) ;
	}
	
	while(1)
	{
	
		addAllSockets(client, &readfd, sock);
		
		retval = select(Client::maxSock,&readfd, NULL, NULL, NULL);
		
		if (retval == -1)
		{
			perror("Select ");
			closeAllSockets(client);
			closeAllFd(fd);
			close(sock);
			exit(1);
		}
			
		if (FD_ISSET(sock, &readfd))
		{
			list<Client*>::iterator i;
			
			client.push_front(new Client(accept(sock,(struct sockaddr *) &csin, &taille)));
            printf("Client connecté\n");
			i = client.begin();
			tbuf = sizeof(buf);
			printf("Taille du buffer : %lu\n",sizeof(buf));
			write((*i)->getSock(),&tbuf,sizeof(int));
		}

			
		for(list<Client*>::iterator i=client.begin(); i != client.end() ; )
		{
			if (FD_ISSET((*i)->getSock(), &readfd))
			{
				if ((*i)->getFirst())
				{
					read((*i)->getSock(),buf,sizeof(buf));
					
					if(strcmp(buf,"break") == 0)
					{
						printf("Fermeture du server\n");
						goto end;
					}
						
					(*i)->setFilename(buf);
					(*i)->setFd(open(buf,O_CREAT | O_WRONLY, S_IRWXU));
					printf("Création du fichier : %s\n",buf);
					if ((*i)->getFd() == -1)
					{
						perror("Ouverture fichier ");
						closeAllSockets(client);
						closeAllFd(fd);
						close(sock) ;
						exit(1) ;
					}
					fd.push_front((*i)->getFd());
					(*i)->setFirst();
				}
				else
				{
						printf("Lecture sur le fd : %d\n",(*i)->getSock());
						nbLu = read((*i)->getSock(),buf,sizeof(buf));
						if (nbLu > 0)
						{
							if (nbLu == -1)
							{
								perror("Lecture ");
								closeAllSockets(client);
								closeAllFd(fd);
								close(sock) ;
								exit(1) ;
							}
				
							if (write((*i)->getFd(),buf,nbLu) == -1)
							{
								perror("Ecriture ");
								closeAllSockets(client);
								closeAllFd(fd);
								close(sock) ;
								exit(1) ;
							}
						}
						if (nbLu == 0)
						{
							list<int>::iterator it=fd.begin(); 
							
                            close((*i)->getSock()) ;                      
                            
                            while (it != fd.end())
                            {
								if ((*i)->getFd() == (*it))
									it = fd.erase(it);
								else
									++it;
							}
                            
                            close((*i)->getFd()) ;
                            
							i = client.erase(i);
							
							continue;
						}
						else 
						{
							++i;
							continue;
						}
				}
					
			}
		++i;
		}
	}

	end:;
	
	closeAllSockets(client);
	closeAllFd(fd);
	close(sock);
	
	client.erase(client.begin(),client.end());
	fd.erase(fd.begin(),fd.end());
	
	return 0;
}
	
