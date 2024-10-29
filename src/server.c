// src/server.c

#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define BUFFER_SIZE 4096

void *handle_client(void *arg);
void send_response(int client_socket, const char *status_code, const char *content_type, const char *file_path);

typedef struct {
    int client_socket;
    char document_root[256];
} client_data_t;

void start_server(int port, const char *document_root) {
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    while (1) {
        client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        // Log the new connection
        FILE *log_file = fopen("server.log", "a");
        if (log_file) {
            time_t now = time(NULL);
            char *time_str = ctime(&now);
            time_str[strlen(time_str) - 1] = '\0'; // Remove newline character
            fprintf(log_file, "[%s] New connection established\n", time_str);
            fclose(log_file);
        }

        client_data_t *client_data = malloc(sizeof(client_data_t));
        client_data->client_socket = client_socket;
        strncpy(client_data->document_root, document_root, sizeof(client_data->document_root));

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, client_data);
        pthread_detach(thread_id);
    }

    close(server_fd);
}

void *handle_client(void *arg) {
    client_data_t *client_data = (client_data_t *)arg;
    int client_socket = client_data->client_socket;
    char document_root[256];
    strncpy(document_root, client_data->document_root, sizeof(document_root));
    free(client_data);

    char buffer[BUFFER_SIZE];
    int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Failed to read from socket");
        close(client_socket);
        return NULL;
    }

    buffer[bytes_read] = '\0';

    char method[8], file_path[256];
    sscanf(buffer, "%s %s", method, file_path);

    // Log the request
    FILE *log_file = fopen("server.log", "a");
    if (log_file) {
        time_t now = time(NULL);
        char *time_str = ctime(&now);
        time_str[strlen(time_str) - 1] = '\0'; // Remove newline character
        fprintf(log_file, "[%s] Request: Method=%s, Path=%s\n", time_str, method, file_path);
        fclose(log_file);
    }

    // Handle video URLs (external links)
    if (strstr(file_path, "http://") == file_path || strstr(file_path, "https://") == file_path) {
        // Allow video streaming from external links
        send_response(client_socket, "200 OK", "video/mp4", file_path);
    } else {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s%s", document_root, strcmp(file_path, "/") == 0 ? "/index.html" : file_path);

        struct stat file_stat;
        const char *status_code = "200 OK";
        char content_type[50] = "text/html";

        // Determine the response code and content type
        if (strcmp(method, "GET") != 0) {
            status_code = "400 Bad Request";
            snprintf(full_path, sizeof(full_path), "%s/400.html", document_root);
        } else if (stat(full_path, &file_stat) < 0) {
            status_code = "404 Not Found";
            snprintf(full_path, sizeof(full_path), "%s/404.html", document_root);
        } else if (!(file_stat.st_mode & S_IROTH)) {
            status_code = "403 Forbidden";
            snprintf(full_path, sizeof(full_path), "%s/403.html", document_root);
        } else {
            // Set content type based on file extension
            char *ext = strrchr(full_path, '.');
            if (ext) {
                if (strcmp(ext, ".html") == 0) {
                    strcpy(content_type, "text/html");
                } else if (strcmp(ext, ".txt") == 0) {
                    strcpy(content_type, "text/plain");
                } else if (strcmp(ext, ".jpg") == 0) {
                    strcpy(content_type, "image/jpeg");
                } else if (strcmp(ext, ".png") == 0) {
                    strcpy(content_type, "image/png");
                } else if (strcmp(ext, ".gif") == 0) {
                    strcpy(content_type, "image/gif");
                } else if (strcmp(ext, ".js") == 0) {
                    strcpy(content_type, "application/javascript");
                } else if (strcmp(ext, ".css") == 0) {
                    strcpy(content_type, "text/css");
                } else if (strcmp(ext, ".mp4") == 0) {
                    strcpy(content_type, "video/mp4");
                }
            }
        }

        // Log the file being served or the error
        log_file = fopen("server.log", "a");
        if (log_file) {
            time_t now = time(NULL);
            char *time_str = ctime(&now);
            time_str[strlen(time_str) - 1] = '\0'; // Remove newline character
            if (strcmp(status_code, "200 OK") == 0) {
                fprintf(log_file, "[%s] Serving file: %s\n", time_str, full_path);
            } else {
                fprintf(log_file, "[%s] Error: %s for path: %s\n", time_str, status_code, file_path);
            }
            fclose(log_file);
        }

        // Send the response
        send_response(client_socket, status_code, content_type, full_path);
    }

    // Log connection close
    log_file = fopen("server.log", "a");
    if (log_file) {
        time_t now = time(NULL);
        char *time_str = ctime(&now);
        time_str[strlen(time_str) - 1] = '\0'; // Remove newline character
        fprintf(log_file, "[%s] Connection closed for client\n", time_str);
        fclose(log_file);
    }

    close(client_socket);
    return NULL;
}

void send_response(int client_socket, const char *status_code, const char *content_type, const char *file_path) {
    char response[BUFFER_SIZE];
    char headers[BUFFER_SIZE];
    struct stat file_stat;

    // Generate the headers
    snprintf(headers, sizeof(headers),
             "HTTP/1.0 %s\r\n"
             "Content-Type: %s\r\n"
             "Connection: close\r\n",
             status_code, content_type);

    // Add Content-Length and Date headers if applicable
    if (stat(file_path, &file_stat) == 0) {
        snprintf(headers + strlen(headers), sizeof(headers) - strlen(headers),
                 "Content-Length: %lld\r\n", (long long)file_stat.st_size);
    }

    time_t now = time(NULL);
    struct tm *tm_info = gmtime(&now);
    char date_str[128];
    strftime(date_str, sizeof(date_str), "%a, %d %b %Y %H:%M:%S GMT", tm_info);
    snprintf(headers + strlen(headers), sizeof(headers) - strlen(headers),
             "Date: %s\r\n\r\n", date_str);

    // Log headers being sent
    FILE *log_file = fopen("server.log", "a");
    if (log_file) {
        time_t now = time(NULL);
        char *time_str = ctime(&now);
        time_str[strlen(time_str) - 1] = '\0'; // Remove newline character
        fprintf(log_file, "[%s] Sending headers:\n%s\n", time_str, headers);
        fclose(log_file);
    }

    // Send headers to the client
    write(client_socket, headers, strlen(headers));

    // Send the content if there is a file to send
    if (file_path) {
        int file_fd = open(file_path, O_RDONLY);
        if (file_fd != -1) {
            int bytes_read;
            while ((bytes_read = read(file_fd, response, sizeof(response))) > 0) {
                write(client_socket, response, bytes_read);
            }
            close(file_fd);
        }
    }
}