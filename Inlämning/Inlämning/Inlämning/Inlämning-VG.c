#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <time.h>
#include "safeinput.h"
#include "Serial.h"

typedef struct
{
	char cardNumber[30];
	bool access;
	struct tm date;
}CARD;

typedef struct
{
	CARD* allCards;
	int numberOfCards;
	SERIALPORT port;
}SYSTEM_STATE;

char *CheckGivenCardNumber(char* number, SYSTEM_STATE* state)
{
	for (int i = 0; i < state->numberOfCards; i++)
	{
		if (strcmp(state->allCards[i].cardNumber, number) == 0)
		{
			return state->allCards[i].cardNumber;
		}
	}
	return NULL; 
}

int GetCardIndex(char* number, SYSTEM_STATE* state)
{
	for (int i = 0; i < state->numberOfCards; i++)
	{
		if (strcmp(state->allCards[i].cardNumber, number) == 0)
		{
			return i;
		}
	}
	return 0; //HJÄLP
}

void PrintCardAccess(int index, SYSTEM_STATE* state)
{
	printf("%s", state->allCards[index].access ? "You have access.\n" : "You don't have access.\n");
}

void SetCardAccess(int index, SYSTEM_STATE* state)
{
	char givenCardAccess[30];
	GetInput("Enter 1 for access, 2 for no access: ", givenCardAccess, 30);
	if (strcmp(givenCardAccess, "1") == 0)
	{
		state->allCards[index].access = true;
	}
	else if (strcmp(givenCardAccess, "2") == 0)
	{
		state->allCards[index].access = false;
	}
}

void SendCardToArduino(SYSTEM_STATE* state)
{
	SerialWritePort(state->port, "<CLEARCARDLIST>", strlen("<CLEARCARDLIST>"));
	for (int i = 0; i < state->numberOfCards; i++)
	{
		if (state->allCards[i].access == true)
		{
			char command[30];
			strcpy(command, "<ADDCARD");
			strcat(command, state->allCards[i].cardNumber);
			strcat(command, ">");
			SerialWritePort(state->port, command, strlen(command));
			Sleep(1000);
		}
	}
}

void SetAddedTime(int index, SYSTEM_STATE* state)
{
	time_t now = time(NULL);
	struct tm currentDate = *localtime(&now);
	state->allCards[index].date = currentDate;
}

void RemoteOpenDoor(SYSTEM_STATE* state)
{
	SerialWritePort(state->port, "<REMOTEOPENDOOR>", strlen("<REMOTEOPENDOOR>"));
}

void ListAllCards(SYSTEM_STATE* state)
{
	for (int i = 0; i < state->numberOfCards; i++)
	{
		CARD currentCard = state->allCards[i];
		printf("Card number: %s, %s, added: %d-%d-%d.\n", currentCard.cardNumber, currentCard.access ? "access" : "no access" , currentCard.date.tm_year + 1900, currentCard.date.tm_mon + 1, currentCard.date.tm_mday);
	}
}

void AddRemoveAccess(SYSTEM_STATE* state)
{
	char givenCardNumber[30];
	GetInput("Enter card number:", givenCardNumber, 30);

	if (state->numberOfCards == 0) //FINNS INGA KORT
	{
		state->allCards = malloc(sizeof(CARD)); 
		int newCardIndex = 0;
		state->numberOfCards = 1;
		strcpy(state->allCards[newCardIndex].cardNumber , givenCardNumber); 
		SetCardAccess(newCardIndex, state);
		SetAddedTime(newCardIndex, state);
		SendCardToArduino(state);
	}

	else //FINNS KORT
	{
		char* checkedCardNumber = CheckGivenCardNumber(givenCardNumber, state);
		if (checkedCardNumber == NULL) //GIVET KORT FINNS INTE
		{
			state->allCards = realloc(state->allCards, sizeof(CARD) * (state->numberOfCards + 1)); 
			int newCardIndex = state->numberOfCards;
			state->numberOfCards += 1;
			strcpy(state->allCards[newCardIndex].cardNumber, givenCardNumber);
			SetCardAccess(newCardIndex, state); 
			SetAddedTime(newCardIndex, state);
			SendCardToArduino(state);
		}
		else //GIVET KORT FINNS
		{
			int cardIndex = GetCardIndex(checkedCardNumber, state);
			PrintCardAccess(cardIndex, state); 
			SetCardAccess(cardIndex, state); 
			SendCardToArduino(state);
		}
	}
}

void FakeTestScanCard(SYSTEM_STATE* state)
{
	char scannedCard[30];
	GetInput("Please scan card to enter or x to return to admin mode: ", scannedCard, 30);
	if (strcmp(scannedCard, "x") == 0)
	{
		return;
	}
	else
	{
		int scannedCardIndex = GetCardIndex(scannedCard, state);
		char command[30];
		strcpy(command, "<SCANCARD");
		strcat(command, state->allCards[scannedCardIndex].cardNumber);
		strcat(command, ">");
		SerialWritePort(state->port, command, strlen(command));
	}
}

void AdminMenu(SYSTEM_STATE* state)
{
	while (true)
	{
		printf("***ADMIN MENU***\n 1. Remote open door\n 2. List all cards in system\n 3. Add/remove access\n 4. Exit\n 5. FAKE TEST SCAN CARD\n");
		char selection[30];
		GetInput("Enter menu choice: ", selection, 30);
		if (strcmp(selection, "1") == 0)
		{
			RemoteOpenDoor(state);
		}
		else if (strcmp(selection, "2") == 0)
		{
			ListAllCards(state);
		}
		else if (strcmp(selection, "3") == 0)
		{
			AddRemoveAccess(state);
		}
		else if (strcmp(selection, "4") == 0)
		{
			return;
		}
		else if (strcmp(selection, "5") == 0)
		{
			FakeTestScanCard(state);
		}
		else
		{
			printf("Wrong format!\n");
		}
	}
}

int main()
{
	
	SYSTEM_STATE state;
	state.port = SerialInit("\\\\.\\COM7");

	if (!SerialIsConnected(state.port))
	{
		return NULL;
	}

	state.allCards = NULL;
	state.numberOfCards = 0;
	AdminMenu(&state);
	return 0;
}

