/********************
 Name: Ido Itzhak
 ID: 69420
 ********************/

#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str) {
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0) {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int) strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src) {
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *) malloc(len + 1);
    if (!dest) {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt) {
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success) {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0) {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int) strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0') {
            printf("Invalid input.\n");
        } else {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type) {
    switch (type) {
        case GRASS:
            return "GRASS";
        case FIRE:
            return "FIRE";
        case WATER:
            return "WATER";
        case BUG:
            return "BUG";
        case NORMAL:
            return "NORMAL";
        case POISON:
            return "POISON";
        case ELECTRIC:
            return "ELECTRIC";
        case GROUND:
            return "GROUND";
        case FAIRY:
            return "FAIRY";
        case FIGHTING:
            return "FIGHTING";
        case PSYCHIC:
            return "PSYCHIC";
        case ROCK:
            return "ROCK";
        case GHOST:
            return "GHOST";
        case DRAGON:
            return "DRAGON";
        case ICE:
            return "ICE";
        default:
            return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput() {
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *) malloc(capacity);
    if (!input) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (size + 1 >= capacity) {
            capacity *= 2;
            char *temp = (char *) realloc(input, capacity);
            if (!temp) {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char) c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// --------------------------------------------------------------
// Utility: Binary Search Tree Generic Functions.
// --------------------------------------------------------------
void BFSGeneric(PokemonNode *root, VisitNodeFunc visit) {
    Queue *myQ = createQueue();
    enQueue(myQ, root);
    while (!isQueueEmpty(myQ)) {
        PokemonNode *current = popQueue(myQ);
        visit(current);
        if (current->left != NULL) {
            enQueue(myQ, current->left);
        }
        if (current->right != NULL) {
            enQueue(myQ, current->right);
        }
    }
    free(myQ);
}

void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (!root) {
        return;
    }
    visit(root);
    preOrderGeneric(root->left, visit);
    preOrderGeneric(root->right, visit);
}

void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (!root) {
        return;
    }
    inOrderGeneric(root->left, visit);
    visit(root);
    inOrderGeneric(root->right, visit);
}

void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (!root) {
        return;
    }
    postOrderGeneric(root->left, visit);
    postOrderGeneric(root->right, visit);
    visit(root);
}

// --------------------------------------------------------------
// Utility: Queue Functions for BFS
// --------------------------------------------------------------
Queue *createQueue() {
    Queue *myQ = (Queue *) malloc(sizeof(Queue));
    if (!myQ) {
        printf("allocation failed at creating queue\n");
        exit(1);
    }
    myQ->front = NULL;
    myQ->rear = NULL;
    return myQ;
}

QueueNode *createQueueNode(PokemonNode *data) {
    QueueNode *new = malloc(sizeof(QueueNode));
    if (!new) {
        printf("Allocation failed at creating queue node\n");
        exit(1);
    }
    new->data = data;
    new->next = NULL;
    return new;
}

int isQueueEmpty(Queue *myQ) {
    return (myQ->front == NULL);
}

void enQueue(Queue *myQ, PokemonNode *node) {
    QueueNode *newQueueNode = createQueueNode(node);
    if (myQ->front == NULL) {
        myQ->front = newQueueNode;
        myQ->rear = newQueueNode;
    } else {
        myQ->rear->next = newQueueNode;
        myQ->rear = newQueueNode;
    }
}

//remove first item from queue, returns its data (if exists).
PokemonNode *popQueue(Queue *myQ) {
    if (isQueueEmpty(myQ)) {
        return NULL;
    }
    QueueNode *temp = myQ->front;
    myQ->front = myQ->front->next;
    //we got last item from queue.
    if (myQ->front == NULL) {
        myQ->rear = NULL;
    }
    PokemonNode *data = temp->data;
    free(temp);
    return data;
}

void destroyQueue(Queue *myQ) {
    while (myQ->front != NULL) {
        popQueue(myQ);
    }
    free(myQ);
}


// --------------------------------------------------------------
// Display alphabetical related functions
// --------------------------------------------------------------
void initNodeArray(NodeArray *na) {
    na->capacity = 0;
    na->nodes = malloc(sizeof(PokemonNode *));
    if (!na->nodes) {
        printf("allocation failed in init Array\n");
        exit(1);
    }
}

