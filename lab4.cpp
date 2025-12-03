#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 20

char prod[MAX][MAX];
char first[MAX][MAX];
char follow[MAX][MAX];
char table[MAX][MAX][MAX];
char terminals[MAX];
char nonterminals[MAX];

int pCount, tCount = 0, ntCount = 0;

// --------------------------- UTILITY -----------------------------

int isTerminal(char c) {
    return !(c >= 'A' && c <= 'Z');
}

int ntIndex(char c) {
    for(int i=0; i<ntCount; i++)
        if(nonterminals[i] == c) return i;
    return -1;
}

int tIndex(char c) {
    for(int i=0; i<tCount; i++)
        if(terminals[i] == c) return i;
    return -1;
}

// ---------------------- FIRST SET COMPUTATION --------------------

void addToSet(char set[], char c) {
    if(!strchr(set, c)) {
        int len = strlen(set);
        set[len] = c;
        set[len+1] = '\0';
    }
}

void computeFIRST(char X, char result[]) {
    result[0] = '\0';

    for(int i=0; i<pCount; i++) {
        if(prod[i][0] == X) {
            char *rhs = prod[i] + 2;
            char c = rhs[0];

            if(isTerminal(c)) {
                addToSet(result, c);
            } else {
                char temp[MAX] = "";
                computeFIRST(c, temp);
                strcat(result, temp);
            }
        }
    }
}

// --------------------- FOLLOW SET COMPUTATION ---------------------

void computeFOLLOW(char X, char result[]) {
    result[0] = '\0';

    if(X == nonterminals[0])
        addToSet(result, '$');

    for(int i=0; i<pCount; i++) {
        char *rhs = prod[i] + 2;

        for(int j=0; rhs[j] != '\0'; j++) {
            if(rhs[j] == X) {
                if(rhs[j+1] != '\0') {
                    char next = rhs[j+1];

                    if(isTerminal(next)) {
                        addToSet(result, next);
                    } else {
                        strcat(result, first[ntIndex(next)]);
                    }
                } else {
                    strcat(result, follow[ntIndex(prod[i][0])]);
                }
            }
        }
    }
}

// ---------------------- BUILD PARSING TABLE -----------------------

void buildTable() {
    // Initialize table
    for(int i=0; i<ntCount; i++)
        for(int j=0; j<tCount; j++)
            strcpy(table[i][j], "-");

    for(int i=0; i<pCount; i++) {
        char A = prod[i][0];
        char *rhs = prod[i] + 2;

        char fs[MAX] = "";
        if(isTerminal(rhs[0])) {
            fs[0] = rhs[0];
            fs[1] = '\0';
        } else {
            strcpy(fs, first[ntIndex(rhs[0])]);
        }

        for(int k=0; k<strlen(fs); k++) {
            int row = ntIndex(A);
            int col = tIndex(fs[k]);
            strcpy(table[row][col], rhs);
        }
    }
}

// ---------------------- PREDICTIVE PARSING -----------------------

void parse(char *input) {
    char stack[MAX];
    int top = -1;

    stack[++top] = '$';
    stack[++top] = nonterminals[0];

    int ip = 0;
    printf("\nParsing Steps:\n");

    while(1) {
        char X = stack[top];
        char a = input[ip];

        printf("Stack: %s | Input: %s\n", stack, input + ip);

        if(X == '$' && a == '$') {
            printf("\n✔ ACCEPTED: Parsing completed successfully.\n");
            return;
        }

        if(isTerminal(X)) {
            if(X == a) {
                top--;
                ip++;
            } else {
                printf("\n✖ ERROR: Expected '%c', found '%c'\n", X, a);
                return;
            }
        } else {
            int row = ntIndex(X);
            int col = tIndex(a);

            if(row == -1 || col == -1 || strcmp(table[row][col], "-") == 0) {
                printf("\n✖ ERROR: No rule for %c with input %c\n", X, a);
                return;
            }

            char *rhs = table[row][col];
            top--;

            int len = strlen(rhs);
            for(int k=len-1; k>=0; k--) {
                if(rhs[k] != 'e')
                    stack[++top] = rhs[k];
            }
        }
    }
}

// ------------------------------ MAIN ------------------------------

int main() {
    printf("Enter number of productions: ");
    scanf("%d", &pCount);

    printf("Enter productions (Format A=alpha):\n");
    for(int i=0; i<pCount; i++) {
        scanf("%s", prod[i]);
        char A = prod[i][0];

        if(!strchr(nonterminals, A))
            nonterminals[ntCount++] = A;

        char *rhs = prod[i] + 2;
        for(int j=0; rhs[j] != '\0'; j++) {
            if(isTerminal(rhs[j]) && rhs[j] != 'e') {
                if(!strchr(terminals, rhs[j]))
                    terminals[tCount++] = rhs[j];
            }
        }
    }
    terminals[tCount++] = '$';

    // Compute FIRST sets
    for(int i=0; i<ntCount; i++)
        computeFIRST(nonterminals[i], first[i]);

    // Compute FOLLOW sets
    for(int i=0; i<ntCount; i++)
        computeFOLLOW(nonterminals[i], follow[i]);

    // Build LL(1) table
    buildTable();

    // Print FIRST
    printf("\n----- FIRST -----\n");
    for(int i=0; i<ntCount; i++)
        printf("FIRST(%c) = { %s }\n", nonterminals[i], first[i]);

    // Print FOLLOW
    printf("\n----- FOLLOW -----\n");
    for(int i=0; i<ntCount; i++)
        printf("FOLLOW(%c) = { %s }\n", nonterminals[i], follow[i]);

    // Print Parsing Table
    printf("\n----- LL(1) PARSING TABLE -----\n   ");
    for(int i=0; i<tCount; i++)
        printf("%5c", terminals[i]);
    printf("\n");

    for(int i=0; i<ntCount; i++) {
        printf("%c |", nonterminals[i]);
        for(int j=0; j<tCount; j++)
            printf("%5s", table[i][j]);
        printf("\n");
    }

    // Parsing input
    char input[MAX];
    printf("\nEnter input string: ");
    scanf("%s", input);
    strcat(input, "$");

    parse(input);

    return 0;
}
