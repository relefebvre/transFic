#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netdb.h>
#include <fcntl.h>
#include <time.h>


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
	char *buf, next='o', barre[26];
	int fic, nbLu=1, tbuf, i=0, tot=0, prog, tfic;
	struct stat info;
    int sock ;
    uint port ;
	struct sockaddr_in sin;
	struct timespec time;
	char IPdefault[10] = "127.0.0.1" ;

    if (argc >= 2)
        if ( sscanf(argv[1],"%u",&port) != 1  )
        {
            fprintf(stderr,"Numéro de port invalide\n");
            exit(1);
        }
	
	time.tv_nsec = 100;
	
	if (argc < 2)
	{
        port = 1025 ;
	}
	
	if (argc < 3)
	{
		argv[2] = IPdefault ;
	}
	
    init_sockaddr(&sin,argv[2],port);
	
	


	while (next == 'o' || next == 'O')
    {

        sock=socket(AF_INET,SOCK_STREAM,0);

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

		printf("Nom du fichier lors de l'écriture: ");
		scanf("%s",buf);
		write(sock,buf,tbuf);
		
		if (strcmp(buf,"break") == 0)
		{
			printf("Fermeture du server\n");
			close(sock);
			return 0;
		}
		
		printf("Emplacement du fichier à ouvrir : ");
		scanf("%s",buf);
		
		if (stat(buf,&info) == -1)
		{
			perror("Informations fichier ");
			close(sock);
			exit(1);
		}
		tfic = info.st_size;
		
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
			tot = tot + nbLu;
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
			
			prog = ((tot*100)/tfic)/4;
				
			for ( ; i<prog ; ++i)
				barre[i]='-';
				
			barre[prog]='\0';
				
			nanosleep(&time,NULL);	
			fflush(stdout);
			printf("\r%s%d%%",barre,prog*4);
			
				
			
		}
		
        close(sock) ;
        close(fic) ;
        nbLu = 1 ;
	printf("\nEnvoyer un autre fichier (O/N) ? ");
	scanf("%*c%c",&next);
		
	}
	
	
	close(sock);
	close(fic);
	
	return 0;
}
	

