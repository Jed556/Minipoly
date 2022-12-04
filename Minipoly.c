#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// -------------------------------- INITIALIZE -------------------------------- //

// Player board position arrays
char pPos1[20];
char pPos2[20];

// Player icons (Board pieces)

char player1 = '1';
char player2 = '2';

// Global player positions
int globPos1;
int globPos2;

// Starting money
float money1 = 1500;
float money2 = 1500;

// Properties
char *propNames[] = {"", "Kentucky Avenue", "Indiana Avenue", "Illinois Avenue", "Pacific Avenue", "", "North Carolina Avenue", "Pennsylvania Avenue", "Oriental Avenue", "Vermont Avenue", "", "Connecticut Avenue", "St. James Place", "Tennessee Avenue", "New York Avenue", "", "Tax Pay", "Atlantic Avenue", "Ventnor Avenue", "Marvin Gardens"};
char propTypes[] = {'0', 'R', 'R', 'R', 'G', '0', 'G', 'G', 'B', 'B', '0', 'B', 'O', 'O', 'O', '0', 'T', 'Y', 'Y', 'Y'};
int props[20] = {0, 18, 18, 20, 26, 0, 26, 28, 6, 6, 0, 8, 14, 14, 16, 0, 100, 22, 22, 24};
int props1[20];
int props2[20];

// Win?
int win1;
int win2;

// Other system vars
int turn = 1;
int debugMode = 0;

// Function prototypes
void mainMenu(void);
void move(void);
int checkBuy(int player);
void payRent(int player);
int posCheck(int player);
int landedOn(int posCheck, char check[]);
void landOnG(void);
void changePos(int rolled, int player);
void printBoard(void);
void newLines(int numNewLines);

// -------------------------------- RUN GAME -------------------------------- //
void main(void)
{
    // initialize game
    for (int i = 0; i <= 19; i++)
    {
        pPos1[i] = ' ';
        pPos2[i] = ' ';
        props1[i] = 0;
        props2[i] = 0;
    }
    changePos(0, 1);
    changePos(0, 2);

    while (1) // Infinitely loop for Main Menu prompt
    {
        mainMenu();

        while (1) // Infinitely loop for UI updates and moves
        {
            move();
            landOnG();

            if (money2 < 0)
            {
                printf("PLAYER 1 WON");
                break;
            }
            else if (money1 < 0)
            {
                printf("PLAYER 2 WON");
                break;
            }

            if (turn == 1)
                turn = 2;
            else
                turn = 1;
        }
        // Ask again if user wants to play (return to top)
    }
}

// -------------------------------- MAIN SYSTEM -------------------------------- //

void mainMenu(void)
{
    int mode;
    printf("MINIPOLY\n");
    printf("By Jed556"); // Hi! ~Jed556
    newLines(2);

    // Set play mode or exit
    printf("SELECT MODE\n1 for Play Mode\n2 for Debug Mode\n0 to Exit\nMode: ");
    scanf("%d", &mode);

    switch (mode) // Check mode input OR toggle debug mode between games
    {
    case 0:
        exit(0);
        break;
    case 1:
        printf("Play Mode");
        debugMode = 0;
        break;
    case 2:
        printf("Debug Mode");
        debugMode = 1;
        break;
    }
    newLines(2);
}

