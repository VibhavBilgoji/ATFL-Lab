#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define EPSILON 'e'

// Structure for an NFA state
typedef struct State {
    int id;
    char transition_symbol;
    struct State* next1;
    struct State* next2;
} State;

// Structure for an NFA fragment
typedef struct NFA {
    State* start;
    State* end;
} NFA;

int state_counter = 0;

// Function to create a new state
State* create_state() {
    State* new_state = (State*)malloc(sizeof(State));
    if (!new_state) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    new_state->id = state_counter++;
    new_state->transition_symbol = '\0';
    new_state->next1 = NULL;
    new_state->next2 = NULL;
    return new_state;
}

// Create a basic NFA for a single character
NFA create_basic_nfa(char symbol) {
    NFA nfa;
    State* s1 = create_state();
    State* s2 = create_state();

    s1->transition_symbol = symbol;
    s1->next1 = s2;

    nfa.start = s1;
    nfa.end = s2;
    return nfa;
}

// Concatenate two NFAs (A . B)
NFA concat_nfa(NFA a, NFA b) {
    NFA nfa;
    a.end->transition_symbol = EPSILON;
    a.end->next1 = b.start;

    nfa.start = a.start;
    nfa.end = b.end;
    return nfa;
}

// Union of two NFAs (A | B)
NFA union_nfa(NFA a, NFA b) {
    NFA nfa;
    State* start_state = create_state();
    State* end_state = create_state();

    start_state->transition_symbol = EPSILON;
    start_state->next1 = a.start;
    start_state->next2 = b.start;

    a.end->transition_symbol = EPSILON;
    a.end->next1 = end_state;

    b.end->transition_symbol = EPSILON;
    b.end->next1 = end_state;

    nfa.start = start_state;
    nfa.end = end_state;
    return nfa;
}

// Kleene Star of an NFA (A*)
NFA star_nfa(NFA a) {
    NFA nfa;
    State* start_state = create_state();
    State* end_state = create_state();

    start_state->transition_symbol = EPSILON;
    start_state->next1 = a.start;
    start_state->next2 = end_state;

    a.end->transition_symbol = EPSILON;
    a.end->next1 = a.start;
    a.end->next2 = end_state;

    nfa.start = start_state;
    nfa.end = end_state;
    return nfa;
}

// Precedence function
int precedence(char c) {
    switch (c) {
        case '*': return 3;
        case '.': return 2;
        case '|': return 1;
        default: return 0;
    }
}

// Insert explicit concatenation operators
void insert_concat(const char* regex, char* res) {
    int i, j = 0;
    int len = strlen(regex);
    for (i = 0; i < len; i++) {
        res[j++] = regex[i];
        if (i + 1 < len) {
            char c1 = regex[i];
            char c2 = regex[i + 1];
            if ((c1 != '(' && c1 != '|' && c1 != '.') &&
                (c2 != ')' && c2 != '|' && c2 != '*' && c2 != '.')) {
                res[j++] = '.';
            }
        }
    }
    res[j] = '\0';
}

// Convert infix regex to postfix regex
void infix_to_postfix(const char* infix, char* postfix) {
    int len = strlen(infix);
    char* stack = (char*)malloc(len * sizeof(char));
    if (!stack) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    int top = -1;
    int i, j = 0;

    for (i = 0; infix[i] != '\0'; i++) {
        char c = infix[i];

        if (c == '(') {
            stack[++top] = c;
        } else if (c == ')') {
            while (top != -1 && stack[top] != '(') {
                postfix[j++] = stack[top--];
            }
            if (top != -1) top--; // pop '('
        } else if (c == '*' || c == '.' || c == '|') {
            while (top != -1 && stack[top] != '(' && precedence(stack[top]) >= precedence(c)) {
                postfix[j++] = stack[top--];
            }
            stack[++top] = c;
        } else {
            postfix[j++] = c; // operand
        }
    }

    while (top != -1) {
        postfix[j++] = stack[top--];
    }
    postfix[j] = '\0';

    free(stack);
}

