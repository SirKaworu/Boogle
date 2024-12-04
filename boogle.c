#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define totalAlphabet 26
#define Modulo 500

struct SlangCharacter
{
    struct SlangCharacter *children[totalAlphabet];
    char character;
    bool isLeaf;
};

struct Dictionary
{
    int key;
    char slangWord[50];
    char definition[100];
    struct Dictionary *left;
    struct Dictionary *right;
};

struct SlangCharacter *trieRoot = NULL;
struct Dictionary *bstRoot = NULL;

struct SlangCharacter *createNewCharacterNode(char character)
{
    struct SlangCharacter *newNode = malloc(sizeof(struct SlangCharacter));
    newNode->isLeaf = false;
    newNode->character = character;
    for (int i = 0; i < totalAlphabet; i++)
    {
        newNode->children[i] = NULL;
    }
    return newNode;
}

void insertSlang(struct SlangCharacter *root, char *slang)
{
    int length = strlen(slang);
    struct SlangCharacter *currNode = root;

    for (int i = 0; i < length; i++)
    {
        int index = slang[i] - 'a';
        if (currNode->children[index] == NULL)
        {
            currNode->children[index] = createNewCharacterNode(slang[i]);
        }
        currNode = currNode->children[index];
    }
    currNode->isLeaf = true;
}

int getSlangKey(char *slang)
{
    int length = strlen(slang);
    int key = 0;
    for (int i = 0; i < length; i++)
    {
        key = key + slang[i];
    }
    return key % Modulo;
}

struct Dictionary *findSlangInDictionary(struct Dictionary *node, int key, char *slangWord)
{
    if (node == NULL)
    {
        return NULL;
    }
    if (key == node->key && strcmp(node->slangWord, slangWord) == 0)
    {
        return node;
    }
    if (key < node->key)
    {
        return findSlangInDictionary(node->left, key, slangWord);
    }
    return findSlangInDictionary(node->right, key, slangWord);
}

struct Dictionary *searchSlangInDictionary(struct Dictionary *node, int key, char *slangWord)
{
    if (node == NULL)
    {
        return NULL;
    }
    if (key == node->key && strcmp(node->slangWord, slangWord) == 0)
    {
        return node;
    }
    if (key < node->key)
    {
        return searchSlangInDictionary(node->left, key, slangWord);
    }
    return searchSlangInDictionary(node->right, key, slangWord);
}

struct Dictionary *addToDictionary(struct Dictionary *node, int key, char *slangWord, char *description)
{
    if (node == NULL)
    {
        struct Dictionary *newNode = malloc(sizeof(struct Dictionary));
        newNode->key = key;
        strcpy(newNode->slangWord, slangWord);
        strcpy(newNode->definition, description);
        newNode->left = newNode->right = NULL;
        return newNode;
    }
    if (key < node->key)
    {
        node->left = addToDictionary(node->left, key, slangWord, description);
    }
    else if (key > node->key)
    {
        node->right = addToDictionary(node->right, key, slangWord, description);
    }
    return node;
}

void releaseNewSlangWord()
{
    char slangWord[50];
    char description[100];
    char tempDescription[100];

    while (1)
    {
        printf("Input a new slang word [Must be more than 1 characters and contains no space]: ");
        scanf("%s", slangWord);
        if (strlen(slangWord) > 1 && strchr(slangWord, ' ') == NULL)
        {
            break;
        }
        printf("Invalid input. Please try again.\n");
    }

    while (1)
    {
        printf("Input a new slang word description [Must be more than 2 words]: ");
        getchar();
        fgets(description, sizeof(description), stdin);
        strcpy(tempDescription, description);

        description[strcspn(description, "\n")] = 0;

        int wordCount = 0;
        char *token = strtok(tempDescription, " ");
        while (token != NULL)
        {
            wordCount++;
            token = strtok(NULL, " ");
        }
        if (wordCount > 2)
        {
            break;
        }
        printf("Invalid input. Please try again.\n");
    }

    int key = getSlangKey(slangWord);
    struct Dictionary *existingNode = findSlangInDictionary(bstRoot, key, slangWord);
    if (existingNode != NULL)
    {

        strcpy(existingNode->definition, description);
        printf("Successfully updated a slang word.\n");
    }
    else
    {
        if (trieRoot == NULL)
        {
            trieRoot = createNewCharacterNode('\0');
        }
        insertSlang(trieRoot, slangWord);

        bstRoot = addToDictionary(bstRoot, key, slangWord, description);
        printf("Successfully released new slang word.\n");
    }
    printf("Press enter to continue...\n");
    getchar();
}

bool searchSlang(struct SlangCharacter *root, char *slang)
{
    int length = strlen(slang);
    struct SlangCharacter *currNode = root;

    for (int i = 0; i < length; i++)
    {
        int index = slang[i] - 'a';
        if (currNode->children[index] == NULL)
        {
            return false;
        }
        currNode = currNode->children[index];
    }
    return currNode != NULL && currNode->isLeaf;
}

