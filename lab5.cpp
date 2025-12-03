#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 20

char productions[MAX][MAX];
int pCount;

// ------------------------------------------------------------
//  Utility Functions
// ------------------------------------------------------------

void printGrammar(char arr[][MAX], int count) {
    printf("\nTransformed Grammar:\n");
    for(int i=0; i<count; i++)
        printf("%s\n", arr[i]);
}

// ------------------------------------------------------------
//  Remove Immediate Left Recursion
// ------------------------------------------------------------

int removeLeftRecursion(char A, char prodList[][MAX], int count,
                        char newProd[][MAX], int *newCount) {
    char alpha[MAX][MAX], beta[MAX][MAX];
    int aCount = 0, bCount = 0;

    for(int i=0; i<count; i++) {
        if(prodList[i][2] == A) {          // A → Aα (left recursion)
            strcpy(alpha[aCount], prodList[i] + 3);
            aCount++;
        } else {                             // A → β
            strcpy(beta[bCount], prodList[i] + 2);
            bCount++;
        }
    }

    if(aCount == 0)
        return 0;  // no left recursion

    char Aprime = A + 1; // Derive new non-terminal

    // A → βA'
    for(int i=0; i<bCount; i++) {
        sprintf(newProd[*newCount], "%c=%s%c", A, beta[i], Aprime);
        (*newCount)++;
    }

    // A' → αA' | ε
    for(int i=0; i<aCount; i++) {
        sprintf(newProd[*newCount], "%c=%s%c", Aprime, alpha[i], Aprime);
        (*newCount)++;
    }

    sprintf(newProd[*newCount], "%c=e", Aprime); // epsilon
    (*newCount)++;

    return 1;
}

// ------------------------------------------------------------
//  LEFT FACTORING
// ------------------------------------------------------------

int leftFactor(char A, char prodList[][MAX], int count,
               char newProd[][MAX], int *newCount) {

    char prefix[MAX] = "";
    int plen = 0;

    // Find longest common prefix
    for(int j = 2; prodList[0][j] != '\0'; j++) {
        char c = prodList[0][j];
        for(int i=1; i<count; i++)
            if(prodList[i][j] != c)
                goto PREF_DONE;
        prefix[plen++] = c;
    }

PREF_DONE:
    prefix[plen] = '\0';

    if(strlen(prefix) == 0)
        return 0;  // No common prefix → no factoring

    char Aprime = A + 1;

    // A → prefix A'
    sprintf(newProd[*newCount], "%c=%s%c", A, prefix, Aprime);
    (*newCount)++;

    // A' productions
    for(int i=0; i<count; i++) {
        char *rhs = prodList[i] + 2;
        if(strncmp(rhs, prefix, plen) == 0) {
            sprintf(newProd[*newCount], "%c=%s",
                    Aprime, rhs + plen);
            (*newCount)++;
        }
    }

    return 1;
}

// ------------------------------------------------------------
//  MAIN
// ------------------------------------------------------------

int main() {
    printf("Enter number of productions: ");
    scanf("%d", &pCount);

    printf("Enter productions (Format A=alpha):\n");
    for(int i=0; i<pCount; i++)
        scanf("%s", productions[i]);

    char newProd[2*MAX][MAX];
    int newCount = 0;

    for(int i=0; i<pCount; i++) {
        char A = productions[i][0];
        char prodList[MAX][MAX];
        int count = 0;

        // Collect all productions for A
        for(int j = i; j<pCount; j++) {
            if(productions[j][0] == A)
                strcpy(prodList[count++], productions[j]);
        }

        // Skip duplicates in main loop
        i += count - 1;

        // FIRST attempt: Remove immediate left recursion
        if(removeLeftRecursion(A, prodList, count, newProd, &newCount))
            continue;

        // SECOND attempt: Left factoring
        if(leftFactor(A, prodList, count, newProd, &newCount))
            continue;

        // Otherwise copy as-is
        for(int k=0; k<count; k++)
            strcpy(newProd[newCount++], prodList[k]);
    }

    printGrammar(newProd, newCount);

    return 0;
}
