#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

#define LISTER 1
#define AFF_ARB 2
#define AFF_TRI 3
#define AJOUTER_INST 4
#define SUPPRIMER_REP_FIC 5
#define AJOUTER_PER 6
#define RECHERCHER_PER 7
#define MODIFIER_PER 8
#define SUPPRIMER_PER 9
#define QUITTER 10
#define REPERTOIRE "./repertoire_telephonique"
int c_socket;
struct sockaddr_in cliaddr;
int clilen;

void console_log(char *str)
{
	printf("[%s]\t%s\n", inet_ntoa(cliaddr.sin_addr), str);
}

int respond(char *message)
{
	return send(c_socket, message, strlen(message), 0);
}

char *strjoin(char const *s1, char const *s2)
{
	char *str;
	int l1;
	int i;

	l1 = 0;
	i = 0;
	if (!s1 || !s2)
		return (NULL);
	str = (char *)malloc((strlen(s1) + strlen(s2) + 1) * sizeof(char));
	if (!str)
		return (NULL);
	while (s1[l1])
	{
		str[l1] = s1[l1];
		l1++;
	}
	while (s2[i])
	{
		str[l1 + i] = s2[i];
		i++;
	}
	str[l1 + i] = '\0';
	return (str);
}

void lister()
{
	struct dirent *dirent;
	DIR *dir;

	dir = opendir(REPERTOIRE);
	if (dir == NULL)
	{
		send(c_socket, "Repertoire inexistant\n", 24, 0);
		return;
	}
	send(c_socket, "Lister le contenu du repertoire telephonique:\n", 47, 0);
	send(c_socket, "*********************************************\n", 47, 0);
	int k = 0;//compteur
	while ((dirent = readdir(dir)) != NULL)
	{
		if (dirent->d_name[0] != '.')
		{
			char *str = strjoin(REPERTOIRE, "/");
			str = strjoin(str, dirent->d_name);
			send(c_socket, dirent->d_name, strlen(dirent->d_name), 0);
			send(c_socket, "\n", 1, 0);
			DIR *dir2 = opendir(str);
			if (dir2 != NULL)
			{
				struct dirent *dirent2;
				while ((dirent2 = readdir(dir2)) != NULL)
				{
					if (dirent2->d_name[0] != '.')
					{
						char *str2 = strjoin(str, "/");
						str2 = strjoin(str2, dirent2->d_name);
						send(c_socket, "  ", 2, 0);
						send(c_socket, dirent2->d_name, strlen(dirent2->d_name), 0);
						send(c_socket, "\n", 1, 0);
						k++;
					}
				}
			}
			free(str);
			k++;
		}
	}
	if (k == 0)
		send(c_socket, "Repertoire vide\n", 18, 0);
}

void afficher_arb()
{
	send(c_socket, "Affichage arbitraire:\n", 23, 0);
	send(c_socket, "=====================\n", 23, 0);
	char *nom_institut = (char *)malloc(sizeof(char) * 20);
	int num_fichier = 5;
	FILE *fichier;

	send(c_socket, "Donnez le nom du fichier: ", 30, 0);
	int n = recv(c_socket, nom_institut, 20, 0);
	nom_institut[n - 1] = '\0';
	do
	{
		char buff[20];
		send(c_socket, "Donnez le numero de fichier (1 ou 2): ", 39, 0);
		recv(c_socket, buff, sizeof(buff), 0);
		num_fichier = atoi(buff);
	} while (num_fichier != 1 && num_fichier != 2);
	send(c_socket, "\n", 1, 0);
	char *path = strjoin("", "./repertoire_telephonique/");
	path = strjoin(path, nom_institut);
	path = strjoin(path, "/fichier");
	path = strjoin(path, num_fichier == 1 ? "1" : "2");
	path = strjoin(path, ".txt");

	fichier = fopen(path, "r");
	if (fichier == NULL)
	{
		send(c_socket, "Erreur:fichier introuvable.\n", 42, 0);
		return;
	}
	char *ligne = (char *)malloc(sizeof(char) * 100);
	while (fgets(ligne, 100, fichier) != NULL)
	{
		send(c_socket, ligne, strlen(ligne), 0);
	}
	send(c_socket, "\n", 1, 0);
}