// Evaluate postfix regex and build NFA
NFA postfix_to_nfa(const char* postfix) {
    int len = strlen(postfix);
    NFA* stack = (NFA*)malloc(len * sizeof(NFA));
    if (!stack) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    int top = -1;

    for (int i = 0; postfix[i] != '\0'; i++) {
        char c = postfix[i];

        if (c == '*') {
            NFA n = stack[top--];
            stack[++top] = star_nfa(n);
        } else if (c == '.') {
            NFA n2 = stack[top--];
            NFA n1 = stack[top--];
            stack[++top] = concat_nfa(n1, n2);
        } else if (c == '|') {
            NFA n2 = stack[top--];
            NFA n1 = stack[top--];
            stack[++top] = union_nfa(n1, n2);
        } else {
            stack[++top] = create_basic_nfa(c);
        }
    }

    NFA result = stack[top];
    free(stack);
    return result;
}

// Helper to print transitions recursively (DFS)
void print_transitions(State* s, int* visited) {
    if (s == NULL || visited[s->id]) return;
    visited[s->id] = 1;

    if (s->next1 != NULL) {
        char sym = s->transition_symbol ? s->transition_symbol : EPSILON;
        if (sym == EPSILON) {
            printf("q%d --ε--> q%d\n", s->id, s->next1->id);
        } else {
            printf("q%d --%c--> q%d\n", s->id, sym, s->next1->id);
        }
        print_transitions(s->next1, visited);
    }
    if (s->next2 != NULL) {
        printf("q%d --ε--> q%d\n", s->id, s->next2->id);
        print_transitions(s->next2, visited);
    }
}

// Function to read a line dynamically
char* read_regex_dynamically() {
    size_t capacity = 32;
    char* buffer = (char*)malloc(capacity * sizeof(char));
    if (!buffer) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    size_t length = 0;
    int c;

    // Skip leading whitespaces
    while ((c = getchar()) == ' ' || c == '\n' || c == '\t');

    if (c != EOF) {
        buffer[length++] = (char)c;
    }

    while ((c = getchar()) != '\n' && c != ' ' && c != EOF) {
        buffer[length++] = (char)c;
        if (length == capacity) {
            capacity *= 2;
            buffer = (char*)realloc(buffer, capacity);
            if (!buffer) {
                printf("Memory allocation failed!\n");
                exit(1);
            }
        }
    }
    buffer[length] = '\0';

    if (length == 0) {
        free(buffer);
        return NULL;
    }
    return buffer;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    printf("Enter a Regular Expression (use | for union, * for Kleene star): ");

    char* regex = read_regex_dynamically();
    if (!regex) {
        printf("Error reading input or empty input.\n");
        return 1;
    }

    int regex_len = strlen(regex);
    // Allocate space for explicit concatenation (max length is < 2 * regex_len)
    char* regex_concat = (char*)malloc((regex_len * 2 + 1) * sizeof(char));
    char* postfix = (char*)malloc((regex_len * 2 + 1) * sizeof(char));

    if (!regex_concat || !postfix) {
        printf("Memory allocation failed!\n");
        free(regex);
        return 1;
    }

    insert_concat(regex, regex_concat);
    infix_to_postfix(regex_concat, postfix);

    printf("\nRegex with explicit concatenation: %s\n", regex_concat);
    printf("Postfix Expression: %s\n\n", postfix);

    NFA final_nfa = postfix_to_nfa(postfix);

    printf("εNFA Transitions:\n");
    printf("Start State: q%d\n", final_nfa.start->id);
    printf("Accept State: q%d\n\n", final_nfa.end->id);

    // Dynamically allocate visited array based on total states generated
    int* visited = (int*)calloc(state_counter, sizeof(int));
    if (!visited) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    print_transitions(final_nfa.start, visited);

    // Clean up memory
    free(regex);
    free(regex_concat);
    free(postfix);
    free(visited);

    return 0;
}