#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// List of standard C keywords
const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
};

int keywordCount = sizeof(keywords) / sizeof(keywords[0]);

// Function to check if a string is a keyword
int isKeyword(char *word) {
    for (int i = 0; i < keywordCount; i++) {
        if (strcmp(word, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

// Function to check if a character is a delimiter
// Note: Spaces and newlines are delimiters but typically not printed as tokens
int isDelimiter(char ch) {
    char delimiters[] = " \t\n,;(){}[]#\"<>"; 
    for (int i = 0; delimiters[i] != '\0'; i++) {
        if (ch == delimiters[i])
            return 1;
    }
    return 0;
}

// Function to check if a character is an operator
int isOperator(char ch) {
    char operators[] = "+-*/%=&|!^";
    for (int i = 0; operators[i] != '\0'; i++) {
        if (ch == operators[i])
            return 1;
    }
    return 0;
}

// Function to determine if a string is a number
int isNumber(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

int main() {
    FILE *fp;
    char filename[100];
    char ch, buffer[100];
    int i = 0;

    // Prompt user for filename
    printf("Enter C source filename (e.g., input.c): ");
    scanf("%s", filename);

    fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Error: Could not open file '%s'.\n", filename);
        return 1;
    }

    printf("\n--- Lexical Analysis Results ---\n");

    while ((ch = fgetc(fp)) != EOF) {
        // Check if the character is a delimiter or operator
        if (isDelimiter(ch) || isOperator(ch)) {
            
            // If buffer has content, process the token found before this delimiter
            if (i != 0) {
                buffer[i] = '\0'; // Null-terminate string
                i = 0; // Reset buffer index

                if (isKeyword(buffer)) {
                    printf("Keyword:    %s\n", buffer);
                } else if (isNumber(buffer)) {
                    printf("Number:     %s\n", buffer);
                } else {
                    printf("Identifier: %s\n", buffer);
                }
            }

            // Process the delimiter or operator itself
            if (isOperator(ch)) {
                printf("Operator:   %c\n", ch);
            } else if (isDelimiter(ch)) {
                // Only print specific delimiters, ignore whitespace for cleaner output
                if (ch != ' ' && ch != '\t' && ch != '\n') {
                    printf("Delimiter:  %c\n", ch);
                }
            }
        } else {
            // If not a delimiter/operator, add char to buffer (building a word/number)
            buffer[i++] = ch;
        }
    }

    fclose(fp);
    return 0;
}