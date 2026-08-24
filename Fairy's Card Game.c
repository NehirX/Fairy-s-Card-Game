/**
 *@file cardGame.c
 *@brief Implementation of a card game
 *
 *This code shows a card game which players take random turns and use special card effects to each other
 *Card effects can either increase or decrease players' life-points
 *The goal is to be the last alive player in the game to win
 *
 *Features include:
 *-Player initialization, elimination and management
 *-Deck initialization, shuffling and dealing
 *-Special card effects and playgame mechanics
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <stdbool.h> 

/// It resets terminal text color to default
#define RESET   "\033[0m"
///it sets terminal text color to red for warning messages
#define RED     "\033[1;31m" 
/// it sets terminal text color to green to ask players whether they want to reveal their faceDown card or not
#define GREEN   "\033[1;32m"
/// it sets terminal text color to magenta for error messages
#define MAGENTA "\033[35m"
///it sets terminal text color to cyan
#define CYAN    "\033[36m"
///it sets terminal text color to yellow to indicate players' information and turns
#define YELLOW  "\033[33m"

///Array for symbols of cards (Hearts, Dioamands, Clubs, Spades)
const char  *symbols[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
///Array for values of cards (1-7, J, Q, K)
const char *values[] = {"1", "2", "3", "4", "5", "6", "7", "J", "Q", "K"};

///represents cards in the deck
typedef struct{
	char v[3]; ///< rank/value of the card
	char s[10]; ///< suit/symbol of the card
	bool faceUp; ///< If true the card is faceUp, if false the card is faceDown
}Cards;

///represents players in the game
typedef struct Players{

       char name[50]; ///< name of the player 
       int lifePoints;///< lifepoints of the player 
       Cards hand[2]; ///< a hand of two cards 
       struct Players* previous; ///< pointer to the previous player 
       struct Players* next; ///< pointer to the next player 
}Players;

/**
 *@brief initiliaze a 40-card deck
 *All cards are set to faceDown at the beginning
 *
 *@param Deck pointer to cards array to be initiliazed
 *
 */
void initializeDeck(Cards* Deck) {
      int index = 0;
      for (int s = 0; s < 4; s++) {
        for (int v = 0; v < 10; v++) {
            strcpy(Deck[index].s, symbols[s]);
            strcpy(Deck[index].v, values[v]);
            Deck[index].faceUp = false;
            index++;
        }
    }
}

/**
 *@brief shuffle the deck randomly
 *
 *Swap cards in random with Fisher_Yates algorithm
 *@param Deck pointer to cards array
 *@param Deck_size is number of cards in the deck
 */
void shuffledeck(Cards* Deck , int Deck_size){
       for (int i = 0 ; i <Deck_size ; ++i){
              int j = rand() % Deck_size; // rand() generates a random number while %Deck_sİZE ensurers that j is between 0 and 39
              Cards temp = Deck[i];
              Deck[i]= Deck[j];
              Deck[j]= temp;
       }
}
   
/**
 *@brief deals two cards to every player
 *
 *first card is dealt faceDown and other one is faceUp
 *
 *@param Deck pointer to shuffled deck of cards
 *@param player array of players who receive dealing cards
 *@param number_of_players total number of players in the game
 */   
void dealCards(Cards* Deck , Players* player , int number_of_players){
       int cardIndex =0 ;
       for (int i =0 ; i< number_of_players;++i){
              player[i].hand[0] = Deck[cardIndex++]; // Face-down
              player[i].hand[0].faceUp = false ;
              player[i].hand[1] = Deck[cardIndex++]; // Face-up
              player[i].hand[1].faceUp = true;
       }
}

/**
 *@brief demonstrate playing field and players' information
 *
 *expresses remaining players and their life-points 
 *
 *@param lifePointsinField pointer to life points of the playing field
 *@param player array of players in the game
 *@param number_of_players total number of players in the game
 */ 
void playingField(int number_of_players, const Players* player, int* lifePointsinField) {
     printf(CYAN "\n===========Playing Field==========\n" RESET);
     printf("LifePoints of the field: %d", *lifePointsinField);
     printf(YELLOW "\nPLAYERS INFO:\n" RESET);
     for(int i = 0; i < number_of_players; i++) {
        if(player[i].lifePoints > 0) {
           printf("-Player %d: %s: Life Points: %d\n", i+1 , player[i].name, player[i].lifePoints);
      }
   }
}
   
