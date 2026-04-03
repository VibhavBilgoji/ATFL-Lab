#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef struct {
    int next_state;
    char output;
} MealyTransition;

void print_nesting(int curr, char* remaining) {
    int len = strlen(remaining);
    printf("\t= ");
    if (len == 0) {
        printf("q%d", curr);
    } else {
        for (int i = 0; i < len; i++) printf("δ(");
        printf("q%d", curr);
        for (int i = 0; i < len; i++) printf(", %c)", remaining[i]);
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    int states, symbols, start_state_num;
    printf("Enter number of states: ");
    if (scanf("%d", &states) != 1) return 1;

    printf("Enter start state: q");
    scanf("%d", &start_state_num);

    printf("Enter number of input symbols: ");
    if (scanf("%d", &symbols) != 1) return 1;

    char* alphabet = (char*)malloc((symbols + 1) * sizeof(char));
    printf("Enter the %d alphabet symbols (e.g., 01): ", symbols);
    scanf("%s", alphabet);

    MealyTransition** table = (MealyTransition**)malloc(states * sizeof(MealyTransition*));
    for (int i = 0; i < states; i++) {
        table[i] = (MealyTransition*)malloc(symbols * sizeof(MealyTransition));
    }

    printf("\nEnter transitions and outputs:\n");
    for (int i = 0; i < states; i++) {
        for (int j = 0; j < symbols; j++) {
            printf("δ(q%d, %c) -> Next State: q", i, alphabet[j]);
            scanf("%d", &table[i][j].next_state);
            printf("λ(q%d, %c) -> Output Symbol: ", i, alphabet[j]);
            scanf(" %c", &table[i][j].output);
        }
    }

    while (true) {
        char input[100];
        char output_gen[100] = {0};
        char lambda_seq[1000] = {0};
        printf("\n--------------------------------\n");
        printf("Enter input string: ");
        scanf("%s", input);

        int curr = start_state_num;
        int len = strlen(input);
        int valid = 1;

        for (int i = 0; i < len; i++) {
            int sym_idx = -1;
            for (int k = 0; k < symbols; k++) {
                if (alphabet[k] == input[i]) {
                    sym_idx = k;
                    break;
                }
            }

            if (sym_idx == -1) {
                printf("\nError: Symbol '%c' not in alphabet\n", input[i]);
                valid = 0;
                break;
            }

            char buf[64];
            snprintf(buf, sizeof(buf), "λ(q%d, %c)", curr, input[i]);
            strncat(lambda_seq, buf, sizeof(lambda_seq) - strlen(lambda_seq) - 1);

            output_gen[i] = table[curr][sym_idx].output;
            output_gen[i+1] = '\0';
            curr = table[curr][sym_idx].next_state;
        }

        if (valid) {
            printf("= %s\n", lambda_seq);
            printf("= %s\n", output_gen);
        }

        printf("\nContinue? (1=Yes, 0=No): ");
        int choice;
        scanf("%d", &choice);
        if (!choice) break;
    }

    for (int i = 0; i < states; i++) {
        free(table[i]);
    }
    free(table);
    free(alphabet);

    return 0;
}