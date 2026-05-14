#include <stdio.h>
// #include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

#define MAX_TRANSITIONS 100
#define MAX_OUTCOMES 10
#define MAX_STACK 1000
#define MAX_PATHLENGTH 2000

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

typedef struct {
    int state;
    char input[100];
    char stack[MAX_STACK];
} ID;

Transition transitions[MAX_TRANSITIONS];
int num_transitions = 0;
int accept_states[20];
int num_accept_states = 0;

ID current_path[MAX_PATHLENGTH];
ID saved_path[MAX_PATHLENGTH];
int saved_path_len = 0;
bool path_saved = false;

bool is_accept_state(int state) {
    for (int i = 0; i < num_accept_states; i++) {
        if (accept_states[i] == state) return true;
    }
    return false;
}

bool simulate(int state, const char* remaining_input, const char* stack, int depth) {
    // Record current state in path
    current_path[depth].state = state;
    strcpy(current_path[depth].input, remaining_input);
    strcpy(current_path[depth].stack, stack);

    // Acceptance by final state when input is exhausted
    if (remaining_input[0] == '\0' && is_accept_state(state)) {
        // If accepted, overwrite the saved path with the successful path
        for (int i = 0; i <= depth; i++) {
            saved_path[i] = current_path[i];
        }
        saved_path_len = depth + 1;
        return true;
    }

    bool moved = false;
    char current_input = remaining_input[0];
    char current_stack_top = stack[0];

    if (current_stack_top != '\0') {
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

                    moved = true;
                    if (simulate(t.outcomes[j].next_state, remaining_input + 1, new_stack, depth + 1)) {
                        return true;
                    }
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

                    moved = true;
                    if (simulate(t.outcomes[j].next_state, remaining_input, new_stack, depth + 1)) {
                        return true;
                    }
                }
            }
        }
    }

    // If we couldn't make any more moves and haven't saved a rejected path yet, save this one.
    if (!moved && !path_saved) {
        for (int i = 0; i <= depth; i++) {
            saved_path[i] = current_path[i];
        }
        saved_path_len = depth + 1;
        path_saved = true;
    }

    return false;
}

void print_saved_path() {
    if (saved_path_len == 0) return;

    // Calculate display width of the first ID for alignment
    char state_str[20];
    sprintf(state_str, "%d", saved_path[0].state);
    int input_len = saved_path[0].input[0] ? strlen(saved_path[0].input) : 1; // 1 for λ
    int stack_len = saved_path[0].stack[0] ? strlen(saved_path[0].stack) : 1; // 1 for λ

    // "(q" + state + ", " + input + ", " + stack + ") "
    int indent_spaces = 2 + strlen(state_str) + 2 + input_len + 2 + stack_len + 2;

    printf("(q%d, %s, %s) ",
           saved_path[0].state,
           saved_path[0].input[0] ? saved_path[0].input : "\xCE\xBB",
           saved_path[0].stack[0] ? saved_path[0].stack : "\xCE\xBB");

    if (saved_path_len > 1) {
        printf("\xE2\x8A\xA2 (q%d, %s, %s)\n",
               saved_path[1].state,
               saved_path[1].input[0] ? saved_path[1].input : "\xCE\xBB",
               saved_path[1].stack[0] ? saved_path[1].stack : "\xCE\xBB");

        for (int i = 2; i < saved_path_len; i++) {
            for (int s = 0; s < indent_spaces; s++) printf(" ");
            printf("\xE2\x8A\xA2 (q%d, %s, %s)\n",
                   saved_path[i].state,
                   saved_path[i].input[0] ? saved_path[i].input : "\xCE\xBB",
                   saved_path[i].stack[0] ? saved_path[i].stack : "\xCE\xBB");
        }
    } else {
        printf("\n");
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

        path_saved = false;
        saved_path_len = 0;

        bool is_accepted = simulate(start_state, input, stack, 0);

        printf("\nInstantaneous Description (Trace):\n");
        print_saved_path();

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