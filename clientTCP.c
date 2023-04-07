#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include <time.h>
#include<ncurses.h>
#include<string.h>
#include <unistd.h>
#include<arpa/inet.h>
#include "common.h"

#define XMAX 80
#define YMAX 30
#define NB_COLORS 5
#define TRAIL_INDEX_SHIFT 50

#define BLUE_ON_BLACK       0
#define RED_ON_BLACK        2
#define YELLOW_ON_BLACK     1
#define MAGENTA_ON_BLACK    3
#define CYAN_ON_BLACK       4

#define BLUE_ON_BLUE        50
#define RED_ON_RED          52
#define YELLOW_ON_YELLOW    51
#define MAGENTA_ON_MAGENTA  53
#define CYAN_ON_CYAN        54



int ecoute_desc (int socket) {  // Cette fonction permet d'ecouter en meme temps sur le socket et 
  TV tv;                               // et sur l'entree standard
  fd_set read_fd;                     // Elle renvoie 1 s'il y'a quelque choz sur entree standard
                                      // Elle renvoie 2 s'il y'a quelque choz sur le socket , 0 s'il ya ereur
  tv.tv_sec=0;
  tv.tv_usec=0;
  FD_ZERO(&read_fd);
  FD_SET(0,&read_fd);
  FD_SET(socket,&read_fd);


  if(select(socket+1, &read_fd, NULL, NULL, &tv) == -1)
    return 0;

  if(FD_ISSET(0,&read_fd))
    return 1;
  if(FD_ISSET(socket,&read_fd))
    return 2;

  return 0;
}

void display_character(int color, int y, int x, char character);
void tune_terminal();
void init_graphics();
void srand();
int main(int argc, char* argv[])
{

    init_graphics();
    srand(time(NULL));
    char board[XMAX][YMAX];

   struct client_input E_S;
   //display_info Rinfo;
   display_info info;

 
  int keyPressed = 0;   /* le Touche appuyee */
  int socket_cli;
  SAI serveur;
  fd_set ensbl,tmp;
  FD_ZERO(&ensbl);


  //Intialisation du socket
  socket_cli = socket(AF_INET,SOCK_STREAM,0);
  if (socket_cli==-1)
  {
    perror("echec");
    exit(-1);
  }
   //control des arguments
   if(argc != 4)
    {
      printf("Usage : %s @dest num_port chaine_a_envoyer\n", argv[0]);
      exit(-1);
    }
    FD_SET(socket_cli,&ensbl); /* Ajoute descripteur a ensemble. */
    FD_SET(0,&ensbl);           /* Ajoute descripteur entree standart a ensemble. */
  
  serveur.sin_family=AF_INET;
  serveur.sin_port = htons(atoi(argv[2]));
  serveur.sin_addr.s_addr = inet_addr(argv[1]);
  socklen_t addrlen = sizeof(SAI);
  
  //Demende de connexion
  if (connect(socket_cli,(SA*)&serveur,addrlen) == -1)
  {
    perror("echec connexion");
    exit(-1);
  }
    char* leur_msg  = calloc(100,sizeof(char*));
    tmp = ensbl;
    //envoie des nombre de jouers
 if(send(socket_cli,argv[3],sizeof(argv[3]),0)==-1)
  {
     printf("L'envoie echoue \n");
     exit(0);
  }

   while (1)
   {
      select(socket_cli+1,&tmp,NULL,NULL,NULL);
      if (FD_ISSET(socket_cli,&tmp))
      {
         if(recv(socket_cli,&board,sizeof(display_info),0)>0)
        {
          if (strcmp(leur_msg,"wait")==0) //on va attendre l'autre joueur 
              //si on a envoie 1 comme nbre de joueur le serveur nous envoie "wait"
            // printf("");
             mvaddstr(YMAX/2, XMAX/4, "On attend l'autre joueur");
          else { //si on a envoie 2 comme nbre de joueur le serveur demarre la partie
           clear();
            for (size_t i = 0; i < YMAX; i++) {
             for (size_t j = 0; j < XMAX; j++) {
                if (i == 0 || i == YMAX-1) {
                   display_character(WALL, i, j, ACS_VLINE);
                }
                else if (j == 0 || j == XMAX-1) {
                     display_character(WALL, i, j, ACS_HLINE);
                } 
               else
                {
                  if (board[j][i] == 50)   
                    display_character(RED_ON_BLACK+TRAIL_INDEX_SHIFT , i, j, '#');
                  if (board[j][i] == 1)  
                    display_character(RED_ON_BLACK, i, j, '#');
                  if (board[j][i] == 2)  
                    display_character(BLUE_ON_BLACK , i, j, '@');
                  if(board[j][i] == 51 ) 
                    display_character(BLUE_ON_BLACK+TRAIL_INDEX_SHIFT , i, j, '@');
                }
                  
            }
  }
 
 while (1) {
   
      refresh();    
    if (ecoute_desc(socket_cli)==1) { // On ecoute sur l'entree standard
      keyPressed = getch();
      if (keyPressed == 'q' || keyPressed == 's' || keyPressed == 'z' || keyPressed == 'd')
      {
        E_S.id = 1;
      }
      if (keyPressed == 'm') // pour ne pas  afficher le lightcycle du joueur #
        {
           E_S.id = 1;
        }
      if (keyPressed == 'i' || keyPressed == 'j' || keyPressed == 'k' || keyPressed == 'l')
      {
        E_S.id = 2;
      }
       if (keyPressed == ' ') // pour ne pas afficher le lightcycle du joueur @
        {
         E_S.id = 2;
        }
      E_S.input = keyPressed;
                     
        if(send(socket_cli,&E_S,sizeof(E_S),0)==-1) // on envoie la touche appuye au serveur
        {
          printf("L'envoie echoue \n");
          exit(0);
        } 
        
    }
    if (ecoute_desc(socket_cli)==2) // On ecoute si le socket a recu quelque chose
    {
      if(recv(socket_cli,&info,sizeof(info),0)>0)
        {
        for (size_t i = 1; i < YMAX; i++) {
             for (size_t j = 1; j < XMAX; j++) {
               if (info.board[j][i] == 50) 
                {
                  display_character(RED_ON_BLACK+TRAIL_INDEX_SHIFT , i, j, 'X');
                } 
                if (info.board[j][i] == 1) 
                {
                  display_character(BLUE_ON_BLACK, i, j, '#');
                }
                if (info.board[j][i] == 2) 
                 {
                  display_character(BLUE_ON_BLACK, i, j, '@');
                 }
               if( info.board[j][i] == 51 ) 
                {
                  display_character(BLUE_ON_BLACK+TRAIL_INDEX_SHIFT , i, j, 'Y');
                }

               // Affichage du joueur qui a gagne
               if (info.winner==1)
                 mvaddstr(YMAX/2, XMAX/4, "Le joueur # a gagnee");
               if(info.winner==2)
                 mvaddstr(YMAX/2, XMAX/4, "Le joueur @ a gagnee");
               if(info.winner==3)
                 mvaddstr(YMAX/2, XMAX/4, "Aucun Joueur n'a gagne, matche nul");
            }           
          }
            mvaddstr(0, XMAX/2 - strlen("C-TRON")/2, "C-TRON");
     }
    sleep(1);
    refresh();
  }      
 }
 
}
  }
 
  }
           
 }  
    tmp = ensbl;  
    close(socket_cli);
     
 return 0; 
}
