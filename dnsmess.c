#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "dnsmess.h"

/*
 * Name: dnsmess
 * Auth: Gavin Lloyd <gavinhungry@gmail.com>
 * Date: Mar 2011
 * Desc: IPv4 DNS lookup/reverse lookup tool with full request/response output
 *
 * This program will report multiple IPv4 addresses for a given domain (if they
 * exist), or multiple domains (reverse DNS lookup) for a given IPv4 address (if
 * they exist).  It also reports NXDOMAIN or SERVFAIL where appropriate.
 *
 * usage: dnsmess [DOMAIN | IP]
 */


int main(int argc, char* argv[]) {

  int ret;
  DNS_QUERY query;

  /* only a single command-line argument is accepted */
  if (argc != 2 || (argc == 2 && argv[1][0] == '-')) {
    fprintf(stderr, "usage: %s [DOMAIN | IP]\n\n", argv[0]);
    return EXIT_FAILURE;
  }

  /* create a DNS_QUERY from the domain (or IP) passed */
  query = createQuery(argv[1]);

  /* hex dump of the DNS query */
  printf("\nDNS Query (%i bytes):\n", query.len);
  hexDump(query.bytes, query.len);

  ret = dnsLookup(&query);
  printf("\n");

  return !ret ? EXIT_SUCCESS : EXIT_FAILURE;
}



/*
 *  Dumps the contents of an array to stdout as hex
 *
 *  input: [buf] buffer to be printed
 *  input: [len] length of buf
 *  return: void
 */
void hexDump(unsigned char buf[], int len) {

  int i, j, width;
  width = (len % HEX_WIDTH) ? len + (HEX_WIDTH-(len % HEX_WIDTH)) : len;

  for (i=0; i<width; i++) {

    /* print hex portion */
    if (i >= len) printf("-- ");
    /* else if (buf[i] == 0xC0) printf("\e[32m%02hhX\e[00m ", buf[i]); */ /* highlight 0xC0 */
    else printf("%02hhX ", buf[i]);

      /* only output HEX_WIDTH bytes on single line */
      if (!((i+1) % HEX_WIDTH)) {

        /* before printing a newline, we need to print out in ASCII */
        printf(" ");
        for (j=i-HEX_WIDTH+1; j<=i; j++) {

          /* if (!((j) % (HEX_WIDTH/2))) printf(" "); */ /* divider for easier reading */
          if (j >= len) break;                     /* don't print what's not there */
          else if (buf[j] < 0x21 || buf[j] > 0x7E) /* don't print the unprintable */
            printf(".");
          else printf("%c", buf[j]); /* do print the rest */
        }
        printf("\n");
      } /* else if (!((i+1) % (HEX_WIDTH/2))) printf(" "); */ /* divider for easier reading */
  }
  printf("\n");
  return;
}



/*
 *  Creates a DNS_QUERY struct, given a domain (or an IP)
 *
 *  input: [domain] the domain (or IP) name we'd like to look up in DNS
 *  return: a DNS_QUERY struct with the data we need to send
 */
