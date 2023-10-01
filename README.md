# API-Project_2022

The development of this project is part of the algorithms and data structures course at the Polytechnic University of Milan, and as a final examination, it is necessary for the bachelor's degree in computer engineering. The course was held by prof. Matteo Pradella, Davide Martinenghi and Alessandro Barenghi in the academic year 2021-2022.<br>
***Final score: 30/30 cum laude***

## Project specification
The aim of this project is to apply the knowledge acquired during the course to implement an efficient solution to a specific problem. The project is evaluated on memory efficiency and speed of execution.

The specific program to implement was a simplified version of the <a href="https://www.nytimes.com/games/wordle/index.html"> Wordle </a> game where the user have to guess a word from a fintie dictionary. 

## How to Play
The system read from standard input a sequence of instructons and gives strings as output depending on the cases.
Specifically teh program reads:
- a value k that define the lenght of the words in the dictionary
- a sequence (of arbitrary length) of words, each with length k, constituting the set of admissible words

After this, a sequence of matches is read, each starting with the `+nuova_partita` command.
The input of a match consists of:
- the word to be guessed
- maximum number n of attempts
- sequence of tried word 

During a game (between attempts) the `+stampa_filtrate` command may appear, this consists of printing in lexicographic order all the words compatible with the constraints learnt from the various attempts.

Both during and between games, the commands `+inserisci_inizio` and `+inserisci_fine` may appear. Between these commands there is a sequence of new words, which are added to the set of admissible words. 

The game can end in two possible way. If the word guessed is the objective word the system prints to standard output ok. Otherwise if the number of guess exceeds the maximum number of tries n the game end with result ko.

For the full explaination and example of how the output is generated given a tried word see [pdf](/docs/Project%20specification%20ENG.pdf) (also available in [Italian](/docs/Project%20specification%20IT.pdf)).

## Implementation choices
This project is implemented in C11 using only the [standard C libraries](https://en.wikipedia.org/wiki/C_standard_library).

The program use a binary tree to store the words in a lexicographical order. Other basic data structures are used to store the constraints learned each time a new word is guessed. The key point of the program is that each binary tree node has a pointer to the next valid word (a word that respect the learned constraints). In this way we can avoid going through the entire tree every time we need to check if a word is valid or not. The idea behind this implementation is to reduce the time spent iterating the tree, but without using a helper list reducing further the memory utilization.

## Tools used
- **Valgrind** - Dinamic analysis
- **Callgrind** - Dinamic analysis
- **Massif (Massif-Visualizer)** - Allocated memory visualizer 
- **Address-Sanitizer**  - Memory error detector
- **GDB** - Debugger
- **GCC + Ubuntu** - Development environment