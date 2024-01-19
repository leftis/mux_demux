#include <stdio.h>
#include <pcap/pcap.h>

void packetHandler(unsigned char *user, const struct pcap_pkthdr *pkthdr, const unsigned char *packet)
{
  printf("Received a packet!\n");

  // Process the packet data here
  // You can access the packet data using the 'packet' parameter
}

int main()
{
  char errbuf[PCAP_ERRBUF_SIZE];

  // Open the network interface for packet capturing
  pcap_t *handle = pcap_open_live("lo0", BUFSIZ, 1, 1000, errbuf);
  if (handle == NULL)
  {
    fprintf(stderr, "Could not open interface: %s\n", errbuf);
    return 1;
  }

  // Set a packet filter (optional)
  struct bpf_program fp;
  char filter_exp[] = "tcp";
  if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1)
  {
    fprintf(stderr, "Could not parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
    return 1;
  }

  if (pcap_setfilter(handle, &fp) == -1)
  {
    fprintf(stderr, "Could not install filter %s: %s\n", filter_exp, pcap_geterr(handle));
    return 1;
  }

  // Capture packets indefinitely
  pcap_loop(handle, 0, packetHandler, NULL);

  // Close the capture handle
  pcap_close(handle);

  return 0;
}