void searchSlangWord()
{
    char slangWord[50];
    while (1)
    {
        printf("Input a slang word to be searched [Must be more than 1 characters and contains no space]: ");
        scanf("%s", slangWord);
        if (strlen(slangWord) > 1 && strchr(slangWord, ' ') == NULL)
        {
            break;
        }
    }

    if (trieRoot == NULL || !searchSlang(trieRoot, slangWord))
    {
        printf("\nThere is no word \"%s\" in the dictionary.\n", slangWord);
    }
    else
    {
        int key = getSlangKey(slangWord);
        struct Dictionary *resultNode = searchSlangInDictionary(bstRoot, key, slangWord);
        if (resultNode == NULL)
        {
            printf("\nThere is no word \"%s\" in the dictionary.\n", slangWord);
        }
        else
        {
            printf("\nSlang word\t: %s\nDescription\t: %s\n", resultNode->slangWord, resultNode->definition);
        }
    }
    printf("\nPress enter to continue...\n");
    getchar();
    getchar();
}

void collectWords(struct SlangCharacter *node, char *prefix, char **words, int *count)
{
    if (node == NULL)
    {
        return;
    }

    if (node->isLeaf)
    {
        words[*count] = malloc(strlen(prefix) + 1);
        strcpy(words[*count], prefix);
        (*count)++;
    }

    for (int i = 0; i < totalAlphabet; i++)
    {
        if (node->children[i] != NULL)
        {
            char newPrefix[100];
            sprintf(newPrefix, "%s%c", prefix, node->children[i]->character);
            collectWords(node->children[i], newPrefix, words, count);
        }
    }
}

void searchWordsWithPrefix(struct SlangCharacter *root, char *prefix, char **words, int *count)
{
    int length = strlen(prefix);
    struct SlangCharacter *currNode = root;

    for (int i = 0; i < length; i++)
    {
        int index = prefix[i] - 'a';
        if (currNode->children[index] == NULL)
        {
            return;
        }
        currNode = currNode->children[index];
    }

    collectWords(currNode, prefix, words, count);
}

void viewWordsWithPrefix()
{
    char prefix[50];
    printf("Input a prefix to be searched: ");
    scanf("%s", prefix);

    char *words[100];
    int count = 0;

    if (trieRoot == NULL || strlen(prefix) < 1)
    {
        printf("\nThere is no prefix \"%s\" in the dictionary.\n", prefix);
    }
    else
    {
        searchWordsWithPrefix(trieRoot, prefix, words, &count);
        if (count == 0)
        {
            printf("\nThere is no prefix \"%s\" in the dictionary.\n", prefix);
        }
        else
        {

            for (int i = 0; i < count - 1; i++)
            {
                for (int j = i + 1; j < count; j++)
                {
                    if (strcmp(words[i], words[j]) > 0)
                    {
                        char *temp = words[i];
                        words[i] = words[j];
                        words[j] = temp;
                    }
                }
            }

            printf("\nWords that has \"%s\":\n", prefix);
            for (int i = 0; i < count; i++)
            {
                printf("%d. %s\n", i + 1, words[i]);
                free(words[i]);
            }
        }
    }
    printf("Press enter to continue...\n");
    getchar();
    getchar();
}

void viewAllSlangWords()
{
    if (trieRoot == NULL)
    {
        printf("\nThere is no slang word yet in the dictionary.\n");
    }
    else
    {
        char *words[100];
        int count = 0;

        collectWords(trieRoot, "", words, &count);

        if (count == 0)
        {
            printf("\nThere is no slang word yet in the dictionary.\n");
        }
        else
        {

            for (int i = 0; i < count - 1; i++)
            {
                for (int j = i + 1; j < count; j++)
                {
                    if (strcmp(words[i], words[j]) > 0)
                    {
                        char *temp = words[i];
                        words[i] = words[j];
                        words[j] = temp;
                    }
                }
            }

            printf("\nList of all slang words in the dictionary:\n");
            for (int i = 0; i < count; i++)
            {
                printf("%d. %s\n", i + 1, words[i]);
                free(words[i]);
            }
        }
    }
    printf("\nPress enter to continue...\n");
    getchar();
    getchar();
}

int main()
{
    int menu;
    do
    {
        printf("\n");
        printf("==============================\n");
        printf("||          BOOGLE          ||\n");
        printf("==============================\n");

        printf("1. Release a new slang word\n");
        printf("2. Search for a slang word\n");
        printf("3. Search for words with a prefix\n");
        printf("4. List all slang words\n");
        printf("5. Exit\n");
        printf("Choose a menu: ");
        scanf("%d", &menu);

        switch (menu)
        {
        case 1:
            releaseNewSlangWord();
            break;
        case 2:
            searchSlangWord();
            break;
        case 3:
            viewWordsWithPrefix();
            break;
        case 4:
            viewAllSlangWords();
            break;
        default:
            printf("\nThank you... Have a nice day :)\n");
            break;
        }
    } while (menu != 5);

    return 0;
}