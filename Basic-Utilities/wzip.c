#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "wzip: file1 [file2 ...]\n");
        return 1;
    }
    
    int current_char = -1;  
    int count = 0;         
    
    for (int i = 1; i < argc; i++) {
        FILE *file = fopen(argv[i], "r");
        if (file == NULL) {
            fprintf(stderr, "wzip: cannot open file '%s'\n", argv[i]);
            return 1;
        }
        
        int c;
        while ((c = fgetc(file)) != EOF) {
            if (c == current_char) {
                
                count++;
            } else {
                if (current_char != -1) {
                    fwrite(&count, sizeof(int), 1, stdout);
                    fwrite(&current_char, sizeof(char), 1, stdout);
                }
    
                current_char = c;
                count = 1;
            }
        }
        
        fclose(file);
    }
    
    if (current_char != -1) {
        fwrite(&count, sizeof(int), 1, stdout);
        fwrite(&current_char, sizeof(char), 1, stdout);
    }
    
    return 0;
}