void afficher_tri()
{
	send(c_socket, "Affichage Trié:\n", 18, 0);
	send(c_socket, "===============\n", 17, 0);
	char *nom_institut = (char *)malloc(sizeof(char) * 20);
	int num_fichier = 5;
	FILE *fichier;

	send(c_socket, "Donnez le nom de l'institut: ", 30, 0);
	int n = recv(c_socket, nom_institut, 20, 0);
	nom_institut[n - 1] = '\0';
	do
	{
		char buff[20];
		send(c_socket, "Donnez le numero de fichier (1 ou 2): ", 39, 0);
		recv(c_socket, buff, sizeof(buff), 0);
		num_fichier = atoi(buff);
	} while (num_fichier != 1 && num_fichier != 2);
	send(c_socket, "\n", 1, 0);
	char *path = strjoin("", "./repertoire_telephonique/");
	path = strjoin(path, nom_institut);
	path = strjoin(path, "/fichier");
	path = strjoin(path, num_fichier == 1 ? "1" : "2");
	path = strjoin(path, ".txt");

	fichier = fopen(path, "r");
	if (fichier == NULL)
	{
		send(c_socket, "Erreur: Institut ou fichier introuvable.\n", 42, 0);
		return;
	}
	system(strjoin(strjoin("sort ", path), " > .tmp"));
	char *ligne = (char *)malloc(sizeof(char) * 100);
	FILE *tmp = fopen(".tmp", "r");
	while (fgets(ligne, 100, tmp) != NULL)
	{
		send(c_socket, ligne, strlen(ligne), 0);
	}
	system("rm .tmp");
	send(c_socket, "\n", 1, 0);
}

void ajouter_inst()
{
	send(c_socket, "Ajouter un institut:\n", 22, 0);
	send(c_socket, "=====================\n", 23, 0);
	char nom_institut[20];
	send(c_socket, "Donnez le nom de l'institut: ", 30, 0);
	int n = recv(c_socket, nom_institut, 20, 0);
	nom_institut[n - 1] = '\0';
	send(c_socket, "\n", 1, 0);
	char *path = strjoin("", "./repertoire_telephonique/");
	path = strjoin(path, nom_institut);
	char *path1 = strjoin(path, "/fichier1.txt");
	char *path2 = strjoin(path, "/fichier2.txt");

	int res = mkdir(path, S_IRWXU);
	if (res != 0)
	{
		send(c_socket, "Erreur: Creation du repertoire ", 32, 0);
		send(c_socket, nom_institut, strlen(nom_institut), 0);
		send(c_socket, " impossible.\n", 14, 0);
		return;
	}
	res = creat(path1, S_IRWXU);
	if (res == -1)
	{
		send(c_socket, "Erreur: Creation du fichier 1 impossible.\n", 43, 0);
		return;
	}
	res = creat(path2, S_IRWXU);
	if (res == -1)
	{
		send(c_socket, "Erreur: Creation du fichier 2 impossible.\n", 43, 0);
		return;
	}
	send(c_socket, "Institut ", 9, 0);
	send(c_socket, nom_institut, strlen(nom_institut), 0);
	send(c_socket, " ajouté.\n", 10, 0);
}

