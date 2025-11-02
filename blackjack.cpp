/*
Advisor gives advice on when the player should hit or stand based off Bicycle Strategy.
https://bicyclecards.com/how-to-play/blackjack/

I found this method to be more efficient than probability/percentage which makes the player calculate
further and think about what to do, Instead this way they have an visual cues on when to hit or stand.

When the dealer's visible card is a 7, 8, 9, facecard, or an ace, it's considered a good card.
The player's best shot at winning is to have a score of at least 17.

When the dealer's visible card is a 4, 5, or 6, it's considered a bad card.
The player should aim for a score of 12 or higher. Put's the pressure on dealer to bust while
trying to reach the minimum required dealer score of 17.

When the dealer's visible card is a 2 or 3, it's considered a fair card.
The player should aim for a score of 13 or higher.

This method yields the best results while presenting the most straightforward advice.
Simulations with the above mention VS. the percentage calculator method yield accuracy within 1%.
*/

#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <ios>
// For enabling ANSI escape codes on Windows
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// ANSI color codes (simple)
const string KNRM = "\x1B[0m";
const string KRED = "\x1B[31m";
const string KGRN = "\x1B[32m";
const string KYEL = "\x1B[33m";
const string KBLU = "\x1B[34m";
const string KMAG = "\x1B[35m";
const string KCYN = "\x1B[36m";
const string KBOLD = "\x1B[1m";

// Global flag: whether to print Unicode suit symbols
bool g_use_unicode = true;

// Try to enable ANSI processing and UTF-8 output on Windows consoles.
// Sets g_use_unicode accordingly. On POSIX assume Unicode is available.
void enableAnsi() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        g_use_unicode = false;
        return;
    }

    // Try set console to UTF-8 output code page
    if (!SetConsoleOutputCP(CP_UTF8)) {
        // could not set UTF-8, fall back
        g_use_unicode = false;
    }

    // Try to enable virtual terminal processing (ANSI colors)
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        g_use_unicode = false;
        return;
    }
    if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        if (!SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            // If we can't enable ANSI, still continue but colors may not work
        }
    }
#else
    // On Linux/macOS terminals Unicode and ANSI are normally available
    g_use_unicode = true;
#endif
}

// STRUCTORS and CONSTRUCTORS

// Structure for Card (Suit, Description, Rank, Value)
struct Card {

    string suit;        // Spades, Hearts, Diamonds, Clubs
    string description; // Ace through King A-K with Suit displayed
    int cardRank;       // 1-13 1 is Ace 13 is King
    int cardValue;      // Value of card in Blackjack

    // Constructor for Card
    Card() {
        suit = "";
        description = "";
        cardRank = 0;
        cardValue = 0;
    }
};

// Structure for creating a card array, max cards in deck and used cards
struct CardArray {

    Card* cards;        // Card Structure pointer
    int maxCards;       // The max number of cards you can have (52 per deck, 12 per hand)
    int usedCards;      // the number of cards in players or dealers hand currently

    // Constructor for CardArray
    CardArray() {
        cards = nullptr;
        maxCards = 0;
        usedCards = 0;
    }
};

// Function prototypes

//////////////////// PART 1 Library /////////////////////////
void getNewDeck(CardArray& deck);
void printDeck(const CardArray& deck);
void shuffleDeck(CardArray& deck);

//////////////////// PART 2 Library /////////////////////////
int blackJack(CardArray& deck);
void deal(CardArray& deck, CardArray& hand);
void printHand(const CardArray& hand, int appearance);
int scoreOfHand(CardArray& hand);
string hitOrStand();

//////////////////// PART 3 Library /////////////////////////
void playGames(CardArray& deck);

//////////////////// PART 4 Library /////////////////////////
bool ifAceInHand(const CardArray& hand);
void aceValueChanger(CardArray& hand, int& aceCount);

//////////////////// PART 5 Library /////////////////////////
int cardEvlauator(const CardArray& hand);
void initialAdvisor(CardArray& dealerHand, int playerScore, int dealerScore);
void computeTotals(const CardArray& hand, int &softTotal, int &hardTotal, int &aceCount);
void advisor(CardArray& playerHand, CardArray& dealerHand, int playerScore, int dealerScore, bool bust);


// GLOBAL CONSTANTS

