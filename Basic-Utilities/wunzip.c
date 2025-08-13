#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "wunzip: file1 [file2 ...]\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "rb");
        if (!fp) {
            fprintf(stderr, "wunzip: cannot open file\n");
            return 1;
        }

        int32_t count;
        unsigned char ch;

        while (fread(&count, sizeof(int32_t), 1, fp) == 1 &&
               fread(&ch, sizeof(unsigned char), 1, fp) == 1) {
            for (int32_t j = 0; j < count; j++) {
                putchar(ch);
            }
        }

        fclose(fp);
    }

    return 0;
}
