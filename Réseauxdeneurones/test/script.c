#include <stdlib.h>

int main() {
    system("for file in *; do convert \"$file\" \"${file%.png}.pgm\"; done");
    return 0;
}