// Move handler
void move(void)
{
    srand(time(NULL)); // Randomize seed

    // Initialize variables
    int rolled = 0;

    while (1)
    {
        // Initialize variables
        int input = 0, buy = 1, propVal = 0, roll = 0, endTurn = 0;

        // Update UI
        printBoard();
        newLines(2);
        printf("Account Balance:\n");
        printf("Player 1: %.2f\n", money1);
        printf("Player 2: %.2f", money2);
        newLines(2);

        // Initialize turns
        switch (turn)
        {
        case 1:
            printf("Player 1's Turn\n");
            break;
        case 2:
            printf("Player 2's Turn\n");
            break;
        }

        buy = checkBuy(turn); // Update buy status

        if (!rolled)
        {
            printf("[1] Roll Dice\n");
        }
        if (buy && rolled)
        {
            printf("[2] Buy Property: %s\n", propNames[posCheck(turn)]);
            printf("[3] Skip Turn\n");
        }

        printf("Input: ");
        scanf("%d", &input);
        newLines(1);

        // Check input
        switch (input)
        {
        case 1:
            if (rolled) // Disregard if rolled already
                break;
            // ROLL DICE
            roll = rand() % 6 + 1; // Roll 6 sided dice

            if (debugMode)
                printf("[DEBUG] Rolled: %d\n", roll);

            // Print what player rolled
            switch (turn)
            {
            case 1:
                printf("Player 1");
                break;
            case 2:
                printf("Player 2");
                break;
            }
            printf(" Rolled: %d", roll);
            newLines(2);

            changePos(roll, turn); // Update the position
            buy = checkBuy(turn);  // Update buy status
            payRent(turn);         // Check if rent should be payed

            rolled = 1;
            break;

        case 2:
            if (!buy || !rolled) // Disregard if buy is disabled
                break;

            // Buy Property
            switch (turn)
            {
            case 1:
                propVal = props[posCheck(turn)];  // Find property in position
                props1[posCheck(turn)] = propVal; // Set property as owned
                money1 -= propVal;                // Deduct $X to player's money
                printf("Player 2 bought %s for $%i", propNames[posCheck(turn)], propVal);
                break;
            case 2:
                propVal = props[posCheck(turn)];  // Find property in position
                props2[posCheck(turn)] = propVal; // Set property as owned
                money2 -= propVal;                // Deduct $X to player's money
                printf("Player 2 bought %s for $%i", propNames[posCheck(turn)], propVal);
                break;
            }
            newLines(2);

            buy = 0;
            break;

        case 3:
            if (!rolled) // Disregard if rolled already
                break;
            buy = 0;
            break;
        }

        if (debugMode)
            printf("[DEBUG] move(): { Rolled: %i, BuyToggle: %i }\n", rolled, buy);

        if (rolled && !buy)
            break;
    }
}

// Update buy toggle based on player position
int checkBuy(int player)
{
    // Check if property in position is buyable
    if (props2[posCheck(player)] || props1[posCheck(player)] || !props[posCheck(player)])
        return 0;
    else
        return 1;
}

// Pay Rent
void payRent(int player)
{
    int toPay = 0;
    // Deduct money from opposite player
    switch (player)
    {
    case 1:
        toPay = props2[posCheck(player)];
        if (toPay)
        {
            money2 -= toPay;
            printf("Player 1 paid $%i for rent", toPay);
        }
        break;

    case 2:
        toPay = props1[posCheck(player)];
        if (toPay)
        {
            money2 -= toPay;
            printf("Player 2 paid $%i for rent", toPay);
        }
        break;
    }
}

// Check player position
int posCheck(int player)
{
    // Return position of player
    switch (player)
    {
    case 1:
        return globPos1;
        break;
    case 2:
        return globPos2;
        break;
    } // expandable
}

// Check player position manual
// int posCheck(int player)
// {
//     // Check every value in array
//     for (int i = 0; i <= 19; i++)
//     {
//         // Check selected player's position
//         switch (player)
//         {
//         case 1:
//             if (pPos1[i] == player1)
//             {
//                 return i;
//             }
//             break;
//         case 2:
//             if (pPos2[i] == player2)
//             {
//                 return i;
//             }
//             break;
//         } // expandable
//     }
// }

// -------------------------------- LAND HANDLING -------------------------------- //

// Check where player landed
int landedOn(int posCheck, char check[])
{
    int bool = 0;

    // Check every value in array > Check selected check > check if player is in the position
    for (int i = 0; i <= 19; i++)
    {
        if (!strcmp(check, "=="))
        {
            switch (turn)
            {
            case 1:
                if (i == posCheck && pPos1[i] == player1)
                    bool = 1;
                break;
            case 2:
                if (i == posCheck && pPos2[i] == player2)
                    bool = 1;
                break;
            }
        }
        else if (!strcmp(check, ">="))
        {
            switch (turn)
            {
            case 1:
                if (i >= posCheck && pPos1[i] == player1)
                    bool = 1;
                break;
            case 2:
                if (i >= posCheck && pPos2[i] == player2)
                    bool = 1;
                break;
            }
        }
        else if (!strcmp(check, "<="))
        {
            switch (turn)
            {
            case 1:
                if (i <= posCheck && pPos1[i] == player1)
                    bool = 1;
                break;
            case 2:
                if (i <= posCheck && pPos2[i] == player2)
                    bool = 1;
                break;
            }
        }
        else if (!strcmp(check, "<"))
        {
            switch (turn)
            {
            case 1:
                if (i < posCheck && pPos1[i] == player1)
                    bool = 1;
                break;
            case 2:
                if (i < posCheck && pPos2[i] == player2)
                    bool = 1;
                break;
            }
        }
        else if (!strcmp(check, ">"))
        {
            switch (turn)
            {
            case 1:
                if (i > posCheck && pPos1[i] == player1)
                    bool = 1;
                break;
            case 2:
                if (i > posCheck && pPos2[i] == player2)
                    bool = 1;
                break;
            }
        }
    }

    if (debugMode)
        printf("[DEBUG] landedOn: { Result: %d  Check: \"%s\" }\n", bool, check);

    return bool; // Return true or false
}