void addNode(NodeArray *na, PokemonNode *node) {
    na->nodes = realloc(na->nodes, (na->capacity + 1) * sizeof(PokemonNode *));
    if (!na->nodes) {
        printf("Reallocation failed in adding note to array, alphabetical sorting pokemons\n");
        exit(1);
    }
    na->nodes[na->capacity] = node;
    na->capacity++;
}

void collectAll(PokemonNode *root, NodeArray *na) {
    if (!root) {
        return;
    }
    collectAll(root->left, na);
    addNode(na, root);
    collectAll(root->right, na);
}

void sortAlphabetical(NodeArray *nodeArray) {
    int isSwapped = 0;
    for (int i = 0; i < nodeArray->capacity - 1; i++) {
        isSwapped = 0;
        for (int j = 0; j < nodeArray->capacity - i - 1; j++) {
            if (strcmp(nodeArray->nodes[j]->data->name, nodeArray->nodes[j + 1]->data->name) > 0) {
                //swap
                PokemonNode *temp = nodeArray->nodes[j];
                nodeArray->nodes[j] = nodeArray->nodes[j + 1];
                nodeArray->nodes[j + 1] = temp;
                isSwapped=1;
            }
        }
        if (!isSwapped) {
            break;
        }
    }
}

// --------------------------------------------------------------
// Pokemon Nodes BST related functions
// --------------------------------------------------------------

PokemonNode *createPokemonNode(const PokemonData *data) {
    PokemonNode *new = malloc(sizeof(PokemonNode));
    if (!new) {
        printf("Allocation failed at creating Pokemon Node\n");
        exit(1);
    }
    new->data = data;
    new->left = NULL;
    new->right = NULL;
    return new;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node) {
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode) {
    if (!root) {
        root = newNode;
        printf("Pokemon %s (ID %d) added.\n", newNode->data->name, newNode->data->id);
        return root;
    }
    if (root->data->id == newNode->data->id) {
        //meaning we got duplicate, free the node.
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", newNode->data->id);
        freePokemonNode(newNode);
        return root;
    }
    if (root->data->id < newNode->data->id) {
        if (root->right == NULL) {
            root->right = newNode;
            printf("Pokemon %s (ID %d) added.\n", newNode->data->name, newNode->data->id);
        } else {
            insertPokemonNode(root->right, newNode);
        }
        return root;
    }
    if (root->data->id > newNode->data->id) {
        if (root->left == NULL) {
            root->left = newNode;
            printf("Pokemon %s (ID %d) added.\n", newNode->data->name, newNode->data->id);
        } else {
            insertPokemonNode(root->left, newNode);
        }
        return root;
    }
    return root;
}

PokemonNode *silentInsertPokemonNode(PokemonNode *root, PokemonNode *newNode) {
    if (!root) {
        root = newNode;
        return root;
    }
    if (root->data->id == newNode->data->id) {
        //meaning we got duplicate, free the node.
        freePokemonNode(newNode);
        return root;
    }
    if (root->data->id < newNode->data->id) {
        if (root->right == NULL) {
            root->right = newNode;
        } else {
            silentInsertPokemonNode(root->right, newNode);
        }
        return root;
    }
    if (root->data->id > newNode->data->id) {
        if (root->left == NULL) {
            root->left = newNode;
        } else {
            silentInsertPokemonNode(root->left, newNode);
        }
        return root;
    }
    return root;
}

PokemonNode *searchPokemonBFS(PokemonNode *root, int id) {
    Queue *myQ = createQueue();
    enQueue(myQ, root);

    while (!isQueueEmpty(myQ)) {
        PokemonNode *current = popQueue(myQ);
        if (current->data->id == id) {
            destroyQueue(myQ);
            return current;
        }
        if (current->left != NULL) {
            enQueue(myQ, current->left);
        }
        if (current->right != NULL) {
            enQueue(myQ, current->right);
        }
    }

    destroyQueue(myQ);
    return NULL;
}

PokemonNode *removePokemonByID(PokemonNode *root, int id) {
    PokemonNode *toRemove = searchPokemonBFS(root, id);
    if (!toRemove) {
        printf("No Pokemon with ID %d found.\n", id);
        return root;
    }
    printf("Removing Pokemon %s (ID %d).\n", toRemove->data->name, toRemove->data->id);
    root = removeNodeBST(root, id);
    return root;
}

