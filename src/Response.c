#include "Response.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

char * render_static_file(char * fileName) {
	FILE* file = fopen(fileName, "r");

	if (file == NULL) {
		printf("file does not exist %s", fileName);
		return NULL;
	}
	// else {
	// 	printf("%s does exist \n", fileName);
		
	// }

	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);
	printf("file %s\n", fileName);

	char* temp = malloc(sizeof(char) * (fsize+1));
	char ch;
	int i = 0;
	while((ch = fgetc(file)) != EOF) {
		temp[i] = ch;
		i++;
	}
	fclose(file);
	return temp;
}
