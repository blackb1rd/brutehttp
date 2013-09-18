/*
 * =====================================================================================
 *
 *       Filename:  brutehttp.c
 *
 *    Description:  This tool for brute force directories and files names on web/application servers.
 *
 *        Version:  0.2
 *        Created:  09/18/2013 03:13:30 PM
 *       Revision:  2
 *       Compiler:  gcc
 *
 *         Author:  blackb1rd (blackb1rd@riseup.net),
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
//#include <pthread.h>
#include <regex.h>
#include <string.h>

#define PORT 80
#define USERAGENT "Brutehttp 0.2"

int http_head(char *host, char *page);
void usage(void);
 
int main(int argc, char **argv)
{
  int result;
  FILE *fp;
  if ( argc < 3 )
  {
    usage();
    return 1;
  }
  if (argc == 3) {
    char line[256];
    fp = fopen(argv[2], "r");
    while(fgets(line,sizeof(line),fp) != NULL) {
      int len = strlen(line)-1;
      if(line[len] == '\n')
        line[len] = '\0';
      result = http_head(argv[1], line);
      if (result)
        printf("-->%s  %d\n",line,result);
    }
    fclose(fp);
  }
  return 0;
}
void usage(void)
{
  fprintf(stderr, "USAGE: brutehttp host [dict]\n\
  \thost: the website hostname. ex: www.google.com\n\
  \tdict: the dictionary. ex: dict.txt\n");
}
int http_head(char *host, char *page)
{
  struct sockaddr_in server;
  struct hostent *hent;
  char buf[BUFSIZ];
  char ip[INET_ADDRSTRLEN];
  int rescode;
  char *head;
  char *tpl = "HEAD /%s HTTP/1.1\r\nUser-Agent: %s\r\nHost: %s\r\nAccept: */*\r\nConnection: close\r\n\r\n";
  int sock_tcp;
  //create socket TCP
  sock_tcp = socket (AF_INET , SOCK_STREAM , IPPROTO_TCP);
  hent = gethostbyname(host);
  if(inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, INET_ADDRSTRLEN) == NULL) {
    perror("Can't resolve host");
    exit(2);
  }
  inet_pton (AF_INET, ip, (void *) (&(server.sin_addr.s_addr)));
  server.sin_family = AF_INET;
  server.sin_port = htons (PORT);
  if (connect(sock_tcp, (struct sockaddr *)&server, sizeof(struct sockaddr)) < 0) {
    puts("Could not connect");
    exit(2);
  }
  head = (char *) malloc(strlen(host)+strlen(page)+strlen(USERAGENT)+strlen(tpl)-5);
  sprintf(head, tpl, page, USERAGENT, host);
  if (send(sock_tcp, head, strlen(head), 0) < 0) {
    puts("Send failed");
    exit(2);
  }
  if (recv(sock_tcp, buf, BUFSIZ, 0) < 0) {
    puts("recv failed");
    exit(2);
  }
  sscanf(buf, "HTTP/1.%*[^ ] %d[^ ]", &rescode);
  close(sock_tcp);
  free(head);
  if (rescode == 200 ||rescode == 302)
    return rescode;
  else
    return 0;
}
