#include <stdlib.h>

void remove_duplicates() {
    system("for file in images/*\\ *; do "
           "if [ -f \"$file\" ]; then "
           "rm \"$file\"; "
           "fi; "
           "done");
}

int main() {
    remove_duplicates();
    return 0;
}

