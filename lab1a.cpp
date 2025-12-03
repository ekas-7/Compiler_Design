#include <stdio.h>
#include <ctype.h>

int main() {
    FILE *fp;
    char filename[100];
    char ch;
    int wordCount = 0, lineCount = 1, charCount = 0;
    int inWord = 0;

    printf("Enter filename: ");
    scanf("%s", filename);

    fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Error: Could not open file '%s'.\n", filename);
        return 1;
    }

    while ((ch = fgetc(fp)) != EOF) {
        charCount++;

        if (ch == '\n')
            lineCount++;

        if (!isspace(ch)) {
            if (!inWord) {
                wordCount++;
                inWord = 1;
            }
        } else {
            inWord = 0;
        }
    }

    fclose(fp);

    printf("\n--- File Statistics ---\n");
    printf("Total Lines:      %d\n", lineCount);
    printf("Total Words:      %d\n", wordCount);
    printf("Total Characters: %d\n", charCount);

    return 0;
}
