#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 50

typedef struct {
    char lhs;
    char rhs[20];
} Production;

Production prods[MAX];
int prodCount;

typedef struct {
    int prodIndex;
    int dotPos;
} Item;

typedef struct {
    Item items[MAX];
    int count;
} ItemSet;

ItemSet states[MAX];
int stateCount = 0;

int equals(Item a, Item b) {
    return (a.prodIndex == b.prodIndex && a.dotPos == b.dotPos);
}

int itemInSet(ItemSet *set, Item item) {
    for (int i = 0; i < set->count; i++)
        if (equals(set->items[i], item))
            return 1;
    return 0;
}

void addItem(ItemSet *set, Item item) {
    if (!itemInSet(set, item)) {
        set->items[set->count++] = item;
    }
}

void closure(ItemSet *set) {
    int added = 1;
    while (added) {
        added = 0;
        for (int i = 0; i < set->count; i++) {
            Item it = set->items[i];
            char *rhs = prods[it.prodIndex].rhs;

            if (it.dotPos < strlen(rhs)) {
                char symbol = rhs[it.dotPos];      // symbol after dot

                if (symbol >= 'A' && symbol <= 'Z') {
                    for (int p = 0; p < prodCount; p++) {
                        if (prods[p].lhs == symbol) {
                            Item newItem = {p, 0};
                            if (!itemInSet(set, newItem)) {
                                addItem(set, newItem);
                                added = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}

int sameSet(ItemSet *a, ItemSet *b) {
    if (a->count != b->count)
        return 0;

    for (int i = 0; i < a->count; i++) {
        int found = 0;
        for (int j = 0; j < b->count; j++) {
            if (equals(a->items[i], b->items[j])) {
                found = 1;
                break;
            }
        }
        if (!found)
            return 0;
    }
    return 1;
}

int findState(ItemSet *set) {
    for (int i = 0; i < stateCount; i++)
        if (sameSet(&states[i], set))
            return i;
    return -1;
}

ItemSet GOTO(ItemSet *set, char symbol) {
    ItemSet next = {0};
    for (int i = 0; i < set->count; i++) {
        Item it = set->items[i];
        char *rhs = prods[it.prodIndex].rhs;

        if (it.dotPos < strlen(rhs) && rhs[it.dotPos] == symbol) {
            Item shifted = {it.prodIndex, it.dotPos + 1};
            addItem(&next, shifted);
        }
    }
    closure(&next);
    return next;
}

void constructLR0() {
    // I0: closure of augmented production
    ItemSet start = {0};
    Item it0 = {0, 0};
    addItem(&start, it0);
    closure(&start);

    states[stateCount++] = start;

    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < stateCount; i++) {
            for (char c = 'A'; c <= 'z'; c++) {
                ItemSet next = GOTO(&states[i], c);
                if (next.count == 0) continue;

                int exists = findState(&next);
                if (exists == -1) {
                    states[stateCount++] = next;
                    changed = 1;
                }
            }
        }
    }
}

void printLR0() {
    printf("\n======= LR(0) ITEM SETS =======\n");
    for (int s = 0; s < stateCount; s++) {
        printf("\nState I%d:\n", s);
        for (int i = 0; i < states[s].count; i++) {
            int p = states[s].items[i].prodIndex;
            int pos = states[s].items[i].dotPos;

            printf("  %c → ", prods[p].lhs);

            for (int j = 0; j < strlen(prods[p].rhs); j++) {
                if (pos == j) printf(".");
                printf("%c", prods[p].rhs[j]);
            }

            if (pos == strlen(prods[p].rhs))
                printf(".");

            printf("\n");
        }
    }
}

// ------------------------ MAIN ----------------------------

int main() {
    printf("Enter number of productions: ");
    scanf("%d", &prodCount);

    printf("Enter productions (Format A=alpha):\n");

    char input[50];
    for (int i = 0; i < prodCount; i++) {
        scanf("%s", input);
        prods[i].lhs = input[0];
        strcpy(prods[i].rhs, input + 2);
    }

    // Augment grammar: add S' → S
    for (int i = prodCount; i > 0; i--)
        prods[i] = prods[i - 1];

    prods[0].lhs = prods[1].lhs + 1;  // New start symbol
    sprintf(prods[0].rhs, "%c", prods[1].lhs);

    prodCount++;

    constructLR0();
    printLR0();

    return 0;
}