DNS_QUERY createQuery(char* domain) {

  char* comp;

  /* if we can convert the string to a binary IPv4 address, presume a reverse lookup */
  struct sockaddr_in serv;
  int i, reverse = inet_aton(domain, &serv.sin_addr);

  /* the first 12 bytes of the query are hard-coded in network-byte order */
  DNS_QUERY query = {
    .request = strdup(domain),
    .type = reverse ? 0x0C : 0x01,
    .len = 12,
    .bytes = { 0x47, 0x4C, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
  };

  if (reverse) {
    serv.sin_addr.s_addr = htonl(serv.sin_addr.s_addr);
    domain = inet_ntoa(serv.sin_addr);
    strcat(domain, ".in-addr.arpa"); /* RFC 1035 3.5 */
  }

  /* start spliting up the domain into components */
  comp = strtok(domain, ".");
  while (comp) {

    /* append the number of bytes in this component */
    query.bytes[query.len++] = strlen(comp);

    /* append the component itself */
    for (i=0; i < strlen(comp); i++)
      query.bytes[query.len++] = comp[i];

    /* get the next component */
    comp = strtok(NULL, ".");
  }
  query.bytes[query.len++] = 0x00; /* finalize with a zero-byte */
  query.bytes[query.len++] = 0x00; query.bytes[query.len++] = query.type; /* lookup type */
  query.bytes[query.len++] = 0x00; query.bytes[query.len++] = 0x01; /* lookup class */

  return query;
}



/*
 *  Does a DNS lookup and prints the result
 *
 *  input: [query] pointer to a DNS_QUERY struct with the data to be sent
 *  return: 0 if successful, -1 otherwise
 */
int dnsLookup(DNS_QUERY* query) {

  struct sockaddr_in serv;
  unsigned char buf[BUFFER_SIZE];
  int sock, size, send, rec, i, j, k = 0, rcode;
  int ip = 0, dom = 0;

  /* clear the buffer */
  memset(buf, 0x00, BUFFER_SIZE);

  /* create a socket */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("error with socket");
    return -1;
  }

  serv.sin_family = AF_INET;
  serv.sin_port = htons(53);
  serv.sin_addr.s_addr = inet_addr(NAME_SERVER);
  size = sizeof(serv);

  /* send the DNS query */
  send = sendto(sock, query->bytes, query->len,  0, (struct sockaddr*)&serv,  size);
  if (send < 0) {
    perror("error sending query");
    return -1;
  }

  /* receive the DNS response and close the socket */
  rec = recvfrom(sock, buf, BUFFER_SIZE, 0, (struct sockaddr*)&serv, &size);
  if (rec < 0) {
    perror("error receiving response");
    return -1;
  }

  close(sock);

  /* hex dump of the DNS response */
  printf("DNS Response (%i bytes):\n", rec);
  hexDump(buf, rec);

  /* RFC 1035 4.1.1: RCODE is the last 4 bits, so we apply a bitmask */
  rcode = (buf[3] & 0x0F);

  if (rcode == 2) {
    printf("nameserver %s returned SERVFAIL:\n", NAME_SERVER);
    printf("  the name server was unable to process this query due to a\n  problem with the name server.\n");
    return -1;
  } else if (rcode == 3) {
      printf("nameserver %s returned NXDOMAIN for %s:\n", NAME_SERVER, query->request);
      printf("  the domain name referenced in the query does not exist\n");
      return -1;
    }

  /* search for and print IPv4 addresses */
  if (query->type == 0x01) {
    printf("IPv4 address(es) for %s:\n", query->request);
    for (i=0; i<rec; i++) {
      if (buf[i] == 0xC0 && buf[i+3] == 0x01) {
        ip++; i += 12; /* ! += buf[i+1]; */
        printf("  %u.%u.%u.%u\n", buf[i], buf[i+1], buf[i+2], buf[i+3]);
      }
    }

    if (!ip) {
      printf("  no IPv4 address found in the DNS response!\n");
      return -1;
    }
  }

  /* search for and print domain names */
  if (query->type == 0x0C) {
    printf("Domain name(s) for %s:\n", query->request);
    for (i=0; i<rec; i++) {
      if (buf[i] == 0xC0 && buf[i+3] == 0x0C) {
        dom++; i += buf[i+1];
        printf("  ");
        while (1) {
          j = buf[i];
          printf("%.*s", j, &buf[++i]);
          i += j;
          if (buf[i] == 0x00) {
            if (k) { /* if we've remembered an old position */
              i = k + 1; /* move to just past the old position */
              k = 0; /* go from there as usual */
            }
            break;
          }
          if (buf[i] == 0xC0 || buf[i] == 0xC1) { /* DNS compression */
            k = i; /* remember old position */
            i = (buf[i] == 0xC0) ? buf[i+1] : (rec+1) - buf[i+1];
          }
          printf(".");
        }
       printf("\n");
      }
    }

    if (!dom) {
      printf("  no domain name found in the DNS response!\n");
      return -1;
    }
  }

  return 0;
}

