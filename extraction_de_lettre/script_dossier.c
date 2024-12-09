#include <stdlib.h>

int main() {
    system("i=1; for file in Jade; do ./test \"$file\" \"$i\"; i=$((i + 1)); done");
    return 0;
}

