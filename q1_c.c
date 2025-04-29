#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

int main()
{
	int sockfd;
	char buf[100];
	struct sockaddr_in sa;

	// Create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// Specify server details
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr("127.0.0.1"); // Loopback IP address
	sa.sin_port = 60018;

	// Connect to the server
	connect(sockfd, (struct sockaddr *)&sa, sizeof(sa));

	// Send command to the server
	memset(buf, '\0', 100);
	printf("Enter the command: ");
	scanf("%s", buf);
	send(sockfd, buf, 100, 0);

	// Receive the file name list
	memset(buf, '\0', 100);
	recv(sockfd, buf, 100, 0);
	printf("%s\n", buf);

	// Send the file name to download
	printf("Enter the file name to download: ");
	memset(buf, '\0', 100);
	scanf("%s", buf);
	send(sockfd, buf, 100, 0);

	// Simulate download progress
	printf("Downloading...0%%\n");
	printf("Downloading...50%%\n");

	// Save the downloaded content to a file
	FILE *file;
	const char *filename = "downloaded";
	file = fopen(filename, "w");
	if (file == NULL) {
		printf("Failed to create the file.\n");
		close(sockfd);
		return 1;
	}

	fprintf(file, "%s", buf);
	fclose(file);

	printf("Downloading...100%%\n");

	// Close the socket
	close(sockfd);
	return 0;
}
