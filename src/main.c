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
	if (strstr(urlRoute, "/static/") != NULL)
	{
		// strcat(template, urlRoute+1);
		strcat(template, "static/index.css");
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
	}

	char *response_data = render_static_file(template);

	char http_header[4096] = "HTTP/1.1 200 OK\r\n\r\n";

	strcat(http_header, response_data);
	strcat(http_header, "\r\n\r\n");

	send(client_socket, http_header, sizeof(http_header), 0);
	close(client_socket);
	free(response_data);
	return NULL;
}

int main()
{
	// initiate HTTP_Server
	HTTP_Server http_server;
	init_server(&http_server, 6969);

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