PokemonNode *removeNodeBST(PokemonNode *root, int id) {
    PokemonNode *temp = NULL;
    //go deeper in the tree according to given id;
    if (root->data->id < id) {
        root->right = removeNodeBST(root->right, id);
    } else if (root->data->id > id) {
        root->left = removeNodeBST(root->left, id);
    } else {
        /*root is the node we want to delete, we return its replacement.
         *we divide into cases:
         *case1: root has no children */
        if (!root->left && !root->right) {
            freePokemonNode(root);
            return NULL;
        }
        //case2: root has one child.
        if (root->left && !root->right) {
            temp = root->left;
            freePokemonNode(root);
            return temp;
        }
        if (!root->left && root->right) {
            temp = root->right;
            freePokemonNode(root);
            return temp;
        }
        //root has 2 children
        if (root->left && root->right) {
            //find the right replacement
            temp = root->right;
            while (temp->left) {
                temp = temp->left;
            }
            root->data = temp->data;
            root->right = removeNodeBST(root->right, temp->data->id);
        }
    }
    return root;
}

void freePokemonNode(PokemonNode *node) {
    free(node);
}

void freePokemonTree(PokemonNode *root) {
    if (!root) {
        return;
    }
    postOrderGeneric(root, freePokemonNode);
}


// --------------------------------------------------------------
// Owner Nodes related functions
// --------------------------------------------------------------
OwnerNode *createOwner(char *ownerName, PokemonNode *starter) {
    OwnerNode *new = malloc(sizeof(OwnerNode));
    if (!new) {
        printf("Allocation failed at creating Owner Node\n");
        exit(1);
    }
    new->ownerName = ownerName;
    new->pokedexRoot = starter;
    new->next = NULL;
    new->prev = NULL;
    return new;
}

//insert owner to the circular list in the last place, FIFO.
void linkOwnerInCircularList(OwnerNode *newOwner) {
    if (ownerHead == NULL) {
        ownerHead = newOwner;
        newOwner->next = newOwner;
        newOwner->prev = newOwner;
        return;
    }
    newOwner->prev = ownerHead->prev;
    newOwner->next = ownerHead;
    ownerHead->prev->next = newOwner;
    ownerHead->prev = newOwner;
}

void removeOwnerFromCircularList(OwnerNode *target) {
    if (ownerHead == target) {
        if (target->next == target) {
            //only item in the list.
            ownerHead = NULL;
        } else {
            ownerHead = target->next;
            target->prev->next = target->next;
            target->next->prev = target->prev;
        }
    } else {
        target->prev->next = target->next;
        target->next->prev = target->prev;
    }
}

OwnerNode *findOwnerByName(const char *name) {
    //if no owners exists yet, cant be duplicates.
    if (ownerHead == NULL) {
        return NULL;
    }
    OwnerNode *current = ownerHead;
    do {
        if (!strcmp(name, current->ownerName)) {
            return current;
        }
        current = current->next;
    } while (current != ownerHead);
    return NULL;
}

void freeOwnerNode(OwnerNode *owner) {
    free(owner->ownerName);
    freePokemonTree(owner->pokedexRoot);
    free(owner);
}

int countOwners() {
    if (ownerHead == NULL) {
        return 0;
    }
    OwnerNode *temp = ownerHead;
    int i = 0;
    do {
        temp = temp->next;
        i++;
    } while (temp != ownerHead);
    return i;
}

void swapOwnerData(OwnerNode *a, OwnerNode *b) {
    OwnerNode c;
    c.ownerName = a->ownerName;
    c.pokedexRoot = a->pokedexRoot;

    a->ownerName = b->ownerName;
    a->pokedexRoot = b->pokedexRoot;

    b->ownerName = c.ownerName;
    b->pokedexRoot = c.pokedexRoot;
}

// --------------------------------------------------------------
// Manage Owners (Delete, merge, sort, print).
// --------------------------------------------------------------
void deletePokedex() {
    OwnerNode *cur = ownerHead;
    int choice;
    //makes sure there are existing pokedexes.
    if (cur == NULL) {
        printf("No existing Pokedexes to delete.\n");
        return;
    }
    // list owners and choose
    printf("\n=== Delete a Pokedex ===\n");
    int i = 1;
    do {
        printf("%d. %s\n", i++, cur->ownerName);
        cur = cur->next;
    } while (cur != ownerHead);
    choice = readIntSafe("Choose a Pokedex to delete by number: ");
    cur = ownerHead;
    for (int j = 1; j < choice; j++) {
        cur = cur->next;
    }
    printf("Deleting %s's entire Pokedex...\n", cur->ownerName);
    removeOwnerFromCircularList(cur);
    freeOwnerNode(cur);
    printf("Pokedex deleted.\n");
}