/**
 *@brief initialize players with names, two lifepoints and two cards (one faceDown and one faceUp)
 *
 *Players are linked in a circular list.
 *
 *@param player array of players to be initiliazed
 *@param number_of_players total number of players in the game
 */                         
void initializePlayers(Players* player, int number_of_players) {

     for(int i = 0; i < number_of_players; i++) {
         printf("Enter a name for player %d:", i + 1);
         scanf("%s", player[i].name);
         player[i].lifePoints = 2;
         player[i].hand[0].faceUp = false;
}
    //linking players in circular list
    for(int i = 0; i < number_of_players; i++) {
        player[i].next = &player[(i+1) % number_of_players];
        player[i].previous = &player[(i-1 + number_of_players) % number_of_players];
   }
}

/**
 *@brief remove eliminated players from the circular list
 *
 *updates previous and next players and active count is reduced
 *
 *@param player pointer to the player to be eliminated
 *@param active pointer to the total number of active players
 */ 
void removePlayer(Players* player, int* active) {

       player->previous->next = player->next;
       player->next->previous = player->previous;
       (*active)--;

}

/**
 * applies the effect of a special card
 * different cards have different effects:
 * '1' : lose 1 Life point and put it on the field 
 * '7' : forces the next player to reveal faceDown card 
 * 'J' : gives 1 life point to the previous player
 * 'Q' : gives 1 life point to the second next player
 * 'K' : takes all life points from playing field 
 *@param player the player who played the card  
 *@param card the card being played  
 *@param active pointer to the number of active players
 *@param lifePointsinField Pointer to the life points in field 
 */
void applyCardEffect(Players *player, Cards card, int* active, int* lifePointsinField) {
     switch(card.v[0]) {
     
	case '1':
                printf("%s drops 1 life point to the playing field\n", player->name);
                player->lifePoints--;
                (*lifePointsinField)++;
                break;

	case '7': 
               if(player->next->hand[0].faceUp == false) {
                  printf("%s forces to next player %s to reveal their faceDowm card.\n", player->name, player->next->name);
                  player->next->hand[0].faceUp = true;
                  printf("%s' revealed card is %s of %s: ", player->next->name, player->next->hand[0].v, player->next->hand[0].s);
                  applyCardEffect(player->next, player->next->hand[0], active, lifePointsinField);
               if (player->next->lifePoints <= 0 ){
                   printf(MAGENTA"%s is eliminated from the game ! \n"RESET ,player->next->name);
                   removePlayer(player->next, active); 
               if(*active == 1) break;
                 }                      
                 } else {
                   printf("%s do not have any faceDown cards to be revealed.\n", player->next->name);
                 }
                 break;

	case 'J':
               printf("%s gives 1 lifePoint to the previous player %s\n", player->name, player->previous->name);
               player->lifePoints--;
               player->previous->lifePoints++;
               break;

	case 'Q':
               if(*active == 2) {
                  printf("%s gives 1 lifePoint to the second following player and gains it back\n", player->name);  
              }else {
                  printf("%s gives 1 lifePoint to the second following player %s\n", player->name, player->next->next->name);
                  player->lifePoints--;
                  player->next->next->lifePoints++;
               }
               break;

	case 'K':
                printf("%s claims all lifePoints from the playing field\n", player->name);
                player->lifePoints += *lifePointsinField;
                *lifePointsinField = 0;
                break;

	default:
              printf("Effectless card\n");
              break;
     }
}

/** 
 *Chooses a random player to start the game 
 * 
 *@param number_of_player total number of players
 *@return index of the player who will start 
 */
int randomstart(int number_of_players) {
return rand() % number_of_players;
}

/**
 *starts the game 
 *shuffles the deck and deals cards to the present players 
 *function keep running until someone wins 
 *@param player the array of all players
 *@param number_of_players this represents the total number of the players
 *@param Deck this is the deck of the cards 
 *@param Deck_size this indicates the total number of cards present in the deck 
 */
