#include <stdio.h>
#include <stdlib.h>

void print_file (FILE* f){
  char buffer[256];
  
  while(fgets(buffer, sizeof(buffer), f)){
    printf("%s", buffer);
  }
}

int main(int argc, char* argv[argc+1]){
  if(argc == 1){
    return 0;
  }else{
    for(int i = 1; i < argc; i++){
      FILE *f = fopen(argv[i], "r");
      if(!f){
	fprintf(stderr, "Error, couldn't open file %s\n", argv[i]);
	exit(1);
      }
      print_file(f);
      fclose(f);
    }
  }
}
