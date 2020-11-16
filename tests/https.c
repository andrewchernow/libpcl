
#include <pcl/ssl.h>
#include <pcl/socket.h> // only needed for pcl_socket_setrcvtimeo */
#include <unistd.h>

#define DEFAULT_HOST "www.google.com"
#define REQUEST \
"GET / HTTP/1.1\r\n" \
"Host: %s\r\n" \
"Connection: close\r\n\r\n"

int main(int argc, char **argv)
{
	int r;
	char request[512];
	pcl_ssl_t *ssl;
	const char *host = argc > 1 ? argv[1] : DEFAULT_HOST;

	pcl_init(NULL, 0);

	if(!(ssl = pcl_ssl_create(0)))
		PANIC("failed to create SSL socket", 0);

	if(pcl_ssl_connect(ssl, host, 443, NULL))
		PANIC("connect failed", 0);

	/* request is large enough for max hostname, so given host is invalid */
	if((r = pcl_sprintf(request, sizeof(request), REQUEST, host)) < 0)
		PANIC("Provided host is invalid: %s", host);

	printf("Request:\n%s", request);

	if(pcl_ssl_send(ssl, request, r) < 0)
		PANIC("sending request failed", 0);

	/* some sites just hang forever. time'm out */
	if(pcl_socket_setrcvtimeo(pcl_ssl_socket(ssl), 5000))
		PANIC("setting receive timeout", 0);

	printf("receiving ... will wait for 5 seconds\n");

	while(1)
	{
		char buf[8192];

		if((r = pcl_ssl_recv(ssl, buf, sizeof(buf))) > 0)
			fwrite(buf, 1, r, stdout);
		else
			break;
	}

	if(r < 0 && pcl_errno != PCL_ECONNRESET)
		PTRACE("reading response failed", 0);

	pcl_close(ssl);

	return 0;
}
