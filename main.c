#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NEW_GAME 0
#define START_INSERT 1
#define FILTERED_PRINT 2

typedef struct TreeNode {
    struct TreeNode *left;
    struct TreeNode *right;
    struct TreeNode *nextValid;
    char word[];
} TreeNode;

typedef struct InsertNode {
    TreeNode *validNode;
    struct InsertNode *left;
    struct InsertNode *right;
} InsertNode;

typedef struct ht {
    int minOccurrence;
    int exactOccurrence;
    char *constr;
} ht;

char *existingChars;
ht *constraints;
int lastExisting = 0;
int insertedWords;
int remainingWords;
bool chars[123] = {false};
TreeNode *start;
TreeNode *lastSeen;
TreeNode *prevNode;
TreeNode *currNode;

//Print to the terminal the entire dictionary
void inorder (TreeNode *x, int numOfChar) {
    if (x == NULL) {
        return;
    }
    inorder(x->left, numOfChar);
    for (int i = 0; i < numOfChar; ++i) {
        putchar_unlocked(x->word[i]);
    }
    putchar_unlocked('\n');
    inorder(x->right, numOfChar);
}

void tree_insert (InsertNode **root, InsertNode *z) {
    InsertNode *prev = NULL;
    InsertNode *curr = *root;
    while (curr != NULL) {
        prev = curr;
        if (strcmp(z->validNode->word, curr->validNode->word) < 0) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }
    if (prev == NULL) {
        *root = z;
    } else if (strcmp(z->validNode->word, prev->validNode->word) < 0) {
        prev->left = z;
    } else {
        prev->right = z;
    }
}

void tree_insert_dictionary (TreeNode **root, TreeNode *z) {
    TreeNode *prev = NULL;
    TreeNode *curr = *root;
    while (curr != NULL) {
        prev = curr;
        if (strcmp(z->word, curr->word) < 0) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }
    if (prev == NULL) {
        *root = z;
    } else if (strcmp(z->word, prev->word) < 0) {
        prev->left = z;
    } else {
        prev->right = z;
    }
}

//Check that the words satisfy the constraints learned until now
bool filter (const char *word, int numOfChar, const char *correctPosition) {

    for (int i = 0; i < lastExisting; ++i) {
        for (int j = 0; j < numOfChar; ++j) {
            if (existingChars[i] == word[j]) {
                break;
            } else {
                if (j == numOfChar - 1) {
                    return false;
                }
            }
        }
    }

    int countTot[78] = {0};

    for (int i = 0; i < numOfChar; ++i) {
        int idx = word[i] - '-';
        countTot[idx]++;
    }

    for (int i = 0; i < numOfChar; ++i) {

        int c = (int) word[i];

        if (chars[c] == true) {
            return false;
        }

        if (correctPosition[i] != '*') {
            if (word[i] != correctPosition[i]) {
                return false;
            }
        }

        ht *temp = &constraints[c];
        if (word[i] == temp->constr[i]) {
            return false;
        }

        int idx = c - '-';
        int count = countTot[idx];

        int occurrence = temp->exactOccurrence;
        if (occurrence != 0) {
            if (count != occurrence) {
                return false;
            }
        } else {
            if (count < temp->minOccurrence) {
                return false;
            }
        }
    }

    return true;
}

//Inserisco le parole che prendo dall'input nell'albero e gestisco anche il caso in cui il dizionario
//di parole da inserire è all'interno del blocco +inserisci_inizio / +inserisci_fine