// Constants related to blackjack rules.
const int MAX_DECK_SIZE = 52;
const int MAX_CARDS_IN_SUIT = 13;
const int MAX_CARDS_IN_HAND = 12;
const int BLACKJACK = 21;
const int DEALER_MIN = 17;

// Constants for creating a card
const string SUIT[] = { "S", "H", "D", "C" };
const string RANK[] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
const int RANK_VALUE[] = { 1, 2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13 };
const int VALUE[] = { 11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10 };

// Constants for visual display of player and dealer hands
const int VISIBLE = 0;
const int HIDDEN = 1;

// Constants for win, lose, draw
const int WIN = 1;
const int LOSE = -1;
const int DRAW = 0;

// Constants for advisor
const int GOOD_CARD = 1;
const int BAD_CARD = -1;
const int FAIR_CARD = 0;


// Prints 52 unsorted cards, then prints shuffled deck and plays blackjack
int main()
{
    // enable color output where supported
    enableAnsi();

    // colorful banner
    cout << KBOLD << KBLU;
    cout << "================================" << endl;
    cout << "          BLACKJACK" << endl;
    cout << "================================" << endl;
    cout << KNRM;
    // Creates a struct variable for a deck
    CardArray deck;
    // Generates a deck of 52 cards
    getNewDeck(deck);
    // Prints the unshuffled deck of 52 cards
    printDeck(deck);

    cout << endl << "Shuffled" << endl;
    // Shuffles the deck of 52 cards
    shuffleDeck(deck);
    // Now prints the newly shuffled deck
    printDeck(deck);
    cout << endl;

    // Function for playing as many games as user desires
    playGames(deck);
    // Delete the deck of cards created in dynamic memory
    delete[] deck.cards;

    return 0;
}

// Function for creating a new deck of 52 cards
void getNewDeck(CardArray& deck) {

    // Initialize variables for indexing cards and suits
    int cardIndex = 0, suitIndex = 0, suit = 0;

    // Set the max amount of cards and create an array
    deck.maxCards = MAX_DECK_SIZE;
    deck.cards = new Card[MAX_DECK_SIZE];

    // create 52 cards for deck
    while (cardIndex < MAX_DECK_SIZE) {
        // loop through 13 cards with first suit
        for (suitIndex = 0; suitIndex < MAX_CARDS_IN_SUIT; suitIndex++, cardIndex++) {
            // assign rank, description and value
            deck.cards[cardIndex].cardRank = RANK_VALUE[suitIndex];
            deck.cards[cardIndex].description = RANK[suitIndex] + SUIT[suit];
            deck.cards[cardIndex].cardValue = VALUE[suitIndex];
        }
        // change suit after 13 cards
        suit++;
    }
}

// Function for randomizing the cards in the deck (Shuffling)
void shuffleDeck(CardArray& deck) {

    // Initialize random seed
    srand(time(0));
    // Temp holder for swapping cards
    Card temp;
    // Variable for swapping random card index
    int randomCard = 0;

    // Randomize (shuffle) the cards in the deck
    for (int i = 0; i < MAX_DECK_SIZE; i++) {
        randomCard = rand() % MAX_DECK_SIZE;
        temp = deck.cards[i];
        deck.cards[i] = deck.cards[randomCard];
        deck.cards[randomCard] = temp;
    }
}

// Prints the deck of cards
void printDeck(const CardArray& deck) {

    // Print right aligned cards
    cout << right;
    cout << setw(4);

    // Loops through the entire deck, printing each card
    for (int i = 0; i < MAX_DECK_SIZE; i++) {
        cout << deck.cards[i].description << setw(4);
        // Prints 13 cards per line displayed
        if ((i + 1) % MAX_CARDS_IN_SUIT == 0) {
            cout << endl;
        }
    }
}

