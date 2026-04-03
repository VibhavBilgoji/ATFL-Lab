#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void print_step(int curr, char* remaining) {
    int len = strlen(remaining);
    printf("\t= ");
    for (int i = 0; i < len; i++) printf("δ(");
    printf("q%d", curr);
    for (int i = 0; i < len; i++) printf(", %c)", remaining[i]);
    printf("\n");
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    int states, symbols, fin_count;
    printf("Enter number of states: ");
    if(scanf("%d", &states) != 1) return 1;
    printf("Enter number of symbols: ");
    if(scanf("%d", &symbols) != 1) return 1;
    int** table = (int**)malloc(states * sizeof(int*));
    for(int i = 0; i < states; i++) {
        table[i] = (int*)malloc(symbols * sizeof(int));
    }

    printf("\nEnter transitions:\n");
    for (int i = 0; i < states; i++) {
        for (int j = 0; j < symbols; j++) {
            printf("State q%d on '%c' -> q", i, 'a' + j);
            scanf("%d", &table[i][j]);
        }
    }
    printf("\nEnter number of final states: ");
    if(scanf("%d", &fin_count) != 1) return 1;
    int* finals = (int*)malloc(fin_count * sizeof(int));
    printf("Enter final states: ");
    for (int i = 0; i < fin_count; i++) scanf("%d", &finals[i]);
    while(true) {
        char input[100];
        printf("\n--------------------------------\n");
        printf("Enter string: ");
        scanf("%s", input);
        printf("\nSteps:\n");
        printf("δ*(q0, %s)\n", input);
        int curr = 0;
        int len = strlen(input);
        for (int i = 0; i <= len; i++) {
            print_step(curr, input + i);
            if (i < len) {
                int sym = input[i] - 'a';
                curr = table[curr][sym];
            }
        }
        bool accepted = false;
        for (int i = 0; i < fin_count; i++) {
            if (curr == finals[i]) {
                accepted = true;
                break;
            }
        }
        if (accepted) printf("\n : Accepted\n");
        else printf("\n : Rejected\n");
        printf("\nContinue? (1=Yes, 0=No): ");
        int choice;
        scanf("%d", &choice);
        if (!choice) break;
    }
    free(finals);
    for(int i = 0; i < states; i++) free(table[i]);
    free(table);
    return 0;
}