#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdbool.h>

typedef struct {
    int* targets;
    int count;
} Transition;

void print_state_set(bool* states, int num_states, bool force_braces) {
    int count = 0;
    for (int i = 0; i < num_states; i++) {
        if (states[i]) count++;
    }

    if (count == 0) {
        printf("∅");
        return;
    }

    if (count == 1 && !force_braces) {
        for (int i = 0; i < num_states; i++) {
            if (states[i]) {
                printf("q%d", i);
                return;
            }
        }
    }

    printf("{");
    bool first = true;
    for (int i = 0; i < num_states; i++) {
        if (states[i]) {
            if (!first) printf(", ");
            printf("q%d", i);
            first = false;
        }
    }
    printf("}");
}



int main() {
    SetConsoleOutputCP(CP_UTF8);

    int num_states, num_symbols;
    printf("Enter number of states: ");
    if(scanf("%d", &num_states) != 1) return 1;
    printf("Enter number of symbols: ");
    if(scanf("%d", &num_symbols) != 1) return 1;

    Transition** table = (Transition**)malloc(num_states * sizeof(Transition*));
    for (int i = 0; i < num_states; i++) {
        table[i] = (Transition*)malloc(num_symbols * sizeof(Transition));
        for (int j = 0; j < num_symbols; j++) {
            printf("Transitions for q%d on '%c' (count): ", i, 'a' + j);
            scanf("%d", &table[i][j].count);
            if (table[i][j].count > 0) {
                table[i][j].targets = (int*)malloc(table[i][j].count * sizeof(int));
                printf("  Enter targets: ");
                for (int k = 0; k < table[i][j].count; k++) scanf("%d", &table[i][j].targets[k]);
            } else {
                table[i][j].targets = NULL;
            }
        }
    }

    int fin_count;
    printf("\nEnter number of final states: ");
    scanf("%d", &fin_count);
    int* finals = (int*)malloc(fin_count * sizeof(int));
    printf("Enter final states: ");
    for (int i = 0; i < fin_count; i++) scanf("%d", &finals[i]);

    while (true) {
        char input[100];
        printf("\n--------------------------------\n");
        printf("Enter string: ");
        scanf("%s", input);

        bool* current_active = (bool*)calloc(num_states, sizeof(bool));
        current_active[0] = true;

        printf("\nstring %s\n", input);

        int input_len = strlen(input);
        for (int i = 0; i < input_len; i++) {
            char symbol = input[i];
            int symbol_idx = symbol - '0'; // Assuming input symbols are '0', '1', etc.
            if (symbol >= 'a' && symbol <= 'z') {
                symbol_idx = symbol - 'a';
            }

            bool* next_active = (bool*)calloc(num_states, sizeof(bool));
            for (int s = 0; s < num_states; s++) {
                if (current_active[s]) {
                    Transition trans = table[s][symbol_idx];
                    for (int t = 0; t < trans.count; t++) {
                        next_active[trans.targets[t]] = true;
                    }
                }
            }

            // Print the step
            printf("δ̂(q0, ");
            for (int j = 0; j <= i; j++) printf("%c", input[j]);
            printf(") = δ(");
            print_state_set(current_active, num_states, false);
            printf(", %c) = ", symbol);
            print_state_set(next_active, num_states, true);
            printf("\n");

            free(current_active);
            current_active = next_active;
        }

        bool accepted = false;
        for (int i = 0; i < num_states; i++) {
            if (current_active[i]) {
                for (int j = 0; j < fin_count; j++) {
                    if (i == finals[j]) { accepted = true; break; }
                }
            }
        }

        if (accepted) printf("∴ String accepted.\n");
        else printf("∴ String Rejected.\n");

        free(current_active);
        printf("\nContinue? (1=Yes, 0=No): ");
        int choice;
        scanf("%d", &choice);
        if (!choice) break;
    }

    for (int i = 0; i < num_states; i++) {
        for (int j = 0; j < num_symbols; j++) {
            if (table[i][j].targets) free(table[i][j].targets);
        }
        free(table[i]);
    }
    free(table);
    free(finals);

    return 0;
}