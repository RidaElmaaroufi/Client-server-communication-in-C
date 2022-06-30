#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define LISTER 1
#define QUITTER 10

int display_menu(char const *program)
{
	int choix = QUITTER;

	system("clear");
	printf("========================================================\n");
	printf("|                REPERTOIRE TELEPHONIQUE               |\n");
	printf("========================================================\n");
	printf("| 1. Lister les contacts                               |\n");
	printf("| 2. Affichage arbitraire                              |\n");
	printf("| 3. Affichage trie                                    |\n");
	printf("| 4. Ajouter un institut                               |\n");
	printf("| 5. Supprimer un institut ou un fichier               |\n");
	printf("| 6. Ajouter un contact                                |\n");
	printf("| 7. Rechercher un contact                             |\n");
	printf("| 8. Modifier un contact                               |\n");
	printf("| 9. Supprimer un contact                              |\n");
	printf("| 10. Quitter                                          |\n");
	printf("========================================================\n");
	do
	{
		printf("| Entrez votre choix : ");
		scanf("%d", &choix);
		printf("\n");
		if (choix == QUITTER)
		{
			printf("Au revoir !\n");
			execl("/usr/bin/pkill", "pkill", "-9", program, NULL);
			exit(0);
		}
	} while (choix < LISTER || choix > QUITTER);
	return choix;
}

int main(int argc, char const *argv[])
{
	struct sockaddr_in addr;

	if (argc != 3)
	{
		printf("Usage: %s <ip> <port>\n", argv[0]);
		return 1;
	}

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror("socket error!");
		return 1;
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	addr.sin_addr.s_addr = inet_addr(strcmp("localhost", argv[1]) == 0 ? "127.0.0.1" : argv[1]);
	printf("Connexion avec le serveur %s:%s ...\n", argv[1], argv[2]);
	int conn = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
	if (conn == -1)
	{
		perror("connect error!");
		return 1;
	}
	printf("Connection reussie\n");

	while (1)
	{
		int choix = display_menu(argv[0]);
		send(sock, &choix, sizeof(choix), 0);
		system("clear");
		if (!fork())
		{
			while (1)
			{
				char buff[1024];
				int n = recv(sock, buff, sizeof(buff), 0);
				if (n <= 0)
					break;
				buff[n] = '\0';
				if (buff[n - 1] == '$')
				{
					buff[n - 1] = '\0';
					write(1, buff, n);
					break;
				}
				else
					write(1, buff, n);
			}
		}
		else
		{
			while (1)
			{
				char buff[1024];
				int n = read(0, buff, sizeof(buff));
				if (n <= 0)
					break;
				send(sock, buff, n, 0);
			}
		}
		printf("\n\nAppuyez sur une touche pour continuer ...\n");
		getchar();
		getchar();
	}
	return 0;
}
