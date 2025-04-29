#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

int main() {
    int sockfd, fd1, length;
    char buf[100];
    struct sockaddr_in sa, ta1;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Configure server address
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = 60018;

    // Bind the socket to the port
    if (bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Listen for incoming connections
    listen(sockfd, 1);

    // Accept a client connection
    length = sizeof(sa);
    fd1 = accept(sockfd, (struct sockaddr *)&ta1, &length);

    // Receive command from client
    recv(fd1, buf, 100, 0);

    if (strncmp(buf, "list", 4) == 0) {
        memset(buf, '\0', sizeof(buf));

        // List files in the current directory
        DIR *d;
        struct dirent *dir;
        d = opendir(".");
        if (d) {
            int i = 0;
            while ((dir = readdir(d)) != NULL) {
                int j = 0;
                while (dir->d_name[j] != '\0') {
                    buf[i++] = dir->d_name[j++];
                }
                buf[i++] = '\n';
            }
            closedir(d);
            send(fd1, buf, 100, 0);

            // Receive file name from client
            char filename[100];
            memset(filename, '\0', sizeof(filename));
            recv(fd1, filename, 100, 0);
            printf("\nRequested file: %s\n", filename);

            // Open and send file content
            FILE *file = fopen(filename, "r");
            if (file == NULL) {
                printf("Failed to open the file.\n");
                return 1;
            }

            while (fgets(buf, 100, file) != NULL) {
                send(fd1, buf, 100, 0);
                memset(buf, '\0', sizeof(buf));
            }

            fclose(file);
            close(fd1);
        }
    } else {
        printf("Invalid command\n");
        close(fd1);
    }

    return 0;
}
