#include  <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1024

void find_line(FILE* f, char* target){
  char* line = NULL;
  size_t len = 0;
  ssize_t nread;

  while((nread = getline(&line, &len, f)) != -1){
    if(strstr(line, target)){
      printf("%s", line);
    }
  }

  free(line);
}


int main(int argc, char* argv[argc+1]){
    if(argc == 1){
      printf("wgrep: searchterm [file ...]\n");
      exit(1);
    }

    if(argc == 2){
      int max_line = 1024;
      char line[max_line];
      

      while(fgets(line, sizeof(line), stdin)){
	if(strstr(line, argv[1])){
	  printf("%s", line);
	}
      }
      exit(0);
    }

    for(int i=2; i < argc; i++){
      FILE* f = fopen(argv[i], "r");
      if(!f){
	printf("wgrep: cannot open file\n");
	exit(1);
      }

      find_line(f, argv[1]);
      fclose(f);
    }
    exit(0);
}
