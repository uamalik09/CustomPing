#include <iostream>
#include <cstdlib>
#include <cstring>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <ctime>
#include <sys/time.h>

#define PING_PKT_S 64
#define PORT_NO 0
#define PING_SLEEP_RATE 1000000
#define RECV_TIMEOUT 1

int pingloop = 1;

struct ping_pkt {
    struct icmphdr hdr;
    char msg[PING_PKT_S - sizeof(struct icmphdr)];
};

unsigned short checksum(void* b, int len) {
    unsigned short* buf = static_cast<unsigned short*>(b);
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(reinterpret_cast<unsigned char*>(buf));
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void intHandler(int dummy) {
    pingloop = 0;
}

char* dns_lookup(const char* addr_host, struct sockaddr_in* addr_con) {
    std::cout << "\nResolving DNS.." << std::endl;
    struct hostent* host_entity;
    char* ip = new char[NI_MAXHOST];

    if ((host_entity = gethostbyname(addr_host)) == nullptr) {
        // No IP found for the hostname
        return nullptr;
    }

    // Filling up the address structure
    strcpy(ip, inet_ntoa(*(struct in_addr*)host_entity->h_addr));

    (*addr_con).sin_family = host_entity->h_addrtype;
    (*addr_con).sin_port = htons(PORT_NO);
    (*addr_con).sin_addr.s_addr = *(reinterpret_cast<long*>(host_entity->h_addr));

    return ip;
}

char* reverse_dns_lookup(const char* ip_addr) {
    struct sockaddr_in temp_addr;
    socklen_t len;
    char buf[NI_MAXHOST];
    char* ret_buf;

    temp_addr.sin_family = AF_INET;
    temp_addr.sin_addr.s_addr = inet_addr(ip_addr);
    len = sizeof(struct sockaddr_in);

    if (getnameinfo((struct sockaddr*)&temp_addr, len, buf, sizeof(buf), NULL, 0, NI_NAMEREQD)) {
        std::cout << "Could not resolve the reverse lookup of the hostname" << std::endl;
        return nullptr;
    }
    ret_buf = new char[strlen(buf) + 1];
    strcpy(ret_buf, buf);
    return ret_buf;
}

void send_ping(int ping_sockfd, struct sockaddr_in* ping_addr, const char* ping_dom, const char* ping_ip, const char* rev_host) {
    int ttl_val = 64;
    int msg_count = 0;
    int i;
    socklen_t addr_len;
    int flag = 1;
    int msg_received_count = 0;

    struct ping_pkt pckt;
    struct sockaddr_in r_addr;
    struct timespec time_start, time_end, tfs, tfe;
    long double rtt_msec = 0;
    long double total_msec = 0;
    struct timeval tv_out;
    tv_out.tv_sec = RECV_TIMEOUT;
    tv_out.tv_usec = 0;

    clock_gettime(CLOCK_MONOTONIC, &tfs);

    if (setsockopt(ping_sockfd, SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0) {
        std::cout << "\nSetting socket options to TTL failed!" << std::endl;
        return;
    } else {
        std::cout << "\nSocket set to TTL.." << std::endl;
    }

    setsockopt(ping_sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof tv_out);

    while (pingloop) {
        flag = 1;

        bzero(&pckt, sizeof(pckt));

        pckt.hdr.type = ICMP_ECHO;
        pckt.hdr.un.echo.id = getpid();

        for (i = 0; i < sizeof(pckt.msg) - 1; i++)
            pckt.msg[i] = i + '0';

        pckt.msg[i] = 0;
        pckt.hdr.un.echo.sequence = msg_count++;
        pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));

        usleep(PING_SLEEP_RATE);

        clock_gettime(CLOCK_MONOTONIC, &time_start);
        if (sendto(ping_sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr*)ping_addr, sizeof(*ping_addr)) <= 0) {
            std::cout << "\nPacket Sending Failed!" << std::endl;
            flag = 0;
        }

        addr_len = sizeof(r_addr);

        if (recvfrom(ping_sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, &addr_len) <= 0 && msg_count > 1) {
            std::cout << "\nPacket receive failed!" << std::endl;
        } else {
            clock_gettime(CLOCK_MONOTONIC, &time_end);

            double timeElapsed = (static_cast<double>(time_end.tv_nsec - time_start.tv_nsec)) / 1000000.0;
            rtt_msec = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + timeElapsed;

            if (flag) {
                if (!(pckt.hdr.type == 69 && pckt.hdr.code == 0)) {
                    std::cout << "Error..Packet received with ICMP type " << pckt.hdr.type << " code " << pckt.hdr.code << std::endl;
                } else {
                    std::cout << msg_count << " bytes from " << ping_dom << " (h: " << rev_host << ") (" << ping_ip << ") msg_seq=" << msg_count << " ttl=" << ttl_val << " rtt = " << rtt_msec << " ms." << std::endl;
                    msg_received_count++;
                }
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &tfe);
    double timeElapsed = (static_cast<double>(tfe.tv_nsec - tfs.tv_nsec)) / 1000000.0;

    total_msec = (tfe.tv_sec - tfs.tv_sec) * 1000.0 + timeElapsed;

    std::cout << "\n===" << ping_ip << " ping statistics===\n" << std::endl;
    std::cout << msg_count << " packets sent, " << msg_received_count << " packets received, " << (msg_count - msg_received_count) / static_cast<double>(msg_count) * 100.0 << " percent packet loss. Total time: " << total_msec << " ms.\n" << std::endl;
}

int main(int argc, char* argv[]) {
    int sockfd;
    char* ip_addr;
    char* reverse_hostname;
    struct sockaddr_in addr_con;
    int addrlen = sizeof(addr_con);
    char net_buf[NI_MAXHOST];

    if (argc != 2) {
        std::cout << "\nFormat " << argv[0] << " <address>" << std::endl;
        return 0;
    }

    ip_addr = dns_lookup(argv[1], &addr_con);
    if (ip_addr == nullptr) {
        std::cout << "\nDNS lookup failed! Could not resolve hostname!" << std::endl;
        return 0;
    }

    reverse_hostname = reverse_dns_lookup(ip_addr);
    std::cout << "\nTrying to connect to '" << argv[1] << "' IP: " << ip_addr << std::endl;
    std::cout << "\nReverse Lookup domain: " << reverse_hostname << std::endl;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        std::cout << "\nSocket file descriptor not received!!" << std::endl;
        return 0;
    } else {
        std::cout << "\nSocket file descriptor " << sockfd << " received" << std::endl;
    }

    signal(SIGINT, intHandler);

    send_ping(sockfd, &addr_con, reverse_hostname, ip_addr, argv[1]);

    return 0;
}