// Allows the user to play mutliple games of BlackJack and gives results
void playGames(CardArray& deck) {

    // Initialize variables to record stats
    int wins = 0, losses = 0, draws = 0;

    // Initialize variable for calling blackjack
    int result = 0;
    // Track the number for games played
    int gamesPlayed = 0;
    // String for taking in user input for decisions
    string userInput;

    // Display black jack and option
    cout << "BLACKJACK" << endl << "---------" << endl;
    cout << endl << "Do you want to play a hand of blackjack (y to play)? ";
    cin >> userInput;

    // If the user uses the right key, continue
    while (userInput == "Y" || userInput == "y") {
        //Play game
        result = blackJack(deck);
        gamesPlayed++;

        // Track results
        if (result == WIN) {
            wins++;
        }
        else if (result == DRAW) {
            draws++;
        }
        else if (result == LOSE) {
            losses++;
        }

        // Ask if they want to play again
        cout << endl << "_____________________________" << endl;
        cout << endl << "Do you want to play another hand (y to play)? ";
        cin >> userInput;
    }

    // Print out the final results if they played games
    if (gamesPlayed > 0) {
        cout << "\n\nThanks for playing, you played " << gamesPlayed << " games and your record was " << endl;
        cout << "  Wins: " << wins << endl;
        cout << "Losses: " << losses << endl;
        cout << " Draws: " << draws << endl;
    }
    // Print goodbye if they didn't play any games
    else if (gamesPlayed == 0) {
        cout << "Goodbye!" << endl;
    }
}

