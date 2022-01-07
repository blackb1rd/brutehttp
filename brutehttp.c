/*
* =====================================================================================
*
* Filename: brutehttp.c
*
* Description: This tool for brute force directories and files names on web/application servers.
*
* Version: 0.5
* Created: 09/18/2013 03:13:30 PM
* Compiler: gcc
*
* Author: Prachya Saechua (blackb1rd@blackb1rd.dev),
*
* =====================================================================================
*/

#include <stdio.h>
#include <resolv.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <uv.h>

static void usage(void);
char* gethost(char* url);

int main(int argc,const char **argv)
{
  if (argc < 3 || argc > 4) {
    usage();
    return 1;
  }

  char ip[INET_ADDRSTRLEN];
  char *head;
  FILE *fp;
  struct hostent *hent;
  const char *url = argv[1];
  const char *method = argv[3];
  if (method == NULL) {
    method = "GET";
  }
  const char *USERAGENT  = "Brutehttp 1.0";
  const int  PORT        = 80;

  head = malloc(2048);

  char *hostname = gethost((char*)url);
  hent = gethostbyname(hostname);
  printf("Hostname: %s\n", hent->h_name);
  printf("Address type #: %d\n", hent->h_addrtype);
  printf("Address length: %d\n", hent->h_length);
  printf("Method: %s\n", method);

  if (inet_ntop(AF_INET, (void *)hent->h_addr, ip, INET_ADDRSTRLEN) == NULL) {
    perror("Can't resolve hostname");
    return 1;
  }

  struct sockaddr_in target;

  inet_pton(AF_INET, ip, (void *) (&(target.sin_addr.s_addr)));
  target.sin_family = AF_INET;
  target.sin_port = htons (PORT);

  char buf[BUFSIZ];
  int sock_tcp;
  unsigned int rescode;

  printf("--------------------------------------------------------------------------------\n");
  printf("| Directories & Files Names                                             |Status|\n");
  printf("--------------------------------------------------------------------------------\n");
  fp = fopen(argv[2], "r");

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&line, &len, fp)) != -1) {
    if ((line)[read - 1] == '\n') {
      (line)[read - 1] = '\0';
    }
    //create socket TCP
    if ((sock_tcp = socket (AF_INET , SOCK_STREAM , IPPROTO_TCP)) < 0) {
      perror("Could not create socket TCP");
      return 1;
    }

    //printf("\r%-70s", line);
    snprintf(head, 2048, "%s /%s HTTP/1.1\r\nUser-Agent: %s\r\nHost: %s\r\nAccept: */*\r\nConnection: close\r\n\r\n", method, line, USERAGENT, url);
    if (connect(sock_tcp, (struct sockaddr *)&target, sizeof(target)) < 0) {
      perror("Could not connect");
      return 1;
    }
    send(sock_tcp, head, strlen(head), 0);
    recv(sock_tcp, buf, BUFSIZ, 0);

    sscanf(buf, "HTTP/1.%*[^ ] %3u[^ ]", &rescode);
    printf("\r| %-70s| %d |\n", line, rescode);
  }
  printf("\r-------------------------------------------------------------------------------\n");

  free(head);
  fclose(fp);

  return 0;
}

char* gethost(char* url)
{
  if (strstr(url, ":"))
  {
    char *host = strtok(url, ":");
    host = strtok(NULL, ":");
    return strtok(host, "/");
  }
  else
  {
    return strtok(url, "/");
  }
}

static void usage(void)
{
  fprintf(stderr, "USAGE: ./brutehttp Host [Dict]\n\
  Host : The website hostname. ex: www.google.com\n\
  Dict : The dictionary.       ex: dict.txt\n");
}

