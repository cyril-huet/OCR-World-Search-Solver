#include <stdlib.h>

int main() {
    system("for file in *; do .././extraction_de_lettre  \"$file\" ; done");
    return 0;
}
