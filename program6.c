#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PRODS 50
#define MAX_LEN 256
#define MAX_DEPTH 20

typedef struct {
    char lhs;
    char rhs[MAX_LEN];
} Production;

int num_prods = 0;
Production P[MAX_PRODS];
char V[MAX_LEN];
char T[MAX_LEN];
char S;

bool is_var(char c) {
    for (int i = 0; V[i] != '\0'; i++) {
        if(V[i] == c) return true;
    }
    return false;
}

void replace_leftmost(const char* current, char var, const char* rhs, char* next) {
    int i = 0, j = 0;
    bool replaced = false;
    while (current[i] != '\0') {
        if (!replaced && current[i] == var) {
            replaced = true;
            if (strcmp(rhs, "e") != 0 && strcmp(rhs, "E") != 0) { // 'e' or 'E' as epsilon
                for (int k = 0; rhs[k] != '\0'; k++) {
                    next[j++] = rhs[k];
                }
            }
            i++;
        } else {
            next[j++] = current[i++];
        }
    }
    next[j] = '\0';
}

void replace_rightmost(const char* current, char var, const char* rhs, char* next) {
    int last_idx = -1;
    for (int i = 0; current[i] != '\0'; i++) {
        if (current[i] == var) {
            last_idx = i;
        }
    }

    int j = 0;
    for (int i = 0; current[i] != '\0'; i++) {
        if (i == last_idx) {
            if (strcmp(rhs, "e") != 0 && strcmp(rhs, "E") != 0) {
                for (int k = 0; rhs[k] != '\0'; k++) {
                    next[j++] = rhs[k];
                }
            }
        } else {
            next[j++] = current[i];
        }
    }
    next[j] = '\0';
}

bool solve_lmd(char* current, char* target, int depth, char path[][MAX_LEN], int* path_len) {
    if (depth > MAX_DEPTH) return false;

    bool has_var = false;
    char first_var = '\0';
    for (int i = 0; current[i] != '\0'; i++) {
        if (is_var(current[i])) {
            has_var = true;
            first_var = current[i];
            break;
        }
    }

    if (!has_var) {
        if (strcmp(current, target) == 0) {
            *path_len = depth;
            return true;
        }
        return false;
    }

    if (strlen(current) > strlen(target) + MAX_DEPTH) return false;

    for (int i = 0; i < num_prods; i++) {
        if (P[i].lhs == first_var) {
            char next[MAX_LEN];
            replace_leftmost(current, first_var, P[i].rhs, next);
            strcpy(path[depth + 1], next);
            if (solve_lmd(next, target, depth + 1, path, path_len)) {
                return true;
            }
        }
    }

    return false;
}

bool solve_rmd(char* current, char* target, int depth, char path[][MAX_LEN], int* path_len) {
    if (depth > MAX_DEPTH) return false;

    bool has_var = false;
    char last_var = '\0';
    for (int i = 0; current[i] != '\0'; i++) {
        if (is_var(current[i])) {
            has_var = true;
            last_var = current[i];
        }
    }

    if (!has_var) {
        if (strcmp(current, target) == 0) {
            *path_len = depth;
            return true;
        }
        return false;
    }

    if (strlen(current) > strlen(target) + MAX_DEPTH) return false;

    for (int i = 0; i < num_prods; i++) {
        if (P[i].lhs == last_var) {
            char next[MAX_LEN];
            replace_rightmost(current, last_var, P[i].rhs, next);
            strcpy(path[depth + 1], next);
            if (solve_rmd(next, target, depth + 1, path, path_len)) {
                return true;
            }
        }
    }

    return false;
}

int main() {
    char target[MAX_LEN];
    char lmd_path[MAX_DEPTH + 2][MAX_LEN];
    char rmd_path[MAX_DEPTH + 2][MAX_LEN];
    int lmd_len = 0, rmd_len = 0;

    printf("Enter Variables (V) without spaces (e.g., SAB): ");
    scanf("%s", V);

    printf("Enter Terminals (T) without spaces (e.g., ab): ");
    scanf("%s", T);

    printf("Enter Start Symbol (S): ");
    scanf(" %c", &S);

    printf("Enter number of productions (P): ");
    scanf("%d", &num_prods);

    printf("Enter productions in format A->aB or A->e (for epsilon):\n");
    for (int i = 0; i < num_prods; i++) {
        char prod_str[MAX_LEN];
        scanf("%s", prod_str);
        P[i].lhs = prod_str[0];
        strcpy(P[i].rhs, prod_str + 3); // Skip "A->"
    }

    printf("Enter string to test: ");
    scanf("%s", target);
    if (strcmp(target, "e") == 0 || strcmp(target, "E") == 0) {
        target[0] = '\0'; // Epsilon target
    }

    char start_str[2] = {S, '\0'};
    strcpy(lmd_path[0], start_str);
    strcpy(rmd_path[0], start_str);

    bool accepted = solve_lmd(start_str, target, 0, lmd_path, &lmd_len);

    if (accepted) {
        solve_rmd(start_str, target, 0, rmd_path, &rmd_len);

        printf("\nString is ACCEPTED!\n\n");

        printf("Leftmost Derivation:\n");
        if (lmd_len == 0) {
            // Only start symbol
            printf("%s\n", lmd_path[0][0] == '\0' ? "e" : lmd_path[0]);
        } else {
            // First line: full form
            const char* first_left = lmd_path[0][0] == '\0' ? "e" : lmd_path[0];
            const char* first_right = lmd_path[1][0] == '\0' ? "e" : lmd_path[1];
            printf("%s => %s\n", first_left, first_right);
            // Subsequent lines: aligned with leading spaces
            for (int i = 2; i <= lmd_len; i++) {
                const char* right = lmd_path[i][0] == '\0' ? "e" : lmd_path[i];
                printf("  => %s\n", right);
            }
        }
        printf("\n");

        printf("Rightmost Derivation:\n");
        if (rmd_len == 0) {
            printf("%s\n", rmd_path[0][0] == '\0' ? "e" : rmd_path[0]);
        } else {
            const char* first_left = rmd_path[0][0] == '\0' ? "e" : rmd_path[0];
            const char* first_right = rmd_path[1][0] == '\0' ? "e" : rmd_path[1];
            printf("%s => %s\n", first_left, first_right);
            for (int i = 2; i <= rmd_len; i++) {
                const char* right = rmd_path[i][0] == '\0' ? "e" : rmd_path[i];
                printf("  => %s\n", right);
            }
        }
        printf("\n");
    } else {
        printf("\nString is REJECTED!\n");
    }

    return 0;
}