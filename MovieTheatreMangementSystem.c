/* Betim Hodza*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "MovieTheaterLib.h"
#include "QueueLib.h"
#include "BSTLib.h"
#include "ListLib.h"
#include "StackLib.h"

void PrintReceipts(SNODE **StackTop)
{
	SNODE *TempPtr = *StackTop;
	char Ticket[4] = {};
	if (StackTop == NULL) //nothing in stack
	{
		printf("There are no reciepts\n");
	}
	else
	{
		while (*StackTop != NULL) //something in stack
		{
			printf("Receipt #%d\n \tTheater Name: %s\n", TempPtr->ReceiptNumber, TempPtr->MovieTheaterName); 
			while (TempPtr->TicketList != NULL) //searches ticklist's next ptr (stops at last node)
			{
				ReturnAndFreeLinkedListNode(&TempPtr->TicketList, Ticket); //copies ticket from linked list node, stores in ticket array
				printf("\t%s", Ticket);
			}
			printf("\n\n"); //spacing
			
			TempPtr = (*StackTop)->next_ptr; //moves tempptr
			pop(StackTop);
		}
		
	}
	
	/* 
	   if the StackTop is empty, the print appropriate message - see example output - 
	   else print out the receipts.  A receipt is one node of the stack and each receipt
	   consists of a receipt number, a movie theater name and a TicketList (a linked list
	   of all seats sold to one customer).  Use ReturnAndFreeLinkedListNode() to traverse 
	   the linked list stored in each stack node and display the seats and free the 
	   linked list nodes. Each call to ReturnAndFreeLinkedListNode() brings back ONE ticket 
	   from the linked list in the Ticket variable.  Call pop() to remove the stack node.
	*/
}

BNODE *PickAndDisplayTheater(BNODE *BSTRoot, char MovieTheaterMap[][MAXCOLS], int *MapRow, int *MapCol)
{
	BNODE *MyTheater = NULL;
	char zip[6] = {};
	char MyDims[6] = {};
	char *token = NULL;
	char *delimeters = "x";
	int Row;
	int Col;
	
	printf("\n\nPick a theater by entering the zipcode\n\n");
				
	// Traverse the BST in order and print out the theaters in zip code order - use InOrder()
	InOrder(BSTRoot);			
	
	// Prompt for a zip
	printf("Please enter zip: ");
	scanf("%s", zip);

	// Call SearchForBNODE()
	MyTheater = SearchForBNODE(BSTRoot, zip);
	strcpy(MyDims, MyTheater->Dimensions);

	// If theater is not found, then print message
	if (MyTheater == NULL)
	{
		printf("Theater not found.\n");
	}
	else
	{
		//tokenize rows and cols using x delim
		token = strtok(MyDims, delimeters);
		Row = atoi(token);
		token = strtok(NULL, delimeters);
		Col = atoi(token);
		*MapRow = Row;
		*MapCol = Col;

		//pass in MTM, MTFN, row, col if it doesnt open print that
		if (!ReadMovieTheaterFile(MovieTheaterMap, MyTheater->FileName, Row, Col))
		{
			printf("Unable to print the seat map at this time. Check back Later.\n");
		}
		else //if it does open, print the seatmap and return the found theater
		{
			PrintSeatMap(MovieTheaterMap, *MapRow, *MapCol);
			return MyTheater;
		}
		
	}
	
	// If theater is found, then tokenize the theater's dimensions and
	// pass those dimensions and the MovieTheaterMap and the theater's filename
	// to ReadMovieTheaterFile()

	// If ReadMovieTheaterFile() returns FALSE, then print
	// "Unable to print that seat map at this time.  Check back later."
	// else call PrintSeatMap()

	// return the found theater
}	

void ReadFileIntoQueue(FILE *QueueFile, QNODE **QH, QNODE **QT)
{
	//	read the passed in file and calls enQueue for each name in the file
	char CustomerName[30] = {};

	while (fgets(CustomerName, sizeof(CustomerName)-1, QueueFile))
	{
		enQueue(CustomerName, QH, QT);
	}
}

void ReadFileIntoBST(FILE *BSTFile, BNODE **BSTnode)
{
	//	read the passed in file and tokenize each line and pass the information to AddBSTNode
	char *token = NULL;
	char *delimeters = "|";
	char MTN[40] = {};
	char zip[6] = {};
	char filename[50] = {};
	char dimensions[6] = {};
	char str1[150] = {};

	while (fgets(str1, sizeof(str1)-1, BSTFile))
	{
		
		token = strtok(str1, delimeters); //movie theater name
		strcpy(MTN, token);
		token = strtok(NULL, delimeters); //zip
		strcpy(zip, token);
		token = strtok(NULL, delimeters); //filename
		strcpy(filename, token);
		token = strtok(NULL, delimeters); //dimensions
		strcpy(dimensions,token);

		if (dimensions[strlen(dimensions) - 1] == '\n') //gets rid of newline
		{
			dimensions[strlen(dimensions) - 1] = '\0'; //replaces it with null
		}
		

		AddBSTNode(BSTnode, MTN, zip, filename, dimensions);	
	}
}

void get_command_line_parameter(char *argv[], char ParamName[], char ParamValue[])
{
	int i = 0; 
	
	while (argv[++i] != NULL)
	{
		if (!strncmp(argv[i], ParamName, strlen(ParamName)))
		{
			strcpy(ParamValue, strchr(argv[i], '=') + 1);
		}
	}
}

