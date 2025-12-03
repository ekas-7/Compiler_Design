#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 50

char productions[MAX][MAX];
char first[MAX][MAX];
char follow[MAX][MAX];
char nonterminals[MAX];
int prodCount = 0, nontermCount = 0;

/* Utility: Check if symbol is non-terminal */
int isNonTerminal(char c) {
    return c >= 'A' && c <= 'Z';
}

/* Add character to a set if not already present */
void addToSet(char set[], char c) {
    if (!strchr(set, c)) {
        int len = strlen(set);
        set[len] = c;
        set[len+1] = '\0';
    }
}

/* Compute FIRST(X) */
void computeFirst(char symbol, char result[]) {
    int i;
    /* Terminal → FIRST is itself */
    if (!isNonTerminal(symbol)) {
        addToSet(result, symbol);
        return;
    }

    /* Non-terminal → check productions */
    for (i = 0; i < prodCount; i++) {
        if (productions[i][0] == symbol) {
            int k = 2;  // start after "A="
            if (productions[i][k] == '#') {
                addToSet(result, '#');
            } else {
                int canEpsilon = 1;
                while (productions[i][k] != '\0' && canEpsilon) {
                    char temp[50] = "";
                    computeFirst(productions[i][k], temp);

                    canEpsilon = 0;
                    for (int j = 0; temp[j] != '\0'; j++) {
                        if (temp[j] == '#')
                            canEpsilon = 1;
                        else
                            addToSet(result, temp[j]);
                    }

                    if (!canEpsilon)
                        break;
                    k++;
                }

                if (canEpsilon)
                    addToSet(result, '#');
            }
        }
    }
}

/* Compute FOLLOW(X) */
void computeFollow(char symbol, char result[]) {
    if (symbol == nonterminals[0]) {
        addToSet(result, '$'); // Start symbol gets $
    }

    for (int i = 0; i < prodCount; i++) {
        char *p = productions[i];
        int len = strlen(p);

        for (int j = 2; j < len; j++) {
            if (p[j] == symbol) {
                /* Case 1: A → αBβ */
                if (j + 1 < len) {
                    char next = p[j+1];
                    char temp[50] = "";

                    computeFirst(next, temp);

                    int epsilonFound = 0;

                    for (int k = 0; temp[k] != '\0'; k++) {
                        if (temp[k] == '#')
                            epsilonFound = 1;
                        else
                            addToSet(result, temp[k]);
                    }

                    if (!epsilonFound)
                        continue;

                    if (epsilonFound && isNonTerminal(next)) {  
                        computeFollow(p[0], result);
                    }
                }
                /* Case 2: A → αB  (B at end) */
                else {
                    if (p[j] == symbol && p[0] != symbol) {
                        computeFollow(p[0], result);
                    }
                }
            }
        }
    }
}

int main() {
    printf("Enter number of productions: ");
    scanf("%d", &prodCount);

    printf("Enter productions (Format A=α):\n");
    for (int i = 0; i < prodCount; i++) {
        scanf("%s", productions[i]);

        char nt = productions[i][0];
        if (!strchr(nonterminals, nt)) {
            nonterminals[nontermCount++] = nt;
        }
    }

    /* Compute FIRST sets */
    for (int i = 0; i < nontermCount; i++) {
        char temp[50] = "";
        computeFirst(nonterminals[i], temp);
        strcpy(first[i], temp);
    }

    /* Compute FOLLOW sets */
    for (int i = 0; i < nontermCount; i++) {
        char temp[50] = "";
        computeFollow(nonterminals[i], temp);
        strcpy(follow[i], temp);
    }

    /* Output */
    printf("\n----- FIRST & FOLLOW Sets -----\n");
    for (int i = 0; i < nontermCount; i++) {
        printf("FIRST(%c) = { %s }\n", nonterminals[i], first[i]);
        printf("FOLLOW(%c) = { %s }\n\n", nonterminals[i], follow[i]);
    }

    return 0;
}