void supprimer_rep_fic()
{
	respond("Supprimer un institut ou un fichier:\n");
	respond("=====================================\n");
	respond("1. Supprimer un institut\n");
	respond("2. Supprimer un fichier\n");
	respond("\n");
	int choix;
	do
	{
		char buff[20];
		respond("Entrez votre choix : ");
		recv(c_socket, buff, sizeof(buff), 0);
		choix = atoi(buff);
		respond("\n");
	} while (choix != 1 && choix != 2);
	char nom_institut[20];
	respond("Donnez le nom de l'institut: ");
	int n = recv(c_socket, nom_institut, 20, 0);
	nom_institut[n - 1] = '\0';
	respond("\n");

	char *path = strjoin("", "./repertoire_telephonique/");
	path = strjoin(path, nom_institut);
	char *path1 = strjoin(path, "/fichier1.txt");
	char *path2 = strjoin(path, "/fichier2.txt");
	if (choix == 1)
	{
		unlink(path1);
		unlink(path2);
		int res = rmdir(path);
		if (res != 0)
		{
			respond("Erreur: Suppression du repertoire ");
			respond(nom_institut);
			respond(" impossible.\n");
			return;
		}
		respond("Institut ");
		respond(nom_institut);
		respond(" supprimé.");
	}
	else
	{
		respond("1. Supprimer le fichier 1\n");
		respond("2. Supprimer le fichier 2\n");
		respond("\n");
		int choix2;
		do
		{
			char buff[20];
			respond("Entrez votre choix : ");
			recv(c_socket, buff, sizeof(buff), 0);
			choix2 = atoi(buff);
			respond("\n");
		} while (choix2 != 1 && choix2 != 2);
		if (choix2 == 1)
		{
			int res = remove(path1);
			if (res != 0)
			{
				respond("Erreur: Suppression du fichier 1 impossible.\n");
				return;
			}
			respond("Fichier 1 supprime.\n");
		}
		else
		{
			int res = remove(path2);
			if (res != 0)
			{
				respond("Erreur: Suppression du fichier 2 impossible.\n");
				return;
			}
			respond("Fichier 2 supprime.\n");
		}
	}
}

void ajouter_contact()
{
	FILE *file;

	respond("Ajouter un contact:");
	respond("===================\n");
	char nom_institut[20];
	respond("Donnez le nom de l'institut: ");
	int n = recv(c_socket, nom_institut, 20, 0);
	nom_institut[n - 1] = '\0';
	respond("\n");
	respond("1. Ajouter dans le fichier 1\n");
	respond("2. Ajouter dans le fichier 2\n");
	int choix;
	do
	{
		char buff[20];
		respond("Entrez votre choix : ");
		recv(c_socket, buff, sizeof(buff), 0);
		choix = atoi(buff);
		respond("\n");
	} while (choix != 1 && choix != 2);
	char *path = strjoin("", "./repertoire_telephonique/");
	path = strjoin(path, nom_institut);
	path = strjoin(path, "/fichier");
	path = strjoin(path, choix == 1 ? "1" : "2");
	path = strjoin(path, ".txt");
	int fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		respond("Erreur: Institut ou fichier introuvable ou inaccessible.\n");
		return;
	}
	file = fopen(path, "a");
	char nom[20];
	char prenom[20];
	char tel[20];
	respond("Donnez le nom: ");
	n = recv(c_socket, nom, 20, 0);
	nom[n - 1] = '\0';
	respond("Donnez le prenom: ");
	n = recv(c_socket, prenom, 20, 0);
	prenom[n - 1] = '\0';
	respond("Donnez le numero du telephone: ");
	n = recv(c_socket, tel, 20, 0);
	tel[n - 1] = '\0';
	int res = fprintf(file, "%s %s %s\n", nom, prenom, tel);
	if (res < 0)
	{
		respond("Erreur: Ecriture dans le fichier est impossible\n");
		return;
	}
	respond("Contact ajoute!\n");
	fclose(file);
}

void rechercher_contact()
{
	FILE *file;

	respond("Rechercher un contact:\n");
	respond("**********************\n");

	char nom_institut[20];
	respond("Donnez le nom de l'institut: ");
	int n = recv(c_socket, nom_institut, 20, 0);
	nom_institut[n - 1] = '\0';
	respond("\n");
	respond("1. Rechercher dans le fichier 1\n");
	respond("2. Rechercher dans le fichier 2\n");
	int choix;
	do
	{
		char buff[20];
		respond("Entrez votre choix : ");
		recv(c_socket, buff, sizeof(buff), 0);
		choix = atoi(buff);
		respond("\n");
	} while (choix != 1 && choix != 2);
	char *path = strjoin("", "./repertoire_telephonique/");
	path = strjoin(path, nom_institut);
	path = strjoin(path, "/fichier");
	path = strjoin(path, choix == 1 ? "1" : "2");
	path = strjoin(path, ".txt");
	int fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		respond("Erreur: fichier introuvable ou inaccessible.\n");
		return;
	}
	file = fopen(path, "r");
	respond("Donnez le nom, prenom ou le numero de telephone: ");
	char recherche[20];
	n = recv(c_socket, recherche, 20, 0);
	recherche[n - 1] = '\0';
	char nom[20];
	char prenom[20];
	char tel[20];
	int flag = 0;
	while (fscanf(file, "%s %s %s\n", nom, prenom, tel) != EOF)
	{
		if (!strcmp(nom, recherche) || !strcmp(prenom, recherche) || !strcmp(tel, recherche))
		{
			if (flag == 0)
			{
				respond("\nResultat:\n");
				respond("---------------------------------------------\n");
				respond("Nom\t\tPrenom\t\tTel\n");
				respond("---------------------------------------------\n");
			}
			flag = 1;
			respond(nom);
			respond("\t\t");
			respond(prenom);
			respond("\t\t");
			respond(tel);
			respond("\n");
		}
	}
	if (flag == 0)
		respond("\nContact n'existe pas!\n");
	fclose(file);
}

