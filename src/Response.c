#include "Response.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

char *render_image(char *fileName)
{
	FILE *file = fopen(fileName, "rb"); 

	if (file == NULL)
	{
		printf("file does not exist %s", fileName);
		return NULL;
	}
	// else {
	// 	printf("%s does exist \n", fileName);

	// }

	// FILE *file = fopen(fileName, "rb");
	// fseek(file, 0, SEEK_END);
	// unsigned long fileLen = ftell(file);
	// char *file_data;
	// rewind(file);
	// file_data = malloc((fileLen) * sizeof(char));
	// if (file_data == NULL)
	// {
	// 	printf("Memory error");
	// 	exit(2);
	// }
	// int num_read = 0;
	// char s;
	// while ((num_read = fread(&s, 1, 1, file)))
	// {
	// 	strncat(file_data, &s, 1);
	// }
	int width, height, bpp;

	uint8_t *rgb_image = stbi_load(fileName, &width, &height, &bpp, 3);

	// stbi_image_free(rgb_image);

	fclose(file);
	return rgb_image;
}
