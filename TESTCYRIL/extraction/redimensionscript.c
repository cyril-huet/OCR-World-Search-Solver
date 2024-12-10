#include <stdlib.h>

int main() {
    system("for FILE in images/*; do ./resizeautre \"$FILE\" \"$FILE\"; done");
    return 0;
}