void mergePokedexMenu() {
    int numOfOwners = countOwners();
    if (numOfOwners < 2) {
        printf("Not enough owners to merge.\n");
        return;
    }
    printf("\n=== Merge Pokedexes ===\n");
    char *ownerName1, *ownerName2;
    printf("Enter name of first owner: ");
    ownerName1 = getDynamicInput();
    printf("Enter name of second owner: ");
    ownerName2 = getDynamicInput();
    OwnerNode *owner1 = findOwnerByName(ownerName1);
    OwnerNode *owner2 = findOwnerByName(ownerName2);
    if (!owner2 || !owner1) {
        printf("One or both owners not found.\n");
        return;
    }
    printf("Merging %s and %s...\n", ownerName1, ownerName2);

    Queue *myQ = createQueue();
    enQueue(myQ, owner2->pokedexRoot);
    PokemonNode *temp = NULL;
    while (!isQueueEmpty(myQ)) {
        PokemonNode *current = popQueue(myQ);
        temp = createPokemonNode(current->data);
        owner1->pokedexRoot = silentInsertPokemonNode(owner1->pokedexRoot, temp);
        if (current->left != NULL) {
            enQueue(myQ, current->left);
        }
        if (current->right != NULL) {
            enQueue(myQ, current->right);
        }
    }
    free(myQ);
    printf("Merge completed.\n");
    removeOwnerFromCircularList(owner2);
    freeOwnerNode(owner2);
    printf("Owner '%s' has been removed after merging.\n", ownerName2);
    free(ownerName1);
    free(ownerName2);
}

void sortOwners() {
    int numOfOwners = countOwners();
    if (numOfOwners < 2) {
        printf("0 or 1 owners only => no need to sort.\n");
        return;
    }
    OwnerNode **ownersTempArray = malloc(numOfOwners * sizeof(OwnerNode *));
    OwnerNode *temp = ownerHead;
    for (int i = 0; i < numOfOwners; i++) {
        ownersTempArray[i] = temp;
        temp = temp->next;
    }
    int isSwapped = 0;
    for (int i = 0; i < numOfOwners - 1; i++) {
        isSwapped = 0;
        for (int j = 0; j < numOfOwners - i - 1; j++) {
            if (strcmp(ownersTempArray[j]->ownerName, ownersTempArray[j + 1]->ownerName) > 0) {
                swapOwnerData(ownersTempArray[j], ownersTempArray[j + 1]);
                isSwapped = 1;
            }
        }
        if (!isSwapped) {
            break;
        }
    }
    printf("Owners sorted by name.\n");
    free(ownersTempArray);
}

void printOwnersCircular() {
    char *choice;
    int numOfOwners = countOwners();
    int numOfPrints;
    OwnerNode *temp = ownerHead;
    if (!numOfOwners) {
        printf("No owners.\n");
        return;
    }
    printf("Enter direction (F or B): ");
    choice = getDynamicInput();
    while (strcmp(choice, "f") != 0 && strcmp(choice, "F") != 0 && strcmp(choice, "b") != 0 && strcmp(choice, "B") !=
           0) {
        printf("Invalid direction, must be L or R.\n");
        printf("Enter direction (F or B): ");
        choice = getDynamicInput();
    }

    if (strcmp(choice, "f") == 0 || strcmp(choice, "F") == 0) {
        numOfPrints = readIntSafe("How many prints? ");
        for (int i = 0; i < numOfPrints; i++) {
            printf("[%d] %s\n", i + 1, temp->ownerName);
            temp = temp->next;
        }
    }

    if (strcmp(choice, "b") == 0 || strcmp(choice, "B") == 0) {
        numOfPrints = readIntSafe("How many prints? ");
        for (int i = 0; i < numOfPrints; i++) {
            printf("[%d] %s\n", i + 1, temp->ownerName);
            temp = temp->prev;
        }
    }

    free(choice);
}

