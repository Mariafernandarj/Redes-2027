#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>

#define ETHERTYPE_PRACTICA 0x88B5
#define MIN_PAYLOAD 46
#define MAX_PAYLOAD 1500

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("Uso: %s <interfaz> <MAC_destino> \"<mensaje>\"\n", argv[0]);
        return 1;
    }

    //MAC destino
    unsigned char dst[6];
    if (sscanf(argv[2], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &dst[0], &dst[1], &dst[2],
               &dst[3], &dst[4], &dst[5]) != 6) {
        printf("MAC destino invalida\n");
        return 1;
    }

    //Socket de capa 2
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    //Obtener información de la interfaz
    struct ifreq ifr = {0};
    strncpy(ifr.ifr_name, argv[1], IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
        perror("SIOCGIFINDEX");
        close(sock);
        return 1;
    }

    int ifindex = ifr.ifr_ifindex;

    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
        perror("SIOCGIFHWADDR");
        close(sock);
        return 1;
    }

    unsigned char *src = (unsigned char *)ifr.ifr_hwaddr.sa_data;

    //Construir trama Ethernet
    unsigned char trama[ETH_HLEN + MAX_PAYLOAD] = {0};

    memcpy(trama, dst, 6);
    memcpy(trama + 6, src, 6);

    unsigned short type = htons(ETHERTYPE_PRACTICA);
    memcpy(trama + 12, &type, 2);

    size_t len = strlen(argv[3]);
    if (len > MAX_PAYLOAD)
        len = MAX_PAYLOAD;

    memcpy(trama + ETH_HLEN, argv[3], len);

    size_t payload = len < MIN_PAYLOAD ? MIN_PAYLOAD : len;
    size_t total = ETH_HLEN + payload;

    //Dirección destino 
    struct sockaddr_ll addr = {0};
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = ifindex;
    addr.sll_halen = ETH_ALEN;
    memcpy(addr.sll_addr, dst, 6);

    if (sendto(sock, trama, total, 0,
               (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("sendto");
        close(sock);
        return 1;
    }

    printf("Trama enviada correctamente (%zu bytes)\n", total);

    close(sock);
    return 0;
}