void modifier_contact()
{
	FILE *file;
	FILE *tmp;

	respond("Modifier un contact:\n");
	respond("********************\n");

	char nom_institut[20];
	respond("Donnez le nom de l'institut: ");
	int n = recv(c_socket, nom_institut, 20, 0);
	nom_institut[n - 1] = '\0';
	respond("\n");
	respond("1. Modifier dans le fichier 1\n");
	respond("2. Modifier dans le fichier 2\n");
	int choix;
	do
	{
		char buff[20];
		respond("Entrez votre choix : ");
		n = recv(c_socket, buff, sizeof(buff), 0);
		choix = atoi(buff);
		respond("\n");
	} while (choix != 1 && choix != 2);
	char *path = strjoin("", "./repertoire_telephonique/");
	path = strjoin(path, nom_institut);
	path = strjoin(path, "/fichier");
	path = strjoin(path, choix == 1 ? "1" : "2");
	path = strjoin(path, ".txt");
	int fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		respond("Erreur: Institut ou fichier introuvable ou inaccessible.\n");
		return;
	}
	file = fopen(path, "r");
	tmp = fopen("tmp", "w");
	respond("Donnez le nom, prenom ou le numero de telephone: ");
	char recherche[20];
	n = recv(c_socket, recherche, 20, 0);
	recherche[n - 1] = '\0';
	char nom[20];
	char prenom[20];
	char tel[20];
	int flag = 0;
	while (!feof(file))
	{
		fscanf(file, "%s %s %s\n", nom, prenom, tel);
		if (!strcmp(nom, recherche) || !strcmp(prenom, recherche) || !strcmp(tel, recherche))
		{
			flag = 1;
			respond("\nContact trouve: ");
			respond(nom);
			respond(" ");
			respond(prenom);
			respond(" ");
			respond(tel);
			respond("\n\n");
			respond("1. Modifier le nom.\n");
			respond("2. Modifier le prenom.\n");
			respond("3. Modifier le numero du telephone.\n");
			respond("\n");
			int choix;
			do
			{
				char buff[20];
				respond("Entrez votre choix: ");
				recv(c_socket, buff, sizeof(buff), 0);
				choix = atoi(buff);
			} while (choix < 1 || choix > 3);
			switch (choix)
			{
			case 1:
				respond("Donnez le nouveau nom: ");
				char nv_nom[20];
				n = recv(c_socket, nv_nom, 20, 0);
				nv_nom[n - 1] = '\0';
				fprintf(tmp, "%s %s %s\n", nv_nom, prenom, tel);
				break;
			case 2:
				respond("Donnez le nouveau prenom: ");
				char nv_prenom[20];
				n = recv(c_socket, nv_prenom, 20, 0);
				nv_prenom[n - 1] = '\0';
				fprintf(tmp, "%s %s %s\n", nom, nv_prenom, tel);
				break;
			case 3:
				respond("Donnez le nouveau nom: ");
				char nv_tel[20];
				n = recv(c_socket, nv_tel, 20, 0);
				nv_tel[n - 1] = '\0';
				fprintf(tmp, "%s %s %s\n", nom, prenom, nv_tel);
				break;
			}
		}
		else
		{
			fprintf(tmp, "%s %s %s\n", nom, prenom, tel);
		}
	}
	if (flag == 0)
	{
		respond("\nContact n'existe pas!\n");
		fclose(tmp);
		remove("tmp");
		return;
	}
	fclose(file);
	fclose(tmp);
	remove(path);
	rename("tmp", path);
	respond("\nContact modifie!\n");
}