void freeAllOwners() {
    if (ownerHead == NULL) {
        return;
    }
    int numOfOwners = countOwners();
    OwnerNode *temp = ownerHead;
    OwnerNode *temp1;

    for (int i=0; i<numOfOwners; i++) {
        temp1= temp->next;
        freeOwnerNode(temp);
        temp = temp1;
    }
    ownerHead=NULL;
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayBFS(PokemonNode *root) {
    BFSGeneric(root, printPokemonNode);
}

void preOrderTraversal(PokemonNode *root) {
    preOrderGeneric(root, printPokemonNode);
}

void inOrderTraversal(PokemonNode *root) {
    inOrderGeneric(root, printPokemonNode);
}

void postOrderTraversal(PokemonNode *root) {
    postOrderGeneric(root, printPokemonNode);
}

void displayAlphabetical(PokemonNode *root) {
    NodeArray *temp = malloc(sizeof(NodeArray));
    if (!temp) {
        printf("Allocation failed while creating NodeArray, display Alphabetical pokemons\n");
        exit(1);
    }
    initNodeArray(temp);
    collectAll(root, temp);
    sortAlphabetical(temp);
    for (int i = 0; i < temp->capacity; i++) {
        printPokemonNode(temp->nodes[i]);
    }
    free(temp->nodes);
    free(temp);
}


void displayMenu(OwnerNode *owner) {
    if (!owner->pokedexRoot) {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice) {
        case 1:
            displayBFS(owner->pokedexRoot);
            break;
        case 2:
            preOrderTraversal(owner->pokedexRoot);
            break;
        case 3:
            inOrderTraversal(owner->pokedexRoot);
            break;
        case 4:
            postOrderTraversal(owner->pokedexRoot);
            break;
        case 5:
            displayAlphabetical(owner->pokedexRoot);
            break;
        default:
            printf("Invalid choice.\n");
    }
}


// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void addPokemon(OwnerNode *owner) {
    int tempID = readIntSafe("Enter ID to add: ");
    int numOfPokemons = sizeof(pokedex) / sizeof(PokemonData);
    if(tempID>numOfPokemons) {
        printf("Invalid ID.\n");
        return;
    }
    PokemonNode *temp = createPokemonNode(&pokedex[tempID - 1]);
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, temp);
}

// ************************
// display Menu funcs above
// ************************

void freePokemon(OwnerNode *owner) {
    if (!owner->pokedexRoot) {
        printf("No Pokemon to release.\n");
        return;
    }
    int releaseID = readIntSafe("Enter Pokemon ID to release: ");
    owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, releaseID);
}

void pokemonFight(OwnerNode *owner) {
    if (!owner->pokedexRoot) {
        printf("Pokedex is empty.\n");
        return;
    }
    int choice1, choice2;
    double score1, score2;
    choice1 = readIntSafe("Enter ID of the first Pokemon: ");
    choice2 = readIntSafe("Enter ID of the second Pokemon: ");
    PokemonNode *firstPokemon, *secondPokemon;
    firstPokemon = searchPokemonBFS(owner->pokedexRoot, choice1);
    secondPokemon = searchPokemonBFS(owner->pokedexRoot, choice2);
    if (!firstPokemon || !secondPokemon) {
        printf("One or both Pokemon IDs not found.\n");
        return;
    }
    score1 = (firstPokemon->data->attack * 1.5) + (firstPokemon->data->hp * 1.2);
    score2 = (secondPokemon->data->attack * 1.5) + (secondPokemon->data->hp * 1.2);
    printf("Pokemon 1: %s (Score = %.2f)\n", firstPokemon->data->name, score1);
    printf("Pokemon 2: %s (Score = %.2f)\n", secondPokemon->data->name, score2);
    if (score1 > score2) {
        printf("%s wins!\n", firstPokemon->data->name);
        return;
    }
    if (score2 > score1) {
        printf("%s wins!\n", secondPokemon->data->name);
    }
    if (score1 == score2) {
        printf("Its a tie!\n");
    }
}

