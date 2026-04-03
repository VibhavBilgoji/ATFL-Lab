#include <stdio.h>
#include <string.h>

// Define states for clarity
// Row 0 (Even b): 0, 1, 2
// Row 1 (Odd b):  3, 4, 5
const int transition_table[6][2] = {
    // a    b
    {1, 3}, // State 0: (a%3=0, b%2=0)
    {2, 4}, // State 1: (a%3=1, b%2=0)
    {0, 5}, // State 2: (a%3=2, b%2=0)
    {4, 0}, // State 3: (a%3=0, b%2=1)
    {5, 1}, // State 4: (a%3=1, b%2=1) -> FINAL
    {3, 2}  // State 5: (a%3=2, b%2=1)
};

int main() {
    char input[100];
    int current_state = 0;
    const int final_state = 4;

    printf("Enter string (a/b only): ");
    scanf("%s", input);

    for (int i = 0; i < strlen(input); i++) {
        int symbol;
        if (input[i] == 'a') symbol = 0;
        else if (input[i] == 'b') symbol = 1;
        else {
            printf("Invalid character detected!\n");
            return 1;
        }

        current_state = transition_table[current_state][symbol];
    }

    if (current_state == final_state) {
        printf("String Accepted!\n");
    } else {
        printf("String Rejected.\n");
        printf("Ended in State %d (Condition not met)\n", current_state);
    }

    return 0;
}
