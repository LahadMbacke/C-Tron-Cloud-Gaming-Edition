#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdlib.h>
#include<netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include<ncurses.h>
#include<arpa/inet.h>
#include <sys/time.h>
#include "common.h"


int collision(int x,int y,int x1,int y1, struct display_info board)
{
      if ( x<0 || x == XMAX-1) {   // on verifie si le joueur 1 cogne le mur
        return 1;
      }
      else if (y<0 || y == YMAX -1) { // on verifie si le joueur 1 cogne le mur
        return 1;
     }
    
     if (x1<0 || x1 == XMAX -1) {  // on verifie si le joueur 2 cogne le mur
        return 2;
      }
      else if (y1<0 || y1 == YMAX-1) {  // on verifie si le joueur 2 cogne le mur
        return 2;
     }
     if (board.board[x][y]==51) // on verifie si le joueur 1 a heurte le joueur 2
          return 1;  
      if (board.board[x1][y1]==50) //on verifie si le joueur 2 a heurte le joueur 1
          return 2;  
      if (board.board[x][y]==2 || board.board[x1][y1]==1)//on verifie si la tete des 2 joueur se heurte 
              return 3;
    
   return 0;
}

int main(int argc, char* argv[])
{
    int socket_serv,socket_cli,sd,max,client[2];
    int tabsock[2]; // va contenir les socket clients apres connexion
    TV tv;
    tv.tv_sec=0;
    tv.tv_usec=0;

  //control des arguments
   if(argc != 3)
    {
      printf("Verifier les arguments\n");
      exit(-1);
    }
  
      char board[XMAX][YMAX] = {{0, 0},{0, 0}};
      struct display_info Rinfo = {};
      struct client_init_infos nb_player;
      struct client_input cli_input;
      SAI addr_serv,addr_cli;
      fd_set ensbl;  // declaration de fd_set
      FD_ZERO(&ensbl);   // Vide ensbl
      bool affiche_j1 = TRUE; /* Pour afficher ou non le lightcycle du joueur 1*/
      bool affiche_j2 = TRUE;  /* Pour afficher ou non le lightcycle du joueur 2*/
      int cpt1 = 1,cpt2 = 1;  


      int x,y,x1,y1,oldX,oldY,oldX1,oldY1;
       x = 2; // x y les coordonnees initial du joueur 1 #
       y = 2; 
       oldX = x+1;  // oldX oldY ces coordonnees sont utilise pour
                   // demarrer le jeu sans un input initial du joueur 1 #
       oldY = y;
       y1 = 15;  // x1 y1 les coordonnees initial du joueur 2 @
       x1 = 16;
       oldX1 = x1+1; // meme chose que oldX oldY
       oldY1 = y1;

    socket_serv = socket(AF_INET,SOCK_STREAM,0);
    if (socket_serv <0)
    {
        printf("Erreur de creation socket");
        exit(-1);
    }
    
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(atoi(argv[1]));
    addr_serv.sin_addr.s_addr = inet_addr("127.0.0.1"); // l'adresse IP du serveur
    socklen_t addrlen = sizeof(SAI);


    if (bind(socket_serv,(SA*)&addr_serv,addrlen)<0) // on fait la liaision
    {
      printf("Echec de bind\n");
      exit(-1);
    }
    if (listen(socket_serv,2) < 0)
      {
          printf("Echec d'ecoute");
          exit(-1);
      }


       char* leur_msg =  (char*)malloc(1); // va contenir l'argumnent nombre de joueur
       int nbj =0;  // va contenir leur_msg converti en entier avec atoi()
       nb_player.nb_players =0;
       max = socket_serv;
       FD_SET(socket_serv,&ensbl); /* Ajoute descripteur a ensemble. */

         board[x][y] = 1; // Pour dire 1 correspond au joueur 1 #
         board[x1][y1] = 2; //Pour dire 2 correspond au joueur 1 @
        

      int k=0; // sera incremente en 1 si le nombr de joueur passe en argument est 1
      while (nb_player.nb_players < 2 && nbj!=2) // on sort de la boucle que si 2 joueur sont presents
      {
         printf("Le serveur est en ecoute ....\n");
          //Ceci concerne une demande de connexoin
          if (FD_ISSET(socket_serv,&ensbl))
          {
              socket_cli = accept(socket_serv,(SA *)&addr_cli,&addrlen);
              tabsock[k]=socket_cli; // on met le socket du client accepte dans tabsbock
              FD_SET(socket_serv,&ensbl); 
                if (socket_cli <0)
                {
                  printf("Echec au niveau de listen");
                  exit(-1);
                }
                /* on met le socket du client accepte dans le tableau client, celle ci 
                 va etre utilise pour donner les infos aux clients*/
                client[nb_player.nb_players] = socket_cli; 
                printf("client : %d\n",client[nb_player.nb_players]);
               if(recv(socket_cli,leur_msg,sizeof(leur_msg),0) > 0 )
               {
                 nbj = atoi(leur_msg); // l'arguemnt nombr de jour recu est mis dans nbj;
                   if (nbj == 2) //on verifi si l'argument correspond a 2 cad dire sur le meme terminal
                    {
                      nb_player.nb_players = nbj;
                      printf("Le Jeu demarre\n");
                         if(send(socket_cli,&board,sizeof(board),0)==-1)
                         {
                          printf("L'envoie echoue \n");
                          }    
                    }
                    else // sinon cad chaque joueur sur  terminal
                    {
                      nb_player.nb_players = nb_player.nb_players + nbj;
                      if (nb_player.nb_players == 2) //s'il connect tous les 2 le jeu demarre
                        {
                          printf("Le Jeu demarre\n");
                          for (int j = 0; j < nb_player.nb_players; j++)
                          {
                            if(send(tabsock[j],&board,sizeof(board),0)==-1)
                            {
                              printf("L'envoie echoue \n");
                            }
                          }                         
                    }
                  }
              } 
              else
              {
                printf("Echec recevoir\n");
              }
                     
       }
       k = k+1; // ceci est incremente seulment si le client envoie un seul joueur(meme terminal)
      }
  
     printf("*******************GOOOOOOOOOOOOOOOOOOOOOOOOOO***********************\n");
     printf("*******************GOOOOOOOOOOOOOOOOOOOOOOOOOO***********************\n");
     printf("*******************GOOOOOOOOOOOOOOOOOOOOOOOOOO***********************\n");
     printf("*******************GOOOOOOOOOOOOOOOOOOOOOOOOOO***********************\n");

    while (1)
    {
          max = socket_serv; // on considere ici que socket_serv est le maximum
      for ( int i = 0 ; i <nb_player.nb_players; i++)
        {
          // descripteur socket client
          sd = client[i];    
          //si le socket est nb_player.nb_players  on l'ajoute dans le descripteur de socket
          if(sd > 0)
            FD_SET(sd,&ensbl);
          //on cherche le plus grand descripteur
          if(sd > max)
            max = sd;
        }  
        /* Ici le jeu demarre et ca continue jusqu'a ce que le client fait un input
          pour changer de direction*/ 
            if(oldY == y)
            {
              if(affiche_j1==TRUE){ Rinfo.board[x][y] = 50;}
               y = y +1;
               oldY = y;
               if (collision(x,y,x1,y1,Rinfo)==1)
                  Rinfo.winner=2;
              else if (collision(x,y,x1,y1,Rinfo)==2)
                  Rinfo.winner=1;
              else if (collision(x,y,x1,y1,Rinfo)==3)
                  Rinfo.winner=3; 
              Rinfo.board[x][y] = 1; 
              printf("coordonnes joueur 1: X = %d,Y = %d\n",x,y);
            }
          
          if(oldX == x)
          {
              if(affiche_j1==TRUE)
              { 
                 Rinfo.board[x][y] = 50;
              }
              x = x +1;
              oldX = x;
             if (collision(x,y,x1,y1,Rinfo)==1)
                  Rinfo.winner=2;
              else if (collision(x,y,x1,y1,Rinfo)==2)
                  Rinfo.winner=1;
             else if (collision(x,y,x1,y1,Rinfo)==3)
                    Rinfo.winner=3; 
               
              Rinfo.board[x][y] = 1;
               printf("coordonnes joueur 1: X = %d,Y = %d\n",x,y);
            }
             if(oldY == -1)
            {
              if(affiche_j1==TRUE){ Rinfo.board[x][y] = 50;}
              x = x -1;
             if (collision(x,y,x1,y1,Rinfo)==1)
                  Rinfo.winner=2;
              else if (collision(x,y,x1,y1,Rinfo)==2)
                  Rinfo.winner=1;
              else if (collision(x,y,x1,y1,Rinfo)==3)
                          Rinfo.winner=3; 
              Rinfo.board[x][y] = 1;
              printf("coordonnes joueur 1: X = %d,Y = %d\n",x,y);
             }
             if(oldX == -1)
             {
               if(affiche_j1==TRUE){ Rinfo.board[x][y] = 50;}
               y = y -1;

              if (collision(x,y,x1,y1,Rinfo)==1)
                  Rinfo.winner=2;
              else if (collision(x,y,x1,y1,Rinfo)==2)
                  Rinfo.winner=1;
              else if (collision(x,y,x1,y1,Rinfo)==3)
                  Rinfo.winner=3; 

              Rinfo.board[x][y] = 1;
               printf("coordonnes joueur 1: X = %d,Y = %d\n",x,y);
             }
           
         // On verifie si les 2 client sont sur le meme PC (meme terminal)
         // Si c'est le cas on envoie le Display_Info sur le client
             if (nbj==2)
                   {
                    if(send(max,&Rinfo,sizeof(display_info),0)==-1)
                      {
                        printf("L'envoie echoue envoie Coord2 \n");
                        exit(0);
                      }
                       if (Rinfo.winner==1)
                      {
                       printf("Le joueur 1 a gagne \n");
                       exit(0);
                      }
                      if (Rinfo.winner==2)
                      {
                       printf("Le joueur 2 a gagne \n");
                       exit(0);
                      }
                      if (Rinfo.winner==3)
                      {
                       printf("Aucun gagnant , match nul \n");
                       exit(0);
                      }
                   }

                   // Ici On verifie si les 2 client sont sur des PC ou terminal different PC
                  // Si c'est le cas on envoie le Display_Info sur les clients(terminal)
                   else
                   {
                     for (int s = 0; s < nb_player.nb_players; s++)
                      {
                        if(send(client[s],&Rinfo,sizeof(display_info),0)==-1)
                          {
                            printf("L'envoie echoue envoie Coord2 \n");
                           
                          }
                           if (Rinfo.winner==1)
                            {
                              printf("Le joueur 1 a gagne \n");
                              exit(0);
                            }
                            if (Rinfo.winner==2)
                            {
                              printf("Le joueur 2 a gagne \n");
                              exit(0);
                            }
                             if (Rinfo.winner==3)
                              {
                                printf("Aucun gagnant , match nul \n");
                                exit(0);
                              }
                      }
                   }

           if(oldY1 == y1)
            {
                if (affiche_j2==TRUE)
                {
                  Rinfo.board[x1][y1] = 51;
                }
               y1 = y1 +1;
               oldY1 = y1;

              if (collision(x,y,x1,y1,Rinfo)==1)
                  Rinfo.winner=2;
              else if (collision(x,y,x1,y1,Rinfo)==2)
                  Rinfo.winner=1;
              else if (collision(x,y,x1,y1,Rinfo)==3)
                  Rinfo.winner=3; 

              Rinfo.board[x1][y1] = 2;
              printf("coordonnes joueur 2: X = %d,Y = %d\n",x1,y1);
            }
          
           if(oldX1 == x1)
           {
            if (affiche_j2==TRUE){Rinfo.board[x1][y1] = 51;}
              x1 = x1 +1;
              oldX1 = x1;

              if (collision(x,y,x1,y1,Rinfo)==1)
                  Rinfo.winner=1;
              else if (collision(x,y,x1,y1,Rinfo)==2)
                  Rinfo.winner=1;
              else if (collision(x,y,x1,y1,Rinfo)==3)
                  Rinfo.winner=3; 

              printf("coordonnes joueur 2: X = %d,Y = %d\n",x1,y1);
              Rinfo.board[x1][y1] = 2;
            }
              if(oldY1 == -1)
                {
                  if (affiche_j2==TRUE){Rinfo.board[x1][y1] = 51;}
                  x1 = x1 -1;

                      if (collision(x,y,x1,y1,Rinfo)==1)
                          Rinfo.winner=2;
                      else if (collision(x,y,x1,y1,Rinfo)==2)
                              Rinfo.winner=1;
                      else if (collision(x,y,x1,y1,Rinfo)==3)
                              Rinfo.winner=3; 

                  Rinfo.board[x1][y1] = 2;
                printf("coordonnes joueur 2: X = %d,Y = %d\n",x1,y1);
                }
             if(oldX1 == -1)
             {
                 if (affiche_j2==TRUE){Rinfo.board[x1][y1] = 51;}
                y1 = y1 -1;

                if (collision(x,y,x1,y1,Rinfo)==1)
                    Rinfo.winner=2;
                else if (collision(x,y,x1,y1,Rinfo)==2)
                    Rinfo.winner=1;
                else if (collision(x,y,x1,y1,Rinfo)==3)
                    Rinfo.winner=3; 

                Rinfo.board[x1][y1] = 2;
                printf("coordonnes joueur 2: X = %d,Y = %d\n",x1,y1);
             }
           
           if (nbj==2)
                   {
                    if(send(max,&Rinfo,sizeof(display_info),0)==-1)
                      {
                        printf("L'envoie echoue envoie Coord2 \n");
                        exit(0);
                      }
                       if (Rinfo.winner==1)
                        {
                        printf("Le joueur 1 a gagne \n");
                        exit(0);
                        }
                      if (Rinfo.winner==2)
                        {
                        printf("Le joueur 2 a gagne \n");
                        exit(0);
                        }
                         if (Rinfo.winner==3)
                          {
                          printf("Aucun gagnant , match nul \n");
                          exit(0);
                          }
                   }
                   else
                   {
                     for (int s = 0; s < nb_player.nb_players; s++)
                      {
                        if(send(client[s],&Rinfo,sizeof(display_info),0)==-1)
                          {
                            printf("L'envoie echoue envoie Coord2 \n");
                            exit(0);
                          }
                           if (Rinfo.winner==1)
                          {
                            printf("Le joueur 1 a gagne \n");
                            exit(0);
                          }
                          if (Rinfo.winner==2)
                          {
                            printf("Le joueur 2 a gagne \n");
                            exit(0);
                          }
                           if (Rinfo.winner==3)
                            {
                              printf("Aucun gagnant , match nul \n");
                              exit(0);
                            }
                      }
                   }

    select(max+1,&ensbl,NULL,NULL,&tv);
		for (int i = 0; i < nb_player.nb_players; i++)
		{
			  sd = client[i]; //on recupere le descripteur su client ici
				
			if (FD_ISSET(sd,&ensbl)) //On verifie le  descripteur s'il contient kelk choz
			{
				if (recv(client[i],&cli_input,sizeof(cli_input),0) == -1)
				{				
					printf("deconnexion\n");	
					//on ferme le socket
					close(sd);
					client[i] = 0;
				}
					else    
           {
               if (cli_input.id==1)  // Joueur 1 #
               {
                  if (cli_input.input == 'z') { //Haut
                  if(affiche_j1==TRUE){ Rinfo.board[x][y] = 50;}
                    y = y -1;
                    oldX = -1;
                    oldY = y;

                  if (collision(x,y,x1,y1,Rinfo)==1)
                    Rinfo.winner=2;
                  else if (collision(x,y,x1,y1,Rinfo)==2)
                      Rinfo.winner=1;
                  else if (collision(x,y,x1,y1,Rinfo)==3)
                      Rinfo.winner=3; 

                  Rinfo.board[x][y] = 1;
                  printf("coordonnes joueur 1: X = %d,Y = %d\n",x,y);
                  }
                  if (cli_input.input == 'q') { //Gauche
                     if(affiche_j1==TRUE){ Rinfo.board[x][y] = 50;}
                      x = x-1;
                      oldX = x;
                      oldY = -1;
                      
                    if (collision(x,y,x1,y1,Rinfo)==1)
                        Rinfo.winner=2;
                    else if (collision(x,y,x1,y1,Rinfo)==2)
                        Rinfo.winner=1; 
                    else if (collision(x,y,x1,y1,Rinfo)==3)
                          Rinfo.winner=3; 

                    Rinfo.board[x][y] = 1;
                    printf("coordonnes joueur 1: X = %d,Y = %d\n",x,y);
                  }
                  if (cli_input.input == 'd' ) { //Droite
                        if(affiche_j1==TRUE)
                        { 
                          Rinfo.board[x][y] = 50;
                        }
                        x = x+1;
                        oldX = x;
                        oldY = 0;

                    if (collision(x,y,x1,y1,Rinfo)==1)
                        Rinfo.winner=2;
                    else if (collision(x,y,x1,y1,Rinfo)==2)
                          Rinfo.winner=1;   
                    else if (collision(x,y,x1,y1,Rinfo)==3)
                          Rinfo.winner=3; 

                     Rinfo.board[x][y] = 1;
                     printf("coordonnes joueur 1: X = %d,Y = %d\n",x,y);
                  }
                  if (cli_input.input == 's') { // Bas
                      if(affiche_j1==TRUE){ Rinfo.board[x][y] = 50;}
                      
                       y = y + 1;
                       oldX = 0;
                       oldY = y;

                     if (collision(x,y,x1,y1,Rinfo)==1)
                        Rinfo.winner=2;
                     else if (collision(x,y,x1,y1,Rinfo)==2)
                          Rinfo.winner=1;
                     else if (collision(x,y,x1,y1,Rinfo)==3)
                          Rinfo.winner=3; 

                    Rinfo.board[x][y] = 1;
                    printf("coordonnes joueur 1: X = %d,Y = %d\n",x,y);
                  }
                  if (cli_input.input == 'm') { // la touche m est appuie
                     if (cpt1%2==0) // on cpt1 modulo 2 est 0
                      affiche_j1 = TRUE; //si c'est le cas on autorise l'affichage du lightcycle du Joueur 1
                     else
                      affiche_j1 = FALSE; // sinon on autorise pas
                    cpt1++; // on incremente pour lui permettre d'afficher ou de cacher a nouveau le lightcycle
                  }
                 
                 if (nbj==2) //si les 2 clients sont sur le meme terminal on n'envoie le plateau du jeu actuel
                 {
                    if(send(sd,&Rinfo,sizeof(display_info),0)==-1)
                      {
                        printf("L'envoie echoue envoie Coord1 \n");
                      }
                      if (Rinfo.winner==1)
                      {
                       printf("Le joueur 1 a gagne \n");
                       exit(0);
                      }
                      if (Rinfo.winner==2)
                      {
                       printf("Le joueur 2 a gagne \n");
                       exit(0);
                      }     
                 }
                 else
                 { //si les 2 clients sont sur 2 terminal different 
                    //on parcocours le tableau et envoye  le plateau du jeu actuel sur chaque terminal 
                     for (int s = 0; s < nb_player.nb_players; s++)
                      {
                        printf("client %d \n",client[s]);
                        
                        if(send(client[s],&Rinfo,sizeof(display_info),0)==-1)
                         {
                          printf("L'envoie echoue envoie Coord1 \n");
                          exit(0); 
                         }
                         if (Rinfo.winner==1)
                           {
                            printf("Le joueur 1 a gagne \n");
                            exit(0);
                           }
                          if (Rinfo.winner==2)
                            {
                            printf("Le joueur 2 a gagne \n");
                            exit(0);
                            }
                      }
                 }
                    
               }
               
               if (cli_input.id==2)  // jjoueur 2 @
                 {
                     printf("ID: %d\n",cli_input.id);
                    if (cli_input.input == 'i') {   //Haut
                        if (affiche_j2==TRUE)
                        {
                           Rinfo.board[x1][y1] = 51;
                        }
                        y1 = y1 -1;
                        oldX1 = -1;
                        oldY1= y1;

                       if (collision(x,y,x1,y1,Rinfo)==1)
                          Rinfo.winner=2; 
                       else if (collision(x,y,x1,y1,Rinfo)==2)
                          Rinfo.winner=1;  
                       else if (collision(x,y,x1,y1,Rinfo)==3)
                          Rinfo.winner=3; 

                       Rinfo.board[x1][y1] = 2;
                      printf("coordonnes joueur 2: X = %d,Y = %d\n",x1,y1);
                      
                    }
                    if (cli_input.input == 'j') {   //Gauche
                        if (affiche_j2==TRUE){Rinfo.board[x1][y1] = 51;}
                       x1 = x1-1 ;
                       oldX1 = x1;
                       oldY1 = -1;

                      if (collision(x,y,x1,y1,Rinfo)==1)
                          Rinfo.winner=2;
                      else if (collision(x,y,x1,y1,Rinfo)==2)
                            Rinfo.winner=1; 
                      else if (collision(x,y,x1,y1,Rinfo)==3)
                            Rinfo.winner=3; 
                    
                      Rinfo.board[x1][y1] = 2;
                      printf("coordonnes joueur 2: X = %d,Y = %d\n",x1,y1);
                    }
                    if (cli_input.input == 'k' ) {  //Bas
                       if (affiche_j2==TRUE)
                        {
                          Rinfo.board[x1][y1] = 51;
                        }
                        y1 = y1 +1;
                        oldX1 = 0;
                        oldY1 = y1;

                        if (collision(x,y,x1,y1,Rinfo)==1)
                            Rinfo.winner=2;
                        else if (collision(x,y,x1,y1,Rinfo)==2)
                                Rinfo.winner=1;
                        else if (collision(x,y,x1,y1,Rinfo)==3)
                              Rinfo.winner=3; 

                          Rinfo.board[x1][y1] = 2;
                          printf("coordonnes joueur 2: X = %d,Y = %d\n",x1,y1);
                    }
                    if (cli_input.input == 'l' ) {  // Droite
                     
                        if (affiche_j2==TRUE)
                          {
                           Rinfo.board[x1][y1] = 51;
                          }
                      x1 = x1 +1;
                      oldX1 = x1;
                      oldY1 = 0;

                      if (collision(x,y,x1,y1,Rinfo)==1)
                          Rinfo.winner=2; 
                      else if (collision(x,y,x1,y1,Rinfo)==2)
                            Rinfo.winner=1; 
                      else if (collision(x,y,x1,y1,Rinfo)==3)
                          Rinfo.winner=3; 

                      Rinfo.board[x1][y1] = 2;
                      printf("coordonnes joueur 2: X = %d,Y = %d\n",x1,y1);
                    }
                    if (cli_input.input == ' ') { 
                     if (cpt2%2==0)
                       affiche_j2 = TRUE;
                     else
                       affiche_j2 = FALSE;
                     cpt2++;
                    printf("Mode==> %d\n",cpt2);
                  }
                    
                   if (nbj==2)
                   {
                      if(send(sd,&Rinfo,sizeof(display_info),0)==-1)
                         printf("L'envoie echoue du mis a jour \n");
                      if (Rinfo.winner==1)
                         exit(0);
                      if (Rinfo.winner==2)
                          exit(0);
                      if (Rinfo.winner==3)
                          exit(0);
                   }
                   else
                   {
                     for (int s = 0; s < nb_player.nb_players; s++)
                      {
                        if(send(client[s],&Rinfo,sizeof(display_info),0)==-1)
                            exit(0);
                        if (Rinfo.winner==1)
                            exit(0);
                        if (Rinfo.winner==2)
                            exit(0);
                        if (Rinfo.winner==3)
                            exit(0);
                      }
                   }
                   
                                      
                }            
            }
     }            
         
    }
    usleep(atoi(argv[2])); // pour le refresh_rate
  }
    return 0;  
}