void playGame(Players* player , int number_of_players , Cards* Deck , int Deck_size){
     int lifePointsinField = 0;
     int active = number_of_players;

       while (active > 1){
              int startP = randomstart(number_of_players);
              shuffledeck(Deck, Deck_size);
              dealCards(Deck , player , number_of_players);
              
              printf(CYAN"New phase begins!\n"RESET);
              playingField(number_of_players, player, &lifePointsinField);
              
              for (int i = startP ; i < number_of_players + startP; ++i){
                   int currentP = i % number_of_players;
              
                     if (player[currentP].lifePoints > 0){
                            printf(YELLOW"%s 's turn : \n"RESET, player[currentP].name);
                            printf("%s plays %s of %s: ", player[currentP].name, player[currentP].hand[1].v, player[currentP].hand[1].s);
                            applyCardEffect(&player[currentP],player[currentP].hand[1], &active, &lifePointsinField);
                            
                            if (player[currentP].lifePoints <= 0 ){
                                printf(MAGENTA"%s is eliminated from the game ! \n"RESET ,player[currentP].name);
                                removePlayer(&player[currentP], &active);
                                if(active == 1) break;
                                continue;
                     }
                     
                     if(player[currentP].hand[0].faceUp == false) {
                        printf(GREEN "Do you want to reveal your faceDown card? (Y/N)\n" RESET);
                        char option;
                        scanf(" %c", &option);
                        
                        while (option != 'Y' && option != 'N') {
                              printf(MAGENTA"Invalid input. Please enter 'Y' or 'N'\n"RESET);
                              scanf(" %c", &option);
                     }
                     
                     if(option == 'Y') {
                       if(player[currentP].lifePoints == 1) {
                          printf(RED "Flipping this card might eliminate you? Do you still want to proceed?(Y/N)\n" RESET);
                          scanf(" %c", &option);
                          
                     while (option != 'Y' && option != 'N') {
                           printf(MAGENTA "Invalid input. Please enter 'Y' or 'N'\n" RESET);
                           scanf(" %c", &option);
                     }
                     
                     if(option == 'N') {
                     continue;
                     }
                   }
                   
                     player[currentP].hand[0].faceUp = true;
                     printf("Revealed card is %s of %s: ", player[currentP].hand[0].v, player[currentP].hand[0].s);
                     applyCardEffect(&player[currentP],player[currentP].hand[0], &active, &lifePointsinField);
                    }
                  }
                    
                      if (player[currentP].lifePoints <= 0 ){
                          printf(MAGENTA "%s is eliminated from the game ! \n" RESET ,player[currentP].name);
                          removePlayer(&player[currentP], &active);
                          if(active == 1) break;
                          continue;

                     }
                   }
                }
                     int alive_count = 0 ;
                     Players* last_alive = NULL ;
                     
                     for (int j = 0 ; j < number_of_players ; ++j){
                            if (player[j].lifePoints > 0 ){
                                alive_count++;
                                last_alive= &player[j];
                            }
                     }
                     if (alive_count == 1) {
                         printf(CYAN"\nGame over! The winner is %s \n"RESET, last_alive->name);
                         return ; 
                     }
                 }
              }

/**
 * @brief The main function of the code 
 *
 * this function asks for the desired number of players by setting a condition (number of players must be between 2 and 20) and initiliazes the game
 * This part also sets up the deck of the cards
 * Moreover, it invokes the  playGame functions by implying all the rules of the game during its execution 
 *
 *@return returns 0 as a sign of a successful execution
 */
int main() {

     srand(time(NULL)); //seed random number generator
     
     int number_of_players;
     printf("Set a number of players\n");
     scanf("%d",&number_of_players);

     while (number_of_players < 2 || number_of_players > 20) {
     printf(MAGENTA "Invalid number of players, please enter a valid number (2-20):" RESET);
     scanf("%d",&number_of_players);
     }

     Players player[number_of_players];
     initializePlayers(player, number_of_players);
      
     Cards Deck[40];
     initializeDeck(Deck);
      
     playGame(player, number_of_players , Deck , 40);

     return 0;
}	