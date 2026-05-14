#include <stdio.h>
// #include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

#define MAX_TRANSITIONS 100
#define MAX_OUTCOMES 10
#define MAX_STACK 1000

typedef struct {
    int next_state;
    char push_str[20];
} Outcome;

typedef struct {
    int state;
    char input_sym;
    char stack_sym;
    int num_outcomes;
    Outcome outcomes[MAX_OUTCOMES];
} Transition;

Transition transitions[MAX_TRANSITIONS];
int num_transitions = 0;
int accept_states[20];
int num_accept_states = 0;
bool is_accepted = false;

bool is_accept_state(int state) {
    for (int i = 0; i < num_accept_states; i++) {
        if (accept_states[i] == state) return true;
    }
    return false;
}

void print_id(int state, const char* remaining_input, const char* stack, int depth) {
    for (int i = 0; i < depth; i++) printf("  ");
    printf("\xE2\x8A\xA2 (q%d, %s, %s)\n", state,
           remaining_input[0] ? remaining_input : "\xCE\xBB",
           stack[0] ? stack : "\xCE\xBB");
}

void simulate(int state, const char* remaining_input, const char* stack, int depth) {
    if (is_accepted) return; // Stop if already accepted

    print_id(state, remaining_input, stack, depth);

    // Acceptance by final state when input is exhausted
    if (remaining_input[0] == '\0' && is_accept_state(state)) {
        is_accepted = true;
        return;
    }

    // Acceptance by empty stack (optional, here we focus on final state as per the images)
    // if (remaining_input[0] == '\0' && stack[0] == '\0') { ... }

    char current_input = remaining_input[0];
    char current_stack_top = stack[0];

    for (int i = 0; i < num_transitions; i++) {
        Transition t = transitions[i];

        // Match state and stack top
        if (t.state != state || t.stack_sym != current_stack_top) continue;

        // Try consuming input symbol
        if (current_input != '\0' && t.input_sym == current_input) {
            for (int j = 0; j < t.num_outcomes; j++) {
                char new_stack[MAX_STACK] = {0};
                if (strcmp(t.outcomes[j].push_str, "e") != 0 && strcmp(t.outcomes[j].push_str, "E") != 0) {
                    strcpy(new_stack, t.outcomes[j].push_str);
                }
                strcat(new_stack, stack + 1);

                simulate(t.outcomes[j].next_state, remaining_input + 1, new_stack, depth + 1);
                if (is_accepted) return;
            }
        }

        // Try epsilon (lambda) transition
        if (t.input_sym == 'e' || t.input_sym == 'E') {
            for (int j = 0; j < t.num_outcomes; j++) {
                char new_stack[MAX_STACK] = {0};
                if (strcmp(t.outcomes[j].push_str, "e") != 0 && strcmp(t.outcomes[j].push_str, "E") != 0) {
                    strcpy(new_stack, t.outcomes[j].push_str);
                }
                strcat(new_stack, stack + 1);

                simulate(t.outcomes[j].next_state, remaining_input, new_stack, depth + 1);
                if (is_accepted) return;
            }
        }
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    int start_state;
    char start_stack_sym;

    printf("Enter number of transitions: ");
    if (scanf("%d", &num_transitions) != 1) return 1;

    printf("Enter start state (e.g., 0 for q0): ");
    scanf("%d", &start_state);

    printf("Enter start stack symbol: ");
    scanf(" %c", &start_stack_sym);

    printf("Enter number of final states: ");
    scanf("%d", &num_accept_states);
    if (num_accept_states > 0) {
        printf("Enter the final states (space-separated): ");
        for (int i = 0; i < num_accept_states; i++) {
            scanf("%d", &accept_states[i]);
        }
    }

    printf("\nEnter transitions. Use 'e' for \xCE\xBB (epsilon/empty string).\n");
    for (int i = 0; i < num_transitions; i++) {
        printf("\nTransition %d\n", i + 1);
        printf("Current State (e.g., 0): ");
        scanf("%d", &transitions[i].state);

        printf("Input Symbol (char or 'e'): ");
        scanf(" %c", &transitions[i].input_sym);

        printf("Stack Symbol to pop (char): ");
        scanf(" %c", &transitions[i].stack_sym);

        printf("How many outcomes for \xCE\xB4(q%d, %c, %c)? ", transitions[i].state, transitions[i].input_sym, transitions[i].stack_sym);
        scanf("%d", &transitions[i].num_outcomes);

        for (int j = 0; j < transitions[i].num_outcomes; j++) {
            printf("  Outcome %d - Next State: ", j + 1);
            scanf("%d", &transitions[i].outcomes[j].next_state);
            printf("  Outcome %d - String to push (or 'e'): ", j + 1);
            scanf("%s", transitions[i].outcomes[j].push_str);
        }
    }

    while (true) {
        char input[100];
        printf("\n--------------------------------\n");
        printf("Enter input string (or 'e' for empty string): ");
        scanf("%s", input);

        if (strcmp(input, "e") == 0 || strcmp(input, "E") == 0) {
            input[0] = '\0';
        }

        char stack[MAX_STACK] = {0};
        stack[0] = start_stack_sym;
        stack[1] = '\0';

        is_accepted = false;

        printf("\nInstantaneous Descriptions:\n");
        simulate(start_state, input, stack, 0);

        if (is_accepted) {
            printf("\nString Accepted!\n");
        } else {
            printf("\nString Rejected!\n");
        }

        printf("\nContinue? (1=Yes, 0=No): ");
        int choice;
        scanf("%d", &choice);
        if (!choice) break;
    }

    return 0;
}