void supprimer_contact()
{
	FILE *file;
	FILE *tmp;

	respond("Supprimer un contact:\n");
	respond("====================\n");

	char nom_institut[20];
	respond("Donnez le nom de l'institut: ");
	int n = recv(c_socket, nom_institut, 20, 0);
	nom_institut[n - 1] = '\0';
	respond("\n");
	respond("1. Ajouter dans le fichier 1\n");
	respond("2. Ajouter dans le fichier 2\n");
	int choix;
	do
	{
		char buff[20];
		respond("Entrez votre choix : ");
		n = recv(c_socket, buff, sizeof(buff), 0);
		choix = atoi(buff);
		respond("\n");
	} while (choix != 1 && choix != 2);
	char *path = strjoin("", "./repertoire_telephonique/");
	path = strjoin(path, nom_institut);
	path = strjoin(path, "/fichier");
	path = strjoin(path, choix == 1 ? "1" : "2");
	path = strjoin(path, ".txt");
	int fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		respond("Erreur: Institut ou fichier introuvable ou inaccessible.\n");
		return;
	}
	file = fopen(path, "r");
	tmp = fopen("tmp", "w");
	respond("Donnez le nom, prenom ou le numero de telephone: ");
	char recherche[20];
	n = recv(c_socket, recherche, 20, 0);
	recherche[n - 1] = '\0';
	char nom[20];
	char prenom[20];
	char tel[20];
	int flag = 0;
	while (!feof(file))
	{
		fscanf(file, "%s %s %s\n", nom, prenom, tel);
		if (!strcmp(nom, recherche) || !strcmp(prenom, recherche) || !strcmp(tel, recherche))
		{
			flag = 1;
			respond("\nContact trouve: ");
			respond(nom);
			respond(" ");
			respond(prenom);
			respond(" ");
			respond(tel);
			respond("\n\n");
		}
		else
		{
			fprintf(tmp, "%s %s %s\n", nom, prenom, tel);
		}
	}
	if (flag == 0)
	{
		respond("\nContact n'existe pas!\n");
		fclose(tmp);
		remove("tmp");
		return;
	}
	fclose(file);
	fclose(tmp);
	remove(path);
	rename("tmp", path);
	respond("\nContact supprime!\n");
}

int main(int argc, char const *argv[])
{
	struct sockaddr_in servaddr;

	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		return 1;
	}

	int s_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (s_socket == -1)
	{
		perror("socket error!");
		return 1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[1]));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int b = bind(s_socket, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (b == -1)
	{
		perror("bind error!");
		return 1;
	}

	int l = listen(s_socket, 1);
	if (l == -1)
	{
		perror("listen error!");
		return 1;
	}
	printf("[SERVER]\tDémarré sur le port %s ...\n", argv[1]);

	c_socket = accept(s_socket, (struct sockaddr *)&cliaddr, &clilen);
	if (c_socket == -1)
	{
		perror("accept error!");
		return 1;
	}
	console_log("Connection établie.");

	while (1)
	{
		int choix;
		recv(c_socket, &choix, sizeof(choix), 0);
		if (choix == -1)
			continue;
		switch (choix)
		{
		case LISTER:
			console_log("Lister le contenu du repertoire.");
			lister();
			break;
		case AFF_ARB:
			console_log("Afficheage arbitraire.");
			afficher_arb();
			break;
		case AFF_TRI:
			console_log("Affichage trié.");
			afficher_tri();
			break;
		case AJOUTER_INST:
			console_log("Ajouter un institut.");
			ajouter_inst();
			break;
		case SUPPRIMER_REP_FIC:
			console_log("Supprimer un repertoire et son contenu.");
			supprimer_rep_fic();
			break;
		case AJOUTER_PER:
			console_log("Ajouter une personne.");
			ajouter_contact();
			break;
		case RECHERCHER_PER:
			console_log("Rechercher un contact.");
			rechercher_contact();
			break;
		case MODIFIER_PER:
			console_log("Modifier un contact.");
			modifier_contact();
			break;
		case SUPPRIMER_PER:
			console_log("Supprimer un contact.");
			supprimer_contact();
			break;
		}
		respond("$");
		choix = -1;
	}
	return 0;
}