// BlackJack game function
int blackJack(CardArray& deck) {

    string userInput;       // initialize string for getting user input
    int playerScore = 0;    // Initialize variable for storing player score
    int dealerScore = 0;    // Initialize variable for storing dealer score

    // Create card array for player and dealer hand
    CardArray playerHand;
    playerHand.cards = new Card[MAX_CARDS_IN_HAND];
    CardArray dealerHand;
    dealerHand.cards = new Card[MAX_CARDS_IN_HAND];

    // Deal first card
    cout << endl << "Deal First Card " << endl << "---------------" << endl;
    deal(deck, playerHand);
    deal(deck, dealerHand);

    // Print first card, both cards visible
    cout << "+Player+: ";
    printHand(playerHand, VISIBLE);
    cout << endl << "*Dealer*: ";
    printHand(dealerHand, VISIBLE);

    // Deal second card
    cout << endl << endl << "Deal second Card " << endl << "----------------" << endl;
    deal(deck, playerHand);
    deal(deck, dealerHand);

    // Print second card, player visible, dealer hidden
    cout << "+Player+: ";
    printHand(playerHand, VISIBLE);
    cout << endl << "*Dealer*: ";
    printHand(dealerHand, HIDDEN);

    // Calculate player and dealer score (use soft/hard totals)
    int playerSoft = 0, playerHard = 0, playerAces = 0;
    computeTotals(playerHand, playerSoft, playerHard, playerAces);
    playerScore = (playerSoft <= BLACKJACK) ? playerSoft : playerHard;

    int dealerSoft = 0, dealerHard = 0, dealerAces = 0;
    computeTotals(dealerHand, dealerSoft, dealerHard, dealerAces);
    dealerScore = (dealerSoft <= BLACKJACK) ? dealerSoft : dealerHard;

    // Print totals and note if an Ace is present or will be revalued
    bool prevSoftUsed = (playerAces > 0 && playerSoft <= BLACKJACK && playerSoft != playerHard);
    if (playerAces > 0) {
        cout << endl << "\tTotals: " << KBOLD << playerSoft << KNRM << " (soft), " << playerHard << " (hard)";
        if (!prevSoftUsed && playerAces > 0 && playerSoft != playerHard) {
            cout << "  (Ace counted as 11)";
        }
        cout << endl;
    }
    // Always print current (chosen) score for player
    cout << "\tCurrent score: " << KBOLD << playerScore << KNRM << endl;

    // Initialize variable to update advisor if hand is bust
    bool bust = false;

    // Give initial feedback regarding dealers upcard
    initialAdvisor(dealerHand, playerScore, dealerScore);

    // Call advisor function and update as necessary
    advisor(playerHand, dealerHand, playerScore, dealerScore, bust);

    // Play game as long as dealer and player score is below 21
    while (dealerScore < BLACKJACK && playerScore < BLACKJACK) {

        // Ask if user wants to hit or stand
        cout << endl << "\nDealing to player: " << endl << "-----------------";
        userInput = hitOrStand();

        // If user hits, deal another card, print it and update score
        while ((userInput == "h" || userInput == "H") && (playerScore < BLACKJACK)) {
            deal(deck, playerHand);
            cout << "+Player+: ";
            printHand(playerHand, VISIBLE);
            // recompute totals after the hit
            int newSoft = 0, newHard = 0, newAces = 0;
            computeTotals(playerHand, newSoft, newHard, newAces);
            int newScore = (newSoft <= BLACKJACK) ? newSoft : newHard;

            // Detect ace revaluation (soft -> hard)
            bool newSoftUsed = (newAces > 0 && newSoft <= BLACKJACK && newSoft != newHard);
            if (playerAces > 0 && prevSoftUsed && !newSoftUsed) {
                cout << "\t(Ace counted as 1 to avoid bust)" << endl;
            }

            // Print updated totals and current score
            if (newAces > 0) {
                cout << endl << "\tTotals: " << KBOLD << newSoft << KNRM << " (soft), " << newHard << " (hard)" << endl;
            }
            playerScore = newScore;
            cout << "\tCurrent score: " << KBOLD << playerScore << KNRM << endl;
            playerAces = newAces; // update trackers
            prevSoftUsed = newSoftUsed;

            // Set bust flag
            if (playerScore > BLACKJACK) bust = true;

            advisor(playerHand, dealerHand, playerScore, dealerScore, bust);

            // Keep asking if they want to hit or stand. As long as they haven't lost or won yet
            if (playerScore < BLACKJACK) {
                userInput = hitOrStand();
            }
        }

        // If user stands and dealer has less than 21
        while ((userInput == "s" || userInput == "S") && (dealerScore < DEALER_MIN)) {
            cout << endl << "Dealing to dealer: " << endl << "-----------------" << endl;
            cout << "*Dealer*: ";
            printHand(dealerHand, VISIBLE);

            // Keep dealing to dealer till they reach the min value they need to stop dealing (17)
            while (dealerScore < DEALER_MIN) {
                deal(deck, dealerHand);
                cout << endl << "*Dealer*: ";
                printHand(dealerHand, VISIBLE);
                int dSoft = 0, dHard = 0, dAces = 0;
                computeTotals(dealerHand, dSoft, dHard, dAces);
                dealerScore = (dSoft <= BLACKJACK) ? dSoft : dHard;
            }
        }

        // If user hits and scores 21 and dealer hasn't reached minimum score 17
        while ((userInput == "h" || userInput == "H") && (playerScore == BLACKJACK) && (dealerScore < DEALER_MIN)) {
            cout << endl << endl << "Dealing to dealer: " << endl << "-----------------" << endl;
            cout << "*Dealer*: ";
            printHand(dealerHand, VISIBLE);

            // Keep dealing to dealer till they reach the min value they need to stop dealing (17)
            while (dealerScore < DEALER_MIN) {
                deal(deck, dealerHand);
                cout << endl << "*Dealer*: ";
                printHand(dealerHand, VISIBLE);
                int dSoft = 0, dHard = 0, dAces = 0;
                computeTotals(dealerHand, dSoft, dHard, dAces);
                dealerScore = (dSoft <= BLACKJACK) ? dSoft : dHard;
            }
        }
        // Proceed to results
        break;
    }

    // RESULTS

    // Tell user they lost if the dealer hits blackjack
    if (dealerScore == BLACKJACK && dealerHand.usedCards == 2) {
        cout << endl << "*Dealer*: ";
        printHand(dealerHand, VISIBLE);
        cout << endl << "\nDealer hit blackjack, You lose.";

        // Delete dynamic memory array
        delete[] playerHand.cards;
        delete[] dealerHand.cards;

        return LOSE;
    }

    // Tell user they lost if they bust
    if (playerScore > BLACKJACK) {
        cout << endl << "*Dealer*: ";
        printHand(dealerHand, VISIBLE);
        cout << endl << "\nBust! You lose.";

        // Delete dynamic memory array
        delete[] playerHand.cards;
        delete[] dealerHand.cards;

        return LOSE;
    }

    // If dealer busts, user wins
    if (dealerScore > BLACKJACK) {
        cout << endl << "\nDealer is bust, you win.";

        // Delete dynamic memory array
        delete[] playerHand.cards;
        delete[] dealerHand.cards;

        return WIN;
    }

    // If user hits blackjack with 2 cards
    if (playerScore == BLACKJACK && playerHand.usedCards == 2) {
        cout << endl << "*Dealer*: ";
        printHand(dealerHand, VISIBLE);
        cout << endl << "\nPlayer hit BlackJack, You win! ";

        // Delete dynamic memory array
        delete[] playerHand.cards;
        delete[] dealerHand.cards;

        return WIN;
    }
    // Double BlackJack - rare scenario. If Both hit 21 right off the bat
    if (playerScore == BLACKJACK && playerHand.usedCards == 2 &&
        dealerScore == BLACKJACK && playerHand.usedCards == 2) {

        cout << endl << "*Dealer*: ";
        printHand(dealerHand, VISIBLE);
        cout << endl << "\nBlackJack TIE!!! ";

        // Delete dynamic memory array
        delete[] playerHand.cards;
        delete[] dealerHand.cards;

        return DRAW;
    }

    // If user gets 21 with more than 2 cards and the dealer doesn't have 21
    if (playerScore == BLACKJACK && playerHand.usedCards > 2 && dealerScore != BLACKJACK) {

        // Reveal dealer's cards if they already have 17 or above with 2 cards
        // Otherwise, the deal to dealer part takes care of printing dealer hand
        if (dealerHand.usedCards == 2) {
            cout << endl << "*Dealer*: ";
            printHand(dealerHand, VISIBLE);
        }

        // Report back score accordingly
        cout << endl << "\nplayer score: " << playerScore;
        cout << ", dealer score: " << dealerScore;
        cout << endl << "\nYou win! ";

        // Delete dynamic memory array
        delete[] playerHand.cards;
        delete[] dealerHand.cards;

        return WIN;
    }
    // Else if the neither has hit blackjack
    else {
        // Printing statements for hands and scores
        if ((dealerScore >= DEALER_MIN) && (dealerHand.usedCards == 2)) {
            cout << endl << "*Dealer*: ";
            printHand(dealerHand, VISIBLE);
        }
        cout << endl << "\nplayer score: " << playerScore;
        cout << ", dealer score: " << dealerScore;

        // If player score is more than dealer score
        if (playerScore > dealerScore) {
            cout << "\nYou win.";

            // Delete dynamic memory array
            delete[] playerHand.cards;
            delete[] dealerHand.cards;

            return WIN;
        }
        // If player score is less than dealer score
        else if (playerScore < dealerScore) {
            cout << "\nYou lose";

            // Delete dynamic memory array
            delete[] playerHand.cards;
            delete[] dealerHand.cards;

            return LOSE;
        }
        // If player score is equal to dealer score
        else if (playerScore == dealerScore) {
            cout << "\nGame is tied.";

            // Delete dynamic memory array
            delete[] playerHand.cards;
            delete[] dealerHand.cards;

            return DRAW;
        }
    }
    return 0;
}

