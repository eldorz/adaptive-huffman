#define __STDC_WANT_LIB_EXT2__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

void char_to_binstring(char c, char *binstring) {
    int next = 0;
    for (int i = 7; i >= 0; --i) {
        next = c & 00000001;
        binstring[i] = 48 + next; // 48 is ASCII '0'
        c = c >> 1;
    }
    binstring[8] = 0;             // null terminate
}


int main(int argc, char **argv) {
	int sflag = 0;
	if (argc == 2 && strcmp(argv[1], "-s") == 0) {
	    sflag = 1;
	}
	else if (argc != 1) {
	    fprintf(stderr, "Usage: ahencode [-s]\n");
	    return 1;
	}
	
	char *nextline = NULL;
	size_t size = 0;
	ssize_t byteCount;
	char binstring[9] = "";
	while ((byteCount = getline(&nextline, &size, stdin)) != -1) {
            for (int i = 0; i < byteCount - 1; ++i) {
              char nextchar = nextline[i];
              char_to_binstring(nextchar, binstring);
	      printf("%s", binstring);
              if (sflag == 1 && i != byteCount - 2) printf(" ");
            }
            printf("\n");
	}

        free(nextline);	
	return 0;
}
