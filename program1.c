#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

bool isNumber(char* str){
    int i = 0;
    bool hasDecimal = false;
    if (str[0] == '\0') return false;
    if (str[0] == '-') {
        i = 1;
        if (str[i] == '\0') return false;
    }
    for (; str[i] != '\0'; i++) {
        if (str[i] == '.') {
            if (hasDecimal) return false;
            hasDecimal = true;
        }
        else if (!isdigit(str[i])) return false;
    }
    return true;
}

bool isKeyword(char* str){
    char keywords[][15] = {
        "auto", "break", "case", "char", "const", "continue", "default",
        "do", "double", "else", "enum", "extern", "float", "for", "while",
        "goto", "if", "inline", "int", "long", "register", "restrict",
        "return", "short", "signed", "sizeof", "static", "struct", "switch",
        "typedef", "union", "unsigned", "void", "volatile",  "_Alignas",
        "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic", "_Noreturn", "_Static_assert",
        "_Thread_local", "alignas", "alignof", "bool", "constexpr", "false", "true", "nullptr"
    };
    int len = sizeof(keywords) / sizeof(keywords[0]);
    for(int i = 0; i < len; i++) if(!strcmp(str, keywords[i])) return true;
    return false;
}

bool isIdentifier(char* str){
    if((isalpha(str[0]) || str[0] == '_') && !isKeyword(str)){
        for(int i = 1; str[i] != '\0'; i++) if(!isalnum(str[i]) && str[i] != '_') return false;
        return true;
    }
    return false;
}

int main(){
    while(true){
        char input[100];
        printf("Enter a string (Press Ctrl+C to exit): ");
        if (fgets(input, 100, stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;

        if(isKeyword(input)) printf("Entered string is a C keyword\n\n");
        else if(isNumber(input)) printf("Entered string is a number\n\n");
        else printf("Entered string is a %svalid identifier\n\n", (isIdentifier(input)) ? "" : "in");
    }
    return 0;
}