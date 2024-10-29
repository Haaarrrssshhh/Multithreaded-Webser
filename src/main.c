// src/main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "server.h"

#define DEFAULT_PORT 8888
#define DEFAULT_ROOT "./webserver_files"

int main(int argc, char *argv[]) {
    char *document_root = DEFAULT_ROOT;
    int port = DEFAULT_PORT;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-document_root") == 0 && i + 1 < argc) {
            document_root = argv[++i];
        } else if (strcmp(argv[i], "-port") == 0 && i + 1 < argc) {
            port = atoi(argv[++i]);
        }
    }

    printf("Starting server on port %d with document root: %s\n", port, document_root);

    start_server(port, document_root);

    return 0;
}
