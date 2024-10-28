#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

extern struct NODE* root;
extern struct NODE* cwd;

// Function to find a child directory by name within a parent node
struct NODE* findChild(struct NODE* parent, const char* name) {
    struct NODE* child = parent->childPtr;
    while (child != NULL) {
        if (strcmp(child->name, name) == 0 && child->fileType == 'D') {
            return child;
        }
        child = child->siblingPtr;
    }
    return NULL;
}

// splitPath function to split path into dirName and baseName
struct NODE* splitPath(char* pathName, char* baseName, char* dirName) {
    struct NODE* currentNode;
    char pathCopy[256];  // Assuming pathName won't exceed 256 characters
    strcpy(pathCopy, pathName);

    // Determine whether the path is absolute or relative
    if (pathCopy[0] == '/') {
        currentNode = root;  // Start from root for absolute paths
        strcpy(dirName, "/");
    } else {
        currentNode = cwd;   // Start from current working directory for relative paths
        dirName[0] = '\0';
    }

    // Set up baseName and dirName
    char* lastSlash = strrchr(pathCopy, '/');  // Find the last '/'
    if (lastSlash != NULL) {
        *lastSlash = '\0';  // Split pathName into dirName and baseName
        strcpy(baseName, lastSlash + 1);
        strcpy(dirName, pathCopy);
    } else {
        // No '/' in pathName, so it's in the current directory
        strcpy(baseName, pathCopy);
        dirName[0] = '\0';
    }

    // Traverse directories in dirName
    char* token = strtok(dirName, "/");
    while (token != NULL && currentNode != NULL) {
        currentNode = findChild(currentNode, token);
        if (currentNode == NULL) {
            printf("ERROR: directory %s does not exist\n", token);
            return NULL;
        }
        token = strtok(NULL, "/");
    }

    return currentNode;
}

// mkdir function to create a new directory
void mkdir(char pathName[]) {
    // Check if pathName is only "/"
    if (strcmp(pathName, "/") == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    // Setup variables to hold baseName and dirName
    char baseName[256];
    char dirName[256];
    struct NODE* parentDir = splitPath(pathName, baseName, dirName);

    // Check if splitPath returned a valid parent directory
    if (parentDir == NULL) {
        printf("MKDIR ERROR: parent directory not found\n");
        return;
    }

    // Search for the target directory to check if it already exists
    struct NODE* child = parentDir->childPtr;
    while (child != NULL) {
        if (strcmp(child->name, baseName) == 0 && child->fileType == 'D') {
            printf("MKDIR ERROR: directory '%s' already exists\n", baseName);
            return;
        }
        child = child->siblingPtr;
    }

    // Allocate memory for the new directory node
    struct NODE* newNode = (struct NODE*)malloc(sizeof(struct NODE));
    if (newNode == NULL) {
        printf("MKDIR ERROR: memory allocation failed\n");
        return;
    }

    // Initialize the new directory node fields
    // Initialize the new directory node fields
    strncpy(newNode->name, baseName, sizeof(newNode->name) - 1);
    newNode->name[sizeof(newNode->name) - 1] = '\0';  // Ensure null-termination
    newNode->fileType = 'D';                // Set fileType as directory
    newNode->parentPtr = parentDir;         // Set parent pointer
    newNode->childPtr = NULL;               // No children initially
    newNode->siblingPtr = NULL;             // No siblings initially

    // Insert the new node into the parent's children list
    if (parentDir->childPtr == NULL) {
        // First child of the parent
        parentDir->childPtr = newNode;
    } else {
        // Add as the last sibling
        struct NODE* lastSibling = parentDir->childPtr;
        while (lastSibling->siblingPtr != NULL) {
            lastSibling = lastSibling->siblingPtr;
        }
        lastSibling->siblingPtr = newNode;
    }

    // Success message
    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
}