//Insert the words taken from the input into the tree
TreeNode* get_dictionary(int numOfChar, TreeNode *root, const char *correctPosition, bool isInsert,
                         InsertNode **insertRoot, bool firstWord) {
    char c;
    int len = 0;
    int lenCmd = 0;
    bool exit = false;
    bool skip = false;
    TreeNode *new_node;
    char temp[numOfChar + 1];
    char cmd[18];
    while (true) {
        while ((c = (char) getchar_unlocked()) != '\n') {
            skip = false;
            if (c == '+'){
                lenCmd = 0;
                while ((c = (char) getchar_unlocked())!= '\n') {
                    cmd[lenCmd] = c;
                    lenCmd++;
                }
                cmd[lenCmd] = '\0';
                if (strcmp(cmd, "nuova_partita") == 0) {
                    exit = true;
                    break;
                }
                if (strcmp(cmd, "inserisci_inizio") == 0) {
                    skip = true;
                }
                if (strcmp(cmd, "inserisci_fine") == 0) {
                    if(isInsert) {
                        exit = true;
                        break;
                    }
                    skip = true;
                }
                if (strcmp(cmd, "stampa_filtrate") == 0) {
                    inorder(root, numOfChar);
                    skip = true;
                }
            }
            if (!skip) {
                temp[len] = c;
                len += 1;
            }
        }
        if (exit) {
            break;
        }
        temp[len] = '\0';
        len = 0;
        new_node = (TreeNode*) malloc(sizeof(TreeNode) + numOfChar + 1);
        strcpy((new_node->word), temp);
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->nextValid = NULL;
        tree_insert_dictionary(&root, new_node);
        if (isInsert && !firstWord) {
            if (filter(temp, numOfChar, correctPosition)) {
                InsertNode *new_inserted_node = (InsertNode *) malloc(sizeof(InsertNode));
                new_inserted_node->validNode = new_node;
                new_inserted_node->left = NULL;
                new_inserted_node->right = NULL;
                tree_insert(insertRoot, new_inserted_node);
            }
        }
    }
    return root;
}

void get_word_to_find(char* wordToFind, int numOfChar){
    char tmp[numOfChar + 1];
    char c;
    int i = 0;
    while ((c = (char) getchar_unlocked()) != '\n') {
        tmp[i] = c;
        i += 1;
    }
    tmp[i] = '\0';
    strcpy(wordToFind, tmp);
}

int get_tries() {
    int tries;
    if (scanf("%d\n", &tries) == 1) {
        return tries;
    }
    return 0;

}

void deallocate (InsertNode *node) {
    if (node == NULL) {
        return;
    }
    deallocate(node->left);
    deallocate(node->right);
    free (node);
}

void deallocate_dictionary (TreeNode *node) {
    if (node == NULL) {
        return;
    }
    deallocate_dictionary(node->left);
    deallocate_dictionary(node->right);
    free (node);
}

