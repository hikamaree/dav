#include "util.h"

char* get_name(char* name, int max_lenght, int words_number) {
	char *x = (char*) malloc(max_lenght * sizeof(char));
	int i, s = 0;
	for(i = 0; i < max_lenght - 1; i++) {
		if(name[i] == ' ') {
			s++;
			if(s == words_number) {
				break;
			}
		}
		if(name[i] == '\0') {
			break;
		}
		x[i] = name[i];
	}
	x[i] = '\0';
	return x;
}