// Takes array of cards, deals shuffled cards one by one to both player
// and the dealer. Deck = 52 Cards, hand = 12 cards max for player/dealer
void deal(CardArray& deck, CardArray& hand) {

    // Initialize variables, create new card and index for tracking
    Card cardToBeDealt;
    int dealtCard = 0;

    // Assign the value of the last card in deck to the new card
    cardToBeDealt = deck.cards[(MAX_DECK_SIZE - deck.usedCards) - 1];

    // track the number of cards dealt
    dealtCard += hand.usedCards;

    // Deal a new card as long as they have less than 12 cards
    if (dealtCard < MAX_CARDS_IN_HAND) {
        hand.cards[dealtCard] = cardToBeDealt;
    }
    hand.usedCards++;
    deck.usedCards++;

    // When we go through entire deck, create new one
    if (deck.usedCards == MAX_DECK_SIZE - 1) {
        shuffleDeck(deck);
        deck.usedCards = 0;
        cout << endl << "//////New shuffled deck//////" << endl << endl;
    }
}

// print hand of player and dealer
void printHand(const CardArray& hand, int display) {

    // Display both cards
    if (display == VISIBLE) {
        for (int i = 0; i < hand.usedCards; i++) {
            // color suits: H and D red, S and C default
            string desc = hand.cards[i].description;
            // last character is suit (S,H,D,C)
            if (!desc.empty()) {
                char suit = desc.back();
                string rank = desc.substr(0, desc.size() - 1);
                string color = "";
                if (suit == 'H' || suit == 'D') color = KRED;
                else color = KNRM;
                cout << "[" << color << rank;
                // display a small suit symbol
                if (g_use_unicode) {
                    // Unicode suit symbols
                    if (suit == 'S') cout << "\u2660"; // spade
                    else if (suit == 'H') cout << "\u2665"; // heart
                    else if (suit == 'D') cout << "\u2666"; // diamond
                    else if (suit == 'C') cout << "\u2663"; // club
                }
                else {
                    // ASCII fallback
                    if (suit == 'S') cout << "S";
                    else if (suit == 'H') cout << "H";
                    else if (suit == 'D') cout << "D";
                    else if (suit == 'C') cout << "C";
                }
                cout << KNRM << "] " << setw(2);
            }
        }
    }

    // Hide dealers second card
    if (display == HIDDEN) {
        // show first card then a hidden card
        string desc = hand.cards[0].description;
        char suit = desc.back();
        string rank = desc.substr(0, desc.size() - 1);
        string color = (suit == 'H' || suit == 'D') ? KRED : KNRM;
        cout << "[" << color << rank;
        if (g_use_unicode) {
            if (suit == 'S') cout << "\u2660";
            else if (suit == 'H') cout << "\u2665";
            else if (suit == 'D') cout << "\u2666";
            else if (suit == 'C') cout << "\u2663";
        }
        else {
            if (suit == 'S') cout << "S";
            else if (suit == 'H') cout << "H";
            else if (suit == 'D') cout << "D";
            else if (suit == 'C') cout << "C";
        }
        cout << KNRM << "] ";
        cout << "[" << KCYN << "??" << KNRM << "]";
    }
}

