#define NAME_SERVER  "8.8.8.8" /* google-public-dns-a.google.com */
#define BUFFER_SIZE  512
#define HEX_WIDTH    16

typedef struct {
  char* request;
  int type; /* 0x01 for domain->IPv4, 0x0C for reverse DNS lookup */
  int len;
  unsigned char bytes[BUFFER_SIZE];
} DNS_QUERY;


/*
 *  Dumps the contents of an array to stdout as hex
 *
 *  input: [buf] buffer to be printed
 *  input: [len] length of buf
 *  return: void
 */
void hexDump(unsigned char buf[], int len);


/*
 *  Creates a DNS_QUERY struct, given a domain (or an IP)
 *
 *  input: [domain] the domain (or IP) name we'd like to look up in DNS
 *  return: a DNS_QUERY struct with the data we need to send
 */
DNS_QUERY createQuery(char* domain);


/*
 *  Does a DNS lookup and prints the result
 *
 *  input: [query] pointer to a DNS_QUERY struct with the data to be sent
 *  return: 0 if successful, -1 otherwise
 */
int dnsLookup(DNS_QUERY* query);