TreeNode *existence(TreeNode *x, char *wordToCheck) {
    while (x != NULL && strcmp(x->word, wordToCheck) != 0) {
        if (strcmp(wordToCheck, x->word) < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    return x;
}

//Controllo quali caratteri non devono esistere nella parola
void check_non_existing_chars (const char *comparedString, const char *word, int numOfChar) {
    for (int i = 0; i < numOfChar; ++i) {
        if (comparedString[i] == '/') {
            int c = (int) word[i];
            chars[c] = true;
            for (int j = 0; j < numOfChar; ++j) {
                if ((word[j] == word[i]) && comparedString[j] != '/'){
                    if (j != i) {
                        chars[c] = false;
                    }
                }
            }
        }
    }
}

//Controllo il numero di occorrenze (minime ed esatte)
void occurrence_check (const char *comparedString, const char *word, int numOfChar) {
    int countRight[78] = {0};

    //Conto il numero totale di caratteri in posizione scorretta e corretta
    for (int i = 0; i < numOfChar; ++i) {
        int idx = word[i] - '-';
        if (comparedString[i] == '|' || comparedString[i] == '+')
            countRight[idx]++;
    }

    int countTot[78] = {0};

    //Conto il numero totale di caratteri
    for (int i = 0; i < numOfChar; ++i) {
        int idx = word[i] - '-';
        countTot[idx]++;
    }

    //Se il numero di caratteri in giusta posizione + il numero di caratteri in posizione
    //scorretta > occorrenze totali conosco le occorrenze esatte
    for (int i = 0; i < numOfChar; ++i) {
        int c = word[i] - '-';
        if (countRight[c] != 0) {
            if (countRight[c] < countTot[c])
                constraints[c + '-'].exactOccurrence = countRight[c];
            else
                if (countRight[c] > constraints[c + '-'].minOccurrence)
                    constraints[c + '-'].minOccurrence = countRight[c];
        }
    }
}

void update_non_existing_chars(int c, int idx) {
    constraints[c].constr[idx] = (char) c;
}

//Aggiorno la stringa che contiene i caratteri che devono esistere senza inserire duplicati
void update_existing_chars(char c) {
    bool exists = false;
    if (lastExisting == 0) {
        existingChars[lastExisting] = c;
        lastExisting++;
        existingChars[lastExisting] = '\0';
    } else {
        for (int i = 0; i < lastExisting; ++i) {
            if (existingChars[i] == c) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            existingChars[lastExisting] = c;
            lastExisting ++;
            existingChars[lastExisting] = '\0';
        }
    }
}

//Creo la stringa di output aggiornando i vincoli di esistenza
void compare_strings(char *word, char *wordToFind, char *comparedString, int numOfChar, char *correctPosition) {
    char tmp[numOfChar + 1];
    char tmpToFind[numOfChar + 1];
    strcpy(tmpToFind, wordToFind);
    strcpy(tmp, word);

    for (int i = 0; i < numOfChar; ++i) {
        if (word[i] == wordToFind[i]){
            tmp[i] = '+';
            tmpToFind[i] = '*';
            correctPosition[i] = word[i];
            update_existing_chars(word[i]);
        }
    }

    for (int i = 0; i < numOfChar; ++i) {
        for (int j = 0; j < numOfChar; ++j) {
            if (tmp[i] == '+'){
                break;
            }
            else {
                if (tmpToFind[j] == '*'){}
                else{
                    if (word[i] == tmpToFind[j]) {
                        update_existing_chars(word[i]);
                        update_non_existing_chars(word[i], i);
                        tmpToFind[j] = '*';
                        tmp[i] = '|';
                        break;
                    } else {
                        update_non_existing_chars(word[i], i);
                        tmp[i] = '/';
                    }
                }
            }
        }
    }
    tmp[numOfChar] = '\0';
    strcpy(comparedString, tmp);
}

//Controllo l'idoneità delle parole iterando sull'albero
void check_nodes (TreeNode *node, int numOfChar, char *correctPosition) {
    if (node != NULL) {
        check_nodes(node->left, numOfChar, correctPosition);
        if (filter(node->word, numOfChar, correctPosition)) {
            lastSeen->nextValid = node;
            lastSeen = node;
            lastSeen->nextValid = NULL;
            insertedWords++;
        }
        check_nodes(node->right, numOfChar, correctPosition);
    }
}

//Controllo l'idoneità delle sole parole valide
void check_nodes2 (TreeNode *node, int numOfChar, const char *correctPosition) {
    node = node->nextValid;
    while (node != NULL) {
        if (filter(node->word, numOfChar, correctPosition)) {
            lastSeen->nextValid = node;
            lastSeen = node;
            remainingWords ++;
        }
        node = node->nextValid;
    }
    lastSeen->nextValid = NULL;
}

//Inserisco le parole valide del blocco +inserisci_inizio / +inserisci_fine nella pseudo lista
void insert (InsertNode *insertRoot) {
    if (insertRoot != NULL) {
        insert(insertRoot->left);
        while (strcmp(currNode->word, insertRoot->validNode->word) < 0) {
            prevNode = currNode;
            currNode = currNode->nextValid;
            if (currNode == NULL) {
                break;
            }
        }
        prevNode->nextValid = insertRoot->validNode;
        insertRoot->validNode->nextValid = currNode;
        currNode = insertRoot->validNode;
        insert(insertRoot->right);
    }
}

//Stampo le parole valide
void inorder_valid (TreeNode *head, int numOfChar) {
    head = head->nextValid;
    while (head != NULL) {
        for (int i = 0; i < numOfChar; ++i) {
            putchar_unlocked(head->word[i]);
        }
        putchar_unlocked('\n');
        head = head->nextValid;
    }
}

int main() {
    int numOfChar = 0;
    TreeNode *root = NULL;
    char str[10];
    char c;
    int len = 0;
    char cmd[18];
    int op = 3;

    constraints = (ht*) malloc (123 * sizeof (ht));

    start = (TreeNode*) malloc (sizeof (TreeNode));
    start->nextValid = start;

    if (fgets(str, 10, stdin) != NULL) {
        numOfChar = strtol(str, NULL, 0);
    }

    existingChars = (char*) malloc ((numOfChar + 1) * sizeof (char));

    for (int i = 45; i < 123; ++i) {
        constraints[i].constr = malloc(numOfChar * sizeof (char));
        memset(constraints[i].constr, '*', numOfChar);
        constraints[i].minOccurrence = 0;
        constraints[i].exactOccurrence = 0;
    }

    root = get_dictionary(numOfChar, NULL, NULL, false, NULL, true);
    InsertNode *insertRoot = NULL;
    lastSeen = start;

    char wordToFind[numOfChar + 1];
    get_word_to_find(wordToFind, numOfChar);

    int tries = get_tries();

    char tempWord[numOfChar + 1];
    char comparedString[numOfChar + 1];
    bool endKo = false;
    bool noMoreTriesCheck = true;
    bool firstWord = true;
    char correctPosition[numOfChar + 1];
    memset(correctPosition, '*', numOfChar);
    correctPosition[numOfChar] = '\0';
    while ((c = (char) getchar_unlocked()) != EOF) {
        if (noMoreTriesCheck) {
            if (tries == 0) {
                if (!endKo) {
                    putchar_unlocked('k');
                    putchar_unlocked('o');
                    putchar_unlocked('\n');
                }
                noMoreTriesCheck = false;
                while (c != '+') {
                    c = (char) getchar_unlocked();
                }
                endKo = false;
            }
        }
        if (c == '+') {
            while ((c = (char) getchar_unlocked()) != '\n') {
                cmd[len] = c;
                len += 1;
            }
            cmd[len] = '\0';
            len = 0;
            if (strcmp(cmd, "nuova_partita") == 0) {
                op = NEW_GAME;
            } else if (strcmp(cmd, "inserisci_inizio") == 0) {
                op = START_INSERT;
            } else if (strcmp(cmd, "stampa_filtrate") == 0) {
                op = FILTERED_PRINT;
            }
            switch (op) {
                case (NEW_GAME):
                    get_word_to_find(wordToFind, numOfChar);
                    tries = get_tries();
                    lastExisting = 0;
                    existingChars[lastExisting] = '\0';
                    memset(correctPosition, '*', numOfChar);
                    for (int i = 45; i < 123; ++i) {
                        memset(constraints[i].constr, '*', numOfChar);
                        constraints[i].minOccurrence = 0;
                        constraints[i].exactOccurrence = 0;
                        chars[i] = false;
                    }
                    noMoreTriesCheck = true;
                    firstWord = true;
                    insertedWords = 0;
                    start->nextValid = start;
                    lastSeen = start;
                    break;
                case (START_INSERT):
                    root = get_dictionary(numOfChar, root, correctPosition, true, &insertRoot, firstWord);
                    if (!firstWord) {
                        lastSeen = start;
                        insertedWords = 0;
                        prevNode = start;
                        currNode = start->nextValid;
                        insert (insertRoot);
                        deallocate(insertRoot);
                        insertRoot = NULL;
                    }
                    break;
                case (FILTERED_PRINT):
                    if (firstWord) {
                        inorder (root, numOfChar);
                    } else {
                        inorder_valid(start, numOfChar);
                    }
                    break;
                default:
                    break;
            }
        } else {
            if (c == '\n') {
                c = (char) getchar_unlocked();
            }
            tempWord[len] = c;
            len += 1;
            while ((c = (char) getchar_unlocked()) != '\n') {
                tempWord[len] = c;
                len += 1;
            }
            tempWord[len] = '\0';
            len = 0;
            if (strcmp(tempWord, wordToFind) != 0) {
                if ((existence(root, tempWord)) != NULL) {
                    compare_strings(tempWord, wordToFind, comparedString, numOfChar, correctPosition);
                    occurrence_check(comparedString, tempWord, numOfChar);
                    check_non_existing_chars(comparedString, tempWord, numOfChar);
                    for (int i = 0; i < numOfChar; ++i) {
                        putchar_unlocked(comparedString[i]);
                    }
                    putchar_unlocked('\n');
                    if (firstWord) {
                        insertedWords = 0;
                        check_nodes(root, numOfChar, correctPosition);
                        firstWord = false;
                        printf("%d\n", insertedWords);
                    } else {
                        lastSeen = start;
                        remainingWords = 0;
                        check_nodes2(start, numOfChar, correctPosition);
                        printf ("%d\n", remainingWords);
                    }
                    tries -= 1;
                    if (tries == 0) {
                        putchar_unlocked('k');
                        putchar_unlocked('o');
                        putchar_unlocked('\n');
                        endKo = true;
                    }
                } else {
                    putchar_unlocked('n');
                    putchar_unlocked('o');
                    putchar_unlocked('t');
                    putchar_unlocked('_');
                    putchar_unlocked('e');
                    putchar_unlocked('x');
                    putchar_unlocked('i');
                    putchar_unlocked('s');
                    putchar_unlocked('t');
                    putchar_unlocked('s');
                    putchar_unlocked('\n');
                }
            } else {
                putchar_unlocked('o');
                putchar_unlocked('k');
                putchar_unlocked('\n');
            }
        }
    }

    for (int i = 45; i < 123; ++i) {
        free(constraints[i].constr);
    }
    free(constraints);
    free(existingChars);
    free(start);
    deallocate_dictionary(root);

    return 0;
}