// Asks the user whether they want to hit or stand
string hitOrStand() {

    // Initialize string for user Input
    string userInput;

    // If the user input isn't h, H for hit and s, S for stop,
    // Show error and ask for input again
    cout << endl << "Enter h to hit or s to stand: ";
    cin >> userInput;
    while (!(userInput == "h" || userInput == "H" || userInput == "s" || userInput == "S")) {
        cout << "Incorrect input, Enter h to hit or s to stand: ";
        cin >> userInput;
    }
    return userInput;
}

// Returns score of given hand
int scoreOfHand(CardArray& hand) {

    // Variable for storing the score
    int score = 0, aceSum = 0;
    bool aceInHand = false;

    // loops through cards and adds value for total
    for (int i = 0; i < MAX_CARDS_IN_HAND; i++) {
        score += hand.cards[i].cardValue;
    }

    // If hand is bust and ace can be changed
    for (int aceCount = 1; score > BLACKJACK && !(aceInHand); ++aceCount) {
        // Check if the player has an ace in hand
        if (ifAceInHand(hand)) {
            // Call the function to change value of ace
            aceValueChanger(hand, aceCount);
            // Reset score
            score = 0;
            for (int i = 0; i < hand.usedCards; ++i) {
                score += hand.cards[i].cardValue;
            }
            // Update the score with new value of ace
            for (int i = 0; i < hand.usedCards; ++i) {
                if (hand.cards[i].cardRank == 1) {
                    aceSum += hand.cards[i].cardValue;
                }
            }
        }
        // If sum of aces less than cards in hand, set ace in hand true
        if (aceSum < MAX_CARDS_IN_HAND) {
            aceInHand = true;
        }
    }
    return score;
}

// Swaps the value of ace from 11 to 1 if needed
void aceValueChanger(CardArray& hand, int& aceCount) {

    // Initialize variable for number of aces
    int aces = 0;
    // Loop through hand and change value of aces
    for (int i = 0; i < hand.usedCards && aces < aceCount; i++) {
        // If the card has rank 1 (ACE) change it's value to 1
        if (hand.cards[i].cardRank == 1) {
            hand.cards[i].cardValue = 1;
            aces += 1;
        }
    }
}

//finds aces in the hand
bool ifAceInHand(const CardArray& hand) {

    // Initialize ace to false
    bool ace = false;
    // Loop through hand and check for aces
    for (int i = 0; i < hand.usedCards; i++) {
        // Set ace to true if found
        if (hand.cards[i].cardRank == 1) {
            ace = true;
        }
    }
    return ace;
}


