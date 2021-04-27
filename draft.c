#include <stdio.h>

int main(){
    char f = 'c';
    printf("%c\n", f);
    char* format = "cdf";
    printf("%d\n", &format);
    printf("%c\n", *format);
    printf("%c\n", *format + 1);
};

