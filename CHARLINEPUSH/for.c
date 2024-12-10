#include <stdlib.h>

int main() {
    system("for file in images/*; do "
           "if [[ $(basename \"$file\") =~ ^[a-zA-Z] ]]; then "
           "{ temp_file=\"${file%.pgm}_temp.pgm\"; "
           "magick convert \"$file\" \"$temp_file\" 2>/dev/null && "
           "mv \"$temp_file\" \"$file\"; }; "
           "fi; "
           "done");
    return 0;
}

