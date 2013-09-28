#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netdb.h>
#include <fcntl.h>

#define PORT 1024

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

int main(int argc, char **argv)
{
	char *buf, next='o';
	int fic, nbLu=1, tbuf;
	int sock=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in sin;
	
	init_sockaddr(&sin,"0.0.0.0",atoi(argv[1]));
	
	
	if (connect(sock, (struct sockaddr *) &sin, sizeof(sin)) != 0)
	{
		perror("Connexion échouée ") ;
		close(sock) ;
		exit(1) ; 
	 }
	 
	 if (read(sock,&tbuf,sizeof(int)) == -1)
	 {
		 perror("Ecriture ");
		 close(sock);
		 exit(1);
	 }
	 
	 printf("Taille du buffer : %d\n",tbuf);
	 
	 buf = (char*)malloc(tbuf);
	 if (buf == NULL)
	 {
		 perror("Malloc ");
		 close(sock);
		 exit(1);
	}

	while (next == 'o' || next == 'O')
	{
	
		printf("Nom du fichier : ");
		scanf("%s",buf);
		write(sock,buf,tbuf);
		
		printf("Emplacement du fichier : ");
		scanf("%s",buf);
		
		fic = open(buf,O_RDONLY);
		
		if (fic == -1)
		{
			perror("Ouverture fichier ");
			close(sock);
			exit(1);
		}
		
		while(nbLu > 0)
		{
			nbLu = read(fic,buf,sizeof(buf));
			if (nbLu == -1)
			{
				perror("Lecture ");
				close(sock) ;
				close(fic);
				exit(1) ;
			}
			
			if (write(sock,buf,nbLu) == -1)
			{
				perror("Ecriture ");
				close(sock) ;
				close(fic);
				exit(1);
			}
			
		}
		
	printf("Envoyer un autre fichier (O/N) ? ");
	scanf("%*c%c",&next);
		
	}
	
	
	close(sock);
	close(fic);
	
	return 0;
}
	

