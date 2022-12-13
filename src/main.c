#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "HTTP_Server.h"
#include "Routes.h"
#include "Response.h"
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include "mime.h"
#include <sys/file.h>
#include "file.h"

#define SERVER_ROOT "./static"

int send_response(int fd, char *header, char *content_type, void *body, int content_length){
	const int max_response_size = 262144;
	char response[max_response_size];
	int response_length;
	time_t t = time(NULL);
	struct tm *local_time = localtime(&t);
	char *timestamp = asctime(local_time);

	response_length = sprintf(response, "%s\r\nDate %s Connection: close\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", header, timestamp, content_type, content_length);
	memcpy(response + response_length, body, content_length);
	response_length += content_length;

	// Send it all!
	int rv = send(fd, response, response_length, 0);
	if (rv < 0)
	{
		perror("send");
	}

	return rv;
}

struct file_data *get_file(char *request_path)
{
	char filepath[4096];
	struct file_data *filedata;

	filedata = file_load(request_path);

	if (filedata == NULL)
	{
		// TODO: make this non-fatal - redirect to 404 page
		fprintf(stderr, "cannot find file :(\n");
		exit(3);
	}

	return filedata;
}

char *get_time()
{
	time_t rawtime;
	int hours, minutes, seconds;
	time(&rawtime);
	struct tm *timeinfo = localtime(&rawtime);
	hours = timeinfo->tm_hour;
	minutes = timeinfo->tm_min;
	seconds = timeinfo->tm_sec;
	char *formatted_time;
	asprintf(&formatted_time, "%02d:%02d:%02d", hours, minutes, seconds);
	return formatted_time;
}

struct thread_args
{
	int first;
	struct Route *second;
};

void * handle_request(void *arg)
{
	struct thread_args *args = (struct thread_args *)arg;
	int client_socket = args->first;
	struct Route *route = args->second;
	char *time;
	time = get_time();
	char client_msg[4096] = "";
	read(client_socket, client_msg, 4095);
	// printf("%s\n", client_msg);

	// parsing client socket header to get HTTP method, route
	char *method = "";
	char *urlRoute = "";

	char *client_http_header = strtok(client_msg, "\n");

	// printf("\n\n%s\n\n", client_http_header);

	char *header_token = strtok(client_http_header, " ");

	int header_parse_counter = 0;

	while (header_token != NULL)
	{

		switch (header_parse_counter)
		{
		case 0:
			method = header_token;
		case 1:
			urlRoute = header_token;
		}
		header_token = strtok(NULL, " ");
		header_parse_counter++;
	}

	printf("[%s] %s -> %s\n", method, time, urlRoute);
	free(time);

	char template[100] = "";
	char *mime_type;
	mime_type = mime_type_get(urlRoute);
	char *response_data;
	char response_length[100];
	int length = 0;
	char http_header[4096] = "HTTP/1.1 200 OK\r\n";
	if (strstr(urlRoute, "/static/") != NULL)
	{
		// copy the path of the requested file to the path to render

		char *sub = malloc(50);
		strncpy(sub, urlRoute + 1, 50);
		strcat(template, sub);
		struct file_data *f = get_file(sub);
		response_data = f->data;

		// printf("file data %s \n %s\n", f->size, f->data);
		sprintf(response_length, "Content-Length: %d\r\n", f->size);
		length = f->size;
		// response_length = f->size;
		send_response(client_socket, "HTTP/1.1 200 OK", mime_type, response_data, f->size);
		free(sub);
		file_free(f);
		close(client_socket);
		return NULL;
	}
	else
	{
		struct Route *destination = search(route, urlRoute);
		strcat(template, "templates/");

		if (destination == NULL)
		{
			strcat(template, "404.html");
		}
		else
		{
			strcat(template, destination->value);
		}
		struct file_data *data = get_file(template);
		response_data = render_static_file(template);
		sprintf(response_length, "Content-Length: %d\r\n", data->size);
		
	
		file_free(data);
		
	}

	strcat(http_header, response_length);
	strcat(http_header, "Connection: close\r\n");
	strcat(http_header, "Content-Type: ");
	strcat(http_header, mime_type);
	// strcat(http_header, response_length);
	strcat(http_header, "\r\n\r\n");
	strcat(http_header, response_data);
	// strcat(http_header, "\r\n\r\n");
	printf("RESPONSE: %s\n", http_header);

	send(client_socket, http_header, sizeof(http_header), 0);
	close(client_socket);
	return NULL;
}

int main(int argc, char **argv)
{
	// initiate HTTP_Server
	HTTP_Server http_server;
	int port;
	if (argc != 2)
	{
		printf("%s [port-number]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	port = atoi(argv[1]);
	init_server(&http_server, port);

	int client_socket;

	// registering Routes
	struct Route *route = initRoute("/", "index.html");
	addRoute(route, "/about", "about.html");

	printf("\n====================================\n");
	printf("=========AVAILABLE ROUTES========\n");
	// display all available routes
	inorder(route);

	// struct thread_pool *pool = pool_init(2);

	while (1)
	{
		client_socket = accept(http_server.socket, NULL, NULL);
		struct thread_args *args = malloc(sizeof(struct thread_args));
		args->first = client_socket;
		args->second = route;
		pthread_t thread_id;
		pthread_create(&thread_id, NULL, handle_request, args);
		// pthread_join(thread_id, NULL);
	}
	return 0;
}
