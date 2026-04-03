#include <stdio.h>
#include <stdlib.h>

int transition(int curr, char* input, int** trans_table, int states, int symbols) {
    int new_curr = trans_table[curr][input[0]-'a'];
    if(input[1] == '\0') return new_curr;
    else return transition(new_curr, input+1, trans_table, states, symbols);
}

int main() {
    int states, symbols, fin_states;

    printf("Enter total number of states: ");
    if(scanf("%d", &states) != 1) return 1;

    printf("Enter alphabet size: ");
    if(scanf("%d", &symbols) != 1) return 1;

    int** trans_table = (int**)malloc(states * sizeof(int*));
    for(int i = 0; i < states; i++) {
        trans_table[i] = (int*)malloc(symbols * sizeof(int));
    }

    printf("\nEnter transitions (State 0 is Start):\n");
    for (int i = 0; i < states; i++) {
        for (int j = 0; j < symbols; j++) {
            printf("State %d on Symbol %c -> ", i, 'a' + j);
            scanf("%d", &trans_table[i][j]);
        }
    }

    printf("\nHow many final states? ");
    if(scanf("%d", &fin_states) != 1) return 1;
    int* final_states = (int*)malloc(fin_states * sizeof(int));

    printf("Enter final state indices: ");
    for (int i = 0; i < fin_states; i++) {
        scanf("%d", final_states + i);
    }

    while(true) {
        char input[100];
        printf("\nEnter string (letters a to %c): ", 'a' + symbols - 1);
        scanf("%s", input );

        int curr = 0;
        curr = transition(curr, input, trans_table, states, symbols);

        bool accepted = false;
        for (int i = 0; i < fin_states; i++) {
            if (curr == final_states[i]) {
                accepted = true;
                break;
            }
        }

        if (accepted) printf("\nRESULT: Accepted!\n");
        else printf("\nRESULT: Rejected (Ended in State %d)\n", curr);

        printf("Do you want to test another string? (1 for Yes / 0 for No): ");
        int choice;

        scanf("%d", &choice);
        if(choice == 0) {
            free(final_states);
            for(int i = 0; i < states; i++) free(trans_table[i]);
            free(trans_table);
            return 0;
        }
        if(choice == 1) continue;
        else {
            free(final_states);
            for(int i = 0; i < states; i++) free(trans_table + i);
            free(trans_table);
            printf("Invalid choice. Exiting.\n");
            return 1;
        }
    }
}
