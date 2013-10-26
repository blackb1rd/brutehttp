/*
 * =====================================================================================
 *
 *       Filename:  brutehttp.c
 *
 *    Description:  This tool for brute force directories and files names on web/application servers.
 *
 *        Version:  0.4
 *        Created:  09/18/2013 03:13:30 PM
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
#include <string.h>

#define PORT 80
#define USERAGENT "Brutehttp 0.4"

void usage(void);
 
int main(int argc, char **argv)
{
  if ( argc < 3 ) {
    usage();
    return 1;
  }
  if (argc == 3) {
    char page[71];
    char buf[BUFSIZ];
    char ip[INET_ADDRSTRLEN];
    char *head;
    unsigned char i;
    unsigned int rescode;
    int sock_tcp;
    FILE *fp;
    struct sockaddr_in server;
    struct hostent *hent;
    
    head = (char *) malloc(strlen(argv[1])+strlen(USERAGENT)+70+78-5);//limit dictionary 70 character
    fp = fopen(argv[2], "r");
    printf("--------------------------------------------------------------------------------\n");
    printf("| Directories & Files Names                                             |Status|\n");
    printf("--------------------------------------------------------------------------------\n");
    hent = gethostbyname(argv[1]);
    if(inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, INET_ADDRSTRLEN) == NULL) {
      perror("Can't resolve host");
      return 1;
    }
    inet_pton (AF_INET, ip, (void *) (&(server.sin_addr.s_addr)));
    server.sin_family = AF_INET;
    server.sin_port = htons (PORT);
    while(fgets(page,70,fp) != NULL) {
      if(page[strlen(page)-1] == '\n')
        page[strlen(page)-1] = '\0';
      printf("\r%-70s", page);
      //create socket TCP
      sock_tcp = socket (AF_INET , SOCK_STREAM , IPPROTO_TCP);
      if (connect(sock_tcp, (struct sockaddr *)&server, sizeof(struct sockaddr)) < 0) {
        perror("Could not connect");
        return 1;
      }
      sprintf(head, "HEAD /%s HTTP/1.1\r\nUser-Agent: %s\r\nHost: %s\r\nAccept: */*\r\nConnection: close\r\n\r\n", page, USERAGENT, argv[1]);
      if (send(sock_tcp, head, strlen(head), 0) < 0) {
        perror("Send failed");
        return 1;
      }
      if (recv(sock_tcp, buf, BUFSIZ, 0) < 0) {
        perror("recv failed");
        return 1;
      }
      sscanf(buf, "HTTP/1.%*[^ ] %d[^ ]", &rescode);
      if (rescode != 404)
        printf("\r| %-70s|  %d |\n", page, rescode);
    }
    printf("\r--------------------------------------------------------------------------------\n");
    free(head);
    fclose(fp);
  }
  return 0;
}
void usage(void)
{
  fprintf(stderr, "USAGE: brutehttp host [dict]\n\
  \thost: the website hostname.  ex: www.google.com\n\
  \tdict: the dictionary.        ex: dict.txt\n");
}
