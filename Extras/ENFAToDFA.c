#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

#define EPSILON 'e'

// Represents a set of NFA states (used as a single DFA state)
typedef struct {
    bool* nfa_states;
} StateSet;

int num_enfa_states = 0;
int num_symbols = 0;
char* alphabet = NULL;
bool* accepting_states_mask = NULL;

// enfa_trans[from_state][symbol][to_state] = true/false
bool*** enfa_trans = NULL;

// Create an empty state set
StateSet create_state_set() {
    StateSet s;
    s.nfa_states = (bool*)calloc(num_enfa_states, sizeof(bool));
    if (!s.nfa_states) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    return s;
}

// Compare two state sets
bool states_equal(StateSet a, StateSet b) {
    for (int i = 0; i < num_enfa_states; i++) {
        if (a.nfa_states[i] != b.nfa_states[i]) {
            return false;
        }
    }
    return true;
}

// Check if a state set is empty
bool is_empty_set(StateSet s) {
    for (int i = 0; i < num_enfa_states; i++) {
        if (s.nfa_states[i]) return false;
    }
    return true;
}

// Compute Epsilon Closure of a given set of states
StateSet epsilon_closure(StateSet s) {
    StateSet closure = create_state_set();
    for (int i = 0; i < num_enfa_states; i++) {
        closure.nfa_states[i] = s.nfa_states[i];
    }
    
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < num_enfa_states; i++) {
            if (closure.nfa_states[i]) {
                // Check all epsilon transitions from state i
                for (int j = 0; j < num_enfa_states; j++) {
                    if (enfa_trans[i][(unsigned char)EPSILON][j] && !closure.nfa_states[j]) {
                        closure.nfa_states[j] = true;
                        changed = true;
                    }
                }
            }
        }
    }
    return closure;
}

// Compute the reachable states from a set of states on a given symbol
StateSet move(StateSet s, char symbol) {
    StateSet result = create_state_set();
    for (int i = 0; i < num_enfa_states; i++) {
        if (s.nfa_states[i]) {
            for (int j = 0; j < num_enfa_states; j++) {
                if (enfa_trans[i][(unsigned char)symbol][j]) {
                    result.nfa_states[j] = true;
                }
            }
        }
    }
    return result;
}

// Helper to print a state set like {q0, q1, q2}
void print_state_set(StateSet s) {
    if (is_empty_set(s)) {
        printf("{}");
        return;
    }
    printf("{");
    bool first = true;
    for (int i = 0; i < num_enfa_states; i++) {
        if (s.nfa_states[i]) {
            if (!first) printf(", ");
            printf("q%d", i);
            first = false;
        }
    }
    printf("}");
}

// Check if a state set is accepting
bool is_accepting(StateSet s) {
    for (int i = 0; i < num_enfa_states; i++) {
        if (s.nfa_states[i] && accepting_states_mask[i]) {
            return true;
        }
    }
    return false;
}

// Find if a state set already exists in our DFA states list
int find_dfa_state(StateSet* dfa_states, int num_dfa_states, StateSet target) {
    for (int i = 0; i < num_dfa_states; i++) {
        if (states_equal(dfa_states[i], target)) {
            return i;
        }
    }
    return -1;
}