// Landed on Go Space
void landOnG(void)
{
    int toAdd = 200;

    // Add X to player money if landed on G
    if (landedOn(0, "=="))
    {
        switch (turn)
        {
        case 1:
            money1 += toAdd;
            break;
        case 2:
            money2 += toAdd;
            break;
        }
    }
}

// -------------------------------- BOARD HANDLING -------------------------------- //

// Update player position in board
void changePos(int rolled, int player)
{
    int toPos;
    switch (player) // check active player
    {
    case 1:
        toPos = globPos1;
        break;

    case 2:
        toPos = globPos2;
        break;
    } // expandable

    toPos += rolled; // add to the target position

    if (toPos > 19) // Check if the position should restart from index 0
        toPos -= 20;

    if (debugMode)
        printf("[DEBUG] toPos: %d\n", toPos);

    // Update every value (player container) in board array
    for (int i = 0; i <= 19; i++)
    {
        // Check if each array value (position) should be a player or empty
        switch (player)
        {
        case 1:
            if (i == toPos)
            {
                pPos1[i] = player1;
            }
            else
            {
                pPos1[i] = ' ';
            }
            globPos1 = toPos;
            break;
        case 2:
            if (i == toPos)
            {
                pPos2[i] = player2;
            }
            else
            {
                pPos2[i] = ' ';
            }
            globPos2 = toPos;
            break;
        } // expandable
    }
}

// Prints the board
void printBoard(void)
{
    printf("LEGEND: { player1: %c, player2: %c, R/P: \"Property Value / Rent\" }", player1, player2);
    newLines(2);
    printf("     1     2     3     4     5     6\n");
    printf("  +-----+-----+-----+-----+-----+-----+\n");
    printf("f | %c%c V| %c%c  | %c%c  | %c%c  | %c%c  | %c%c P|\n", pPos1[5], pPos2[5], pPos1[6], pPos2[6], pPos1[7], pPos2[7], pPos1[8], pPos2[8], pPos1[9], pPos2[9], pPos1[10], pPos2[10]);
    printf("  +-----+-----+-----+-----+-----+-----+\n");
    printf("e | %c%c  |                       | %c%c  |\n", pPos1[4], pPos2[4], pPos1[11], pPos2[11]);
    printf("  +-----+                       +-----+\n");
    printf("d | %c%c  |                       | %c%c  |\n", pPos1[3], pPos2[3], pPos1[12], pPos2[12]);
    printf("  +-----+                       +-----+\n");
    printf("c | %c%c  |                       | %c%c  |\n", pPos1[2], pPos2[2], pPos1[13], pPos2[13]);
    printf("  +-----+                       +-----+\n");
    printf("b | %c%c  |                       | %c%c  |\n", pPos1[1], pPos2[1], pPos1[14], pPos2[14]);
    printf("  +-----+-----+-----+-----+-----+-----+\n");
    printf("a | %c%c G| %c%c  | %c%c T| %c%c  | %c%c  | %c%c J|\n", pPos1[0], pPos2[0], pPos1[19], pPos2[19], pPos1[18], pPos2[18], pPos1[17], pPos2[17], pPos1[16], pPos2[16], pPos1[15], pPos2[15]);
    printf("  +-----+-----+-----+-----+-----+-----+\n");
    // printf("           o           n     m         ");
}

// -------------------------------- MISC FUNCTIONS -------------------------------- //

// Creates X new lines based on input (C doesn't support multiple new lines in printf)
void newLines(int numNewLines)
{
    for (int i = 0; i < numNewLines; i++)
        printf("\n");
}