// Advisor for giving advice to the player
void advisor(CardArray& playerHand, CardArray& dealerHand, int playerScore, int dealerScore, bool bust) {

    CardArray dHand = dealerHand;

    int goodCardTarget = 17;    // draw cards until 17 if dealer has good card
    int badCardTarget = 12;     // draw cards unitl 12 if dealer has poor card
    int fairCardTarget = 13;    // draw cards unitlt 13 if dealer has fair card


    // Evaluates dealers up card
    cardEvlauator(dHand);

    // If the player and dealer haven't hit black jack and hand isn't bust
    if ((playerScore != BLACKJACK) && (dealerScore != BLACKJACK) && (!bust)) {

        cout << endl << "\t" << KBOLD << "Advice:" << KNRM << " ";

        // If dealer card is good and player hasn't reached target
        if ((playerScore < goodCardTarget) && (cardEvlauator(dHand) == GOOD_CARD)) {
            cout << KYEL << "You should " << KBOLD << "HIT" << KNRM << KYEL << " until your score is at least 17" << KNRM;
        }
        // If dealer card is good and player has reached target
        else if ((playerScore >= goodCardTarget) && (cardEvlauator(dHand) == GOOD_CARD)) {
            cout << KGRN << "You should " << KBOLD << "STAND" << KNRM << KGRN << " since your score is 17 or above" << KNRM;
        }

        // If dealer card is bad and player hasn't reached target
        if ((playerScore < badCardTarget) && (cardEvlauator(dHand) == BAD_CARD)) {
            cout << KYEL << "You should " << KBOLD << "HIT" << KNRM << KYEL << " until your score is at least 12" << KNRM;
        }
        // If dealer card is good and player has reached target
        else if ((playerScore >= badCardTarget) && (cardEvlauator(dHand) == BAD_CARD)) {
            cout << KGRN << "You should " << KBOLD << "STAND" << KNRM << KGRN << " since your score is 12 or above" << KNRM;
        }

        // If dealer card is fair and player hasn't reached target
        if ((playerScore < fairCardTarget) && (cardEvlauator(dHand) == FAIR_CARD)) {
            cout << KYEL << "You should " << KBOLD << "HIT" << KNRM << KYEL << " until your score is at least 13" << KNRM;
        }
        // If dealer card is fair and player has reached target
        else if ((playerScore >= fairCardTarget) && (cardEvlauator(dHand) == FAIR_CARD)) {
            cout << KGRN << "You should " << KBOLD << "STAND" << KNRM << KGRN << " since your score is 13 or above" << KNRM;
        }
    // (Player score printed separately with soft/hard totals)

    }
}

// Evaluates whether the dealer up card is good, bad or fair
int cardEvlauator(const CardArray& hand) {

    // Initialized Card values for good, bad or fair card threshold
    int goodCardValue = 7;
    int badCardValue = 4;

    // If statements to find the whether card is good, bad or fair
    if (1 < hand.cards[0].cardRank && hand.cards[0].cardRank < badCardValue) {
        return FAIR_CARD;
    }
    else if (badCardValue <= hand.cards[0].cardRank && hand.cards[0].cardRank < goodCardValue) {
        return BAD_CARD;
    }
    else {
        return GOOD_CARD;
    }
}

// Compute soft (best <=21 considering one ace as 11 if possible) and hard totals
void computeTotals(const CardArray& hand, int &softTotal, int &hardTotal, int &aceCount) {
    hardTotal = 0;
    aceCount = 0;
    for (int i = 0; i < hand.usedCards; ++i) {
        int rank = hand.cards[i].cardRank;
        if (rank == 1) {
            aceCount++;
            hardTotal += 1; // count ace as 1 for hard total
        }
        else if (rank >= 11) {
            hardTotal += 10;
        }
        else {
            hardTotal += rank;
        }
    }
    // soft total: if there is at least one ace and adding 10 keeps us <=21, do it
    softTotal = hardTotal;
    if (aceCount > 0 && (softTotal + 10) <= BLACKJACK) {
        softTotal += 10;
    }
}

void initialAdvisor(CardArray& dealerHand, int playerScore, int dealerScore) {

    // Evaluates dealers up card
    cardEvlauator(dealerHand);

    // Conditions for calling advisor
    if ((playerScore != BLACKJACK) && (dealerScore != BLACKJACK) && (playerScore < BLACKJACK)) {

        // Statement for observation
        cout << endl << endl << "\tADVISOR observation: ";
        // If up card is good
        if (cardEvlauator(dealerHand) == GOOD_CARD) {
            cout << "The dealer upcard is good.";
        }
        // If up card is bad
        if (cardEvlauator(dealerHand) == BAD_CARD) {
            cout << "The dealer upcard is bad.";
        }
        // If up card is fair
        if (cardEvlauator(dealerHand) == FAIR_CARD) {
            cout << "The dealer upcard is fair.";
        }
    }
}