void evolvePokemon(OwnerNode *owner) {
    if (!owner->pokedexRoot) {
        printf("Cannot evolve. Pokedex empty.\n");
        return;
    }
    int choice = readIntSafe("Enter ID of Pokemon to evolve: ");
    PokemonNode *toEvolve = searchPokemonBFS(owner->pokedexRoot, choice);
    if (!toEvolve) {
        printf("No Pokemon with ID %d found.\n", choice);
    } else if (toEvolve->data->CAN_EVOLVE == CAN_EVOLVE) {
        PokemonNode *evolved = searchPokemonBFS(owner->pokedexRoot, choice + 1);
        //check if we already have the evolution in the pokedex.
        if (!evolved) {
            printf("Removing Pokemon %s (ID %d).\n", toEvolve->data->name, choice);
            silentInsertPokemonNode(owner->pokedexRoot, createPokemonNode(&pokedex[choice]));
            removeNodeBST(owner->pokedexRoot,choice-1);
            printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n", toEvolve->data->name, choice,
                   pokedex[choice].name, choice + 1);
        } else {
            printf("Evolution ID %d (%s) already in the Pokedex. Releasing %s (ID %d).\n", choice + 1,
                   evolved->data->name, toEvolve->data->name, choice);
            printf("Removing Pokemon %s (ID %d).\n", toEvolve->data->name, choice);
            owner->pokedexRoot = removeNodeBST(owner->pokedexRoot, choice);
        }
    } else if (toEvolve->data->CAN_EVOLVE == CANNOT_EVOLVE) {
        printf("%s (ID %d) cannot evolve.\n", toEvolve->data->name, choice);
    }
}


void enterExistingPokedexMenu() {
    OwnerNode *cur = ownerHead;
    int choice;
    //makes sure there are existing pokedexes.
    if (cur == NULL) {
        printf("No existing Pokedexes.\n");
        return;
    }
    // list owners and choose
    printf("\nExisting Pokedexes:\n");
    int i = 1;
    do {
        printf("%d. %s\n", i++, cur->ownerName);
        cur = cur->next;
    } while (cur != ownerHead);
    choice = readIntSafe("Choose a Pokedex by number: ");
    cur = ownerHead;
    for (int j = 1; j < choice; j++) {
        cur = cur->next;
    }

    printf("\nEntering %s's Pokedex...\n", cur->ownerName);

    int subChoice;
    do {
        printf("\n-- %s's Pokedex Menu --\n", cur->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice) {
            case 1:
                addPokemon(cur);
                break;
            case 2:
                displayMenu(cur);
                break;
            case 3:
                freePokemon(cur);
                break;
            case 4:
                pokemonFight(cur);
                break;
            case 5:
                evolvePokemon(cur);
                break;
            case 6:
                printf("Back to Main Menu.\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

// --------------------------------------------------------------
// Add new Pokedex
// --------------------------------------------------------------
void openPokedexMenu() {
    printf("Your name: ");
    char *ownerName = getDynamicInput();
    //check if Owner with given name already exists.
    if (findOwnerByName(ownerName)) {
        printf("Owner '%s' already exists. Not creating a new Pokedex.\n", ownerName);
        free(ownerName);
        return;
    }
    PokemonNode *starter;
    int choice;
    do {
        printf("Choose Starter:\n");
        printf("1. Bulbasaur\n");
        printf("2. Charmander\n");
        printf("3. Squirtle\n");
        choice = readIntSafe("Your choice: ");
        switch (choice) {
            case 1:
                starter = createPokemonNode(&pokedex[0]);
                break;
            case 2:
                starter = createPokemonNode(&pokedex[3]);
                break;
            case 3:
                starter = createPokemonNode(&pokedex[6]);
                break;
            default:
                printf("Invalid Choice\n");
        }
    } while (choice != 1 && choice != 2 && choice != 3);
    OwnerNode *new = createOwner(ownerName, starter);
    linkOwnerInCircularList(new);
    printf("New Pokedex created for %s with starter %s.\n", new->ownerName, new->pokedexRoot->data->name);
}


// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu() {
    int choice;
    do {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice) {
            case 1:
                openPokedexMenu();
                break;
            case 2:
                enterExistingPokedexMenu();
                break;
            case 3:
                deletePokedex();
                break;
            case 4:
                mergePokedexMenu();
                break;
            case 5:
                sortOwners();
                break;
            case 6:
                printOwnersCircular();
                break;
            case 7:
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main() {
    mainMenu();
    freeAllOwners();
    return 0;
}
