#ifndef PAXOS_UDP_H_X98E254H
#define PAXOS_UDP_H_X98E254H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

typedef struct udp_send_buffer_t {
    int sock;
    struct sockaddr_in addr;
    int dirty;
    // size_t bufsize;
    char buffer[MAX_UDP_MSG_SIZE];
} udp_send_buffer;

typedef struct udp_receiver_t {
    int sock;
    struct sockaddr_in addr;
    char recv_buffer[MAX_UDP_MSG_SIZE];
} udp_receiver;


udp_send_buffer * udp_sendbuf_new(char* address_string, int port);
void sendbuf_clear(udp_send_buffer * sb, paxos_msg_code type, short int sender_id);
void sendbuf_flush(udp_send_buffer * sb);


void sendbuf_add_repeat_req(udp_send_buffer * sb, iid_t iid);
void sendbuf_add_accept_ack(udp_send_buffer * sb, acceptor_record * rec);

void sendbuf_add_prepare_req(udp_send_buffer * sb, iid_t iid, ballot_t ballot);
void sendbuf_add_prepare_ack(udp_send_buffer * sb, acceptor_record * rec);

udp_receiver * udp_receiver_new(char* address_string, int port);
int udp_read_next_message(udp_receiver * recv_info);
int udp_receiver_destroy(udp_receiver * rec);


#endif /* end of include guard: PAXOS_UDP_H_X98E254H */
