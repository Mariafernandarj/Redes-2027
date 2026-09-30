#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>

#define ETHERTYPE_PRACTICA 0x88B5

// Imprime una dirección MAC de 6 bytes en formato hexadecimal separado por dos puntos (XX:XX:XX:XX:XX:XX)
void imprime_mac(const char *nombre, unsigned char *mac)
{
    printf("%s: %02x:%02x:%02x:%02x:%02x:%02x\n",
           nombre, mac[0], mac[1], mac[2],
           mac[3], mac[4], mac[5]);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Uso: %s <interfaz>\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct ifreq ifr = {0};
    strncpy(ifr.ifr_name, argv[1], IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
        perror("SIOCGIFINDEX");
        close(sock);
        return 1;
    }

    struct sockaddr_ll addr = {0};
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = ifr.ifr_ifindex;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return 1;
    }

    printf("Escuchando en %s...\n", argv[1]);

    unsigned char buffer[ETH_FRAME_LEN];

    while (1) {
        ssize_t n = recvfrom(sock, buffer, sizeof(buffer), 0, NULL, NULL);

        if (n < ETH_HLEN)
            continue;

        struct ethhdr *eth = (struct ethhdr *)buffer;

        if (ntohs(eth->h_proto) != ETHERTYPE_PRACTICA)
            continue;

        size_t payload_len = n - ETH_HLEN;

        printf("\n--- Trama recibida ---\n");
        imprime_mac("MAC destino", eth->h_dest);
        imprime_mac("MAC origen", eth->h_source);
        printf("EtherType: 0x%04x\n", ntohs(eth->h_proto));

        printf("Payload: ");
        for (size_t i = 0; i < payload_len && buffer[ETH_HLEN + i]; i++)
            putchar(isprint(buffer[ETH_HLEN + i])
                    ? buffer[ETH_HLEN + i] : '.');

        printf("\n");
    }

    close(sock);
}
