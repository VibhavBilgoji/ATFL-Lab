#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdbool.h>

typedef struct {
    int* targets;
    int count;
} Transition;

void print_state_set(bool* states, int num_states) {
    printf("{");
    bool first = true;
    for (int i = 0; i < num_states; i++) {
        if (states[i]) {
            if (!first) printf(", ");
            printf("q%d", i);
            first = false;
        }
    }
    if (first) printf("∅");
    printf("}");
}

void print_step(bool* current_active, int num_states, char* remaining) {
    int len = strlen(remaining);
    printf("\t= ");

    for (int i = 0; i < len; i++) printf("δ(");

    int active_count = 0;
    for (int i = 0; i < num_states; i++) if (current_active[i]) active_count++;

    if (active_count > 1 && len > 0) {
        bool first = true;
        for (int i = 0; i < num_states; i++) {
            if (current_active[i]) {
                if (!first) printf(" ∪ ");
                printf("δ(q%d, %c)", i, remaining[0]);
                first = false;
            }
        }
    } else {
        print_state_set(current_active, num_states);
        if (len > 0) printf(", %c)", remaining[0]);
    }

    for (int i = 1; i < len; i++) printf(", %c)", remaining[i]);
    printf("\n");
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

        printf("\nSteps:\n");
        printf("δ*({q0}, %s)\n", input);

        int input_len = strlen(input);
        for (int i = 0; i <= input_len; i++) {
            print_step(current_active, num_states, input + i);

            if (i < input_len) {
                int symbol_idx = input[i] - 'a';
                bool* next_active = (bool*)calloc(num_states, sizeof(bool));
                for (int s = 0; s < num_states; s++) {
                    if (current_active[s]) {
                        Transition trans = table[s][symbol_idx];
                        for (int t = 0; t < trans.count; t++)
                            next_active[trans.targets[t]] = true;
                    }
                }
                free(current_active);
                current_active = next_active;
            }
        }

        bool accepted = false;
        for (int i = 0; i < num_states; i++) {
            if (current_active[i]) {
                for (int j = 0; j < fin_count; j++) {
                    if (i == finals[j]) { accepted = true; break; }
                }
            }
        }

        if (accepted) printf("\n : Accepted\n");
        else printf("\n : Rejected\n");

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