int PrintMenu()
{
	int choice = 0;
	
	printf("\n\n1.	Sell tickets to next customer\n\n");
	printf("2.	See who's in line\n\n");
	printf("3.	See the seat map for a given theater\n\n");
	printf("4.	Print today's receipts\n\n");
	printf("Choice : ");
	scanf("%d", &choice);
	
	while (choice < 1 || choice > 4)
	{
		printf("Invalid choice.  Please reenter. ");
		scanf("%d", &choice);
	}
	
	return choice;
}

int main(int argc, char *argv[])
{
	int i, j, k;
	FILE *queueFile = NULL;
	FILE *zipFile = NULL;
	char arg_value[20];
	char queuefilename[20];
	char zipfilename[20];
	int ReceiptNumber = 0;
	int choice = 0;
	int SeatNumber;
	char Row;
	char Ticket[5];
	int ArrayRow, ArrayCol;
	int MapRow, MapCol;
	char MovieTheaterMap[MAXROWS][MAXCOLS] = {};
	LNODE *TicketLinkedListHead = NULL;
	QNODE *QueueHead = NULL;
	QNODE *QueueTail = NULL;
	BNODE *BSTRoot = NULL;
	BNODE *MyTheater = NULL;
	SNODE *StackTop = NULL;
	int NumberOfTickets = 0;
	
	if (argc != 4)
	{
		printf("%s QUEUE=xxxxxx ZIPFILE=xxxxx RECEIPTNUMBER=xxxxx\n", argv[0]);
		exit(0);
	}
	
	get_command_line_parameter(argv, "QUEUE=", queuefilename);
	get_command_line_parameter(argv, "ZIPFILE=", zipfilename);
	get_command_line_parameter(argv, "RECEIPTNUMBER=", arg_value);
	ReceiptNumber = atoi(arg_value);
		
	/* call function to open queuefilename - if it does not open, print message and exit */	
	queueFile = fopen(queuefilename, "r");
	if (queueFile == NULL)
	{
		printf("QueueFile did not open correctly\n exiting...");
		exit(0);
	}
	

	/* calll function to open zipfilename - if it does not open, print message and exit */
	zipFile = fopen(zipfilename, "r");
	if (zipFile == NULL)
	{
		printf("QueueFile did not open correctly\n exiting...");
		exit(0);
	}

	ReadFileIntoQueue(queueFile, &QueueHead, &QueueTail);
	ReadFileIntoBST(zipFile, &BSTRoot);
	
	while (QueueHead != NULL)
	{
		choice = PrintMenu();
	
		switch (choice)
		{
			case 1 :
				printf("\n\nHello %s\n", QueueHead->name);				
				MyTheater = PickAndDisplayTheater(BSTRoot, MovieTheaterMap, &MapRow, &MapCol);
				if (MyTheater != NULL)
				{
					printf("\n\nHow many movie tickets do you want to buy? ");
					scanf("%d", &NumberOfTickets);
					for (i = 0; i < NumberOfTickets; i++)
					{
						do
						{
							printf("\nPick a seat (Row Seat) ");
							scanf(" %c%d", &Row, &SeatNumber);
							Row = toupper(Row);
							ArrayRow = (int)Row - 65;
							ArrayCol = SeatNumber - 1;
						
							if ((ArrayRow < 0 || ArrayRow >= MapRow) ||
								(ArrayCol < 0 || ArrayCol >= MapCol))
							{
								printf("\nThat is not a valid seat.  Please choose again\n\n");
							}		
						}
						while ((ArrayRow < 0 || ArrayRow >= MapRow) ||
							   (ArrayCol < 0 || ArrayCol >= MapCol));
						
						if (MovieTheaterMap[ArrayRow][ArrayCol] == 'O')
						{	
							MovieTheaterMap[ArrayRow][ArrayCol] = 'X';
							sprintf(Ticket, "%c%d", Row, SeatNumber); 
							InsertNode(&TicketLinkedListHead, Ticket);
						}
						else
						{
							printf("\nSeat %c%d is not available.\n\n", Row, SeatNumber);
							i--;
						}
						PrintSeatMap(MovieTheaterMap, MapRow, MapCol);
					}
					
					WriteSeatMap(MyTheater, MovieTheaterMap, MapRow, MapCol);
					push(&StackTop, TicketLinkedListHead, ReceiptNumber, MyTheater->MovieTheaterName);
					TicketLinkedListHead = NULL;
					ReceiptNumber++;
					printf("\nThank you %s - enjoy your movie!\n", QueueHead->name);
					deQueue(&QueueHead);
				}
				break;
			case 2 : 
				printf("\n\nCustomer Queue\n\n");
				DisplayQueue(QueueHead);
				printf("\n\n");
				break;
			case 3 :
				PickAndDisplayTheater(BSTRoot, MovieTheaterMap, &MapRow, &MapCol);
				break;
			case 4 : 
				PrintReceipts(&StackTop);
				break;
			default :
				printf("Bad menu choice");
		}
	}
	
	printf("Good job - you sold tickets to all of the customers in line.\n");
	PrintReceipts(&StackTop);
	
	fclose(queueFile);
	fclose(zipFile);
	
	return 0;
}