// Function to read a line dynamically
char* read_string_dynamically() {
    size_t capacity = 32;
    char* buffer = (char*)malloc(capacity * sizeof(char));
    if (!buffer) exit(1);
    
    size_t length = 0;
    int c;
    
    while ((c = getchar()) == ' ' || c == '\n' || c == '\t'); // Skip leading whitespaces
    if (c != EOF) buffer[length++] = (char)c;

    while ((c = getchar()) != '\n' && c != ' ' && c != EOF) {
        buffer[length++] = (char)c;
        if (length == capacity) {
            capacity *= 2;
            buffer = (char*)realloc(buffer, capacity);
            if (!buffer) exit(1);
        }
    }
    buffer[length] = '\0';
    return buffer;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    
    printf("--- E-NFA to DFA Converter ---\n");
    printf("Enter number of E-NFA states: ");
    if (scanf("%d", &num_enfa_states) != 1 || num_enfa_states <= 0) {
        printf("Invalid number of states.\n");
        return 1;
    }
    
    // Allocate global structures
    accepting_states_mask = (bool*)calloc(num_enfa_states, sizeof(bool));
    enfa_trans = (bool***)malloc(num_enfa_states * sizeof(bool**));
    for (int i = 0; i < num_enfa_states; i++) {
        enfa_trans[i] = (bool**)malloc(256 * sizeof(bool*));
        for (int j = 0; j < 256; j++) {
            enfa_trans[i][j] = (bool*)calloc(num_enfa_states, sizeof(bool));
        }
    }
    
    printf("Enter the alphabet characters excluding epsilon (e.g., ab): ");
    alphabet = read_string_dynamically();
    num_symbols = strlen(alphabet);
    
    int num_transitions;
    printf("Enter number of transitions: ");
    scanf("%d", &num_transitions);
    
    printf("Enter transitions (format: from_state symbol to_state) [use 'e' or 'ε' for epsilon]:\n");
    for (int i = 0; i < num_transitions; i++) {
        int u, v;
        char c[10];
        scanf("%d %s %d", &u, c, &v);
        if (u >= 0 && u < num_enfa_states && v >= 0 && v < num_enfa_states) {
            char sym = (strcmp(c, "ε") == 0) ? EPSILON : c[0];
            enfa_trans[u][(unsigned char)sym][v] = true;
        }
    }
    
    int num_acc;
    printf("Enter number of accepting states: ");
    scanf("%d", &num_acc);
    if (num_acc > 0) {
        printf("Enter the accepting states (space separated): ");
        for (int i = 0; i < num_acc; i++) {
            int acc_state;
            scanf("%d", &acc_state);
            if (acc_state >= 0 && acc_state < num_enfa_states) {
                accepting_states_mask[acc_state] = true;
            }
        }
    }

    // Dynamic Subset Construction Algorithm
    int dfa_capacity = 16;
    StateSet* dfa_states = (StateSet*)malloc(dfa_capacity * sizeof(StateSet));
    int** dfa_transitions = (int**)malloc(dfa_capacity * sizeof(int*));
    for (int i = 0; i < dfa_capacity; i++) {
        dfa_transitions[i] = (int*)calloc(256, sizeof(int));
    }
    
    int num_dfa_states = 0;
    
    // Initial DFA state is the Epsilon Closure of the E-NFA start state (q0)
    StateSet start_initial = create_state_set();
    start_initial.nfa_states[0] = true;
    StateSet start_state = epsilon_closure(start_initial);
    free(start_initial.nfa_states);
    
    dfa_states[num_dfa_states++] = start_state;
    
    int unmarked_idx = 0;
    while (unmarked_idx < num_dfa_states) {
        int current_idx = unmarked_idx++;
        StateSet current_set = dfa_states[current_idx];
        
        for (int i = 0; i < num_symbols; i++) {
            char sym = alphabet[i];
            
            // Move and Closure
            StateSet move_set = move(current_set, sym);
            StateSet new_state = epsilon_closure(move_set);
            free(move_set.nfa_states);
            
            if (is_empty_set(new_state)) {
                free(new_state.nfa_states);
                dfa_transitions[current_idx][(unsigned char)sym] = -1; // -1 represents dead state
                continue; 
            }
            
            int dest_idx = find_dfa_state(dfa_states, num_dfa_states, new_state);
            if (dest_idx == -1) {
                // Resize if needed
                if (num_dfa_states == dfa_capacity) {
                    int old_capacity = dfa_capacity;
                    dfa_capacity *= 2;
                    dfa_states = (StateSet*)realloc(dfa_states, dfa_capacity * sizeof(StateSet));
                    dfa_transitions = (int**)realloc(dfa_transitions, dfa_capacity * sizeof(int*));
                    for (int k = old_capacity; k < dfa_capacity; k++) {
                        dfa_transitions[k] = (int*)calloc(256, sizeof(int));
                    }
                }
                dest_idx = num_dfa_states;
                dfa_states[num_dfa_states++] = new_state;
            } else {
                free(new_state.nfa_states);
            }
            
            // Record transition
            dfa_transitions[current_idx][(unsigned char)sym] = dest_idx;
        }
    }
    
    // Output the resulting DFA
    printf("\n--- Resulting DFA ---\n");
    printf("DFA States:\n");
    for (int i = 0; i < num_dfa_states; i++) {
        printf("S%d = ", i);
        print_state_set(dfa_states[i]);
        if (i == 0) printf(" [Start]");
        if (is_accepting(dfa_states[i])) printf(" [Accept]");
        printf("\n");
    }
    
    printf("\nTransitions:\n");
    for (int i = 0; i < num_dfa_states; i++) {
        for (int j = 0; j < num_symbols; j++) {
            char sym = alphabet[j];
            int dest = dfa_transitions[i][(unsigned char)sym];
            if (dest != -1) {
                printf("S%d --%c--> S%d\n", i, sym, dest);
            }
        }
    }
    
    // Cleanup
    free(alphabet);
    free(accepting_states_mask);
    for (int i = 0; i < num_enfa_states; i++) {
        for (int j = 0; j < 256; j++) free(enfa_trans[i][j]);
        free(enfa_trans[i]);
    }
    free(enfa_trans);
    
    for (int i = 0; i < num_dfa_states; i++) {
        free(dfa_states[i].nfa_states);
    }
    free(dfa_states);
    for (int i = 0; i < dfa_capacity; i++) {
        free(dfa_transitions[i]);
    }
    free(dfa_transitions);

    return 0;
}