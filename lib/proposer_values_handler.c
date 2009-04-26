#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "event.h"
#include "libpaxos.h"
#include "libpaxos_priv.h"
#include "paxos_udp.h"

static int vh_list_size = 0;
static vh_value_wrapper * vh_list_head = NULL;
static vh_value_wrapper * vh_list_tail = NULL;


static struct event leader_msg_event;
static udp_receiver * for_leader;

static void 
vh_handle_newmsg(int sock, short event, void *arg) {
    //Make the compiler happy!
    UNUSED_ARG(sock);
    UNUSED_ARG(event);
    UNUSED_ARG(arg);
    
    assert(sock == for_leader->sock);
    
    //Read the next message
    int valid = udp_read_next_message(for_leader);
    if (valid < 0) {
        printf("Dropping invalid client-to-leader message\n");
        return;
    }

    //The message is valid, take the appropriate action
    // based on the type
    paxos_msg * msg = (paxos_msg*) &for_leader->recv_buffer;
    switch(msg->type) {
        case submit: {
            vh_enqueue_value(msg->data, msg->data_size);
        }
        break;

        default: {
            printf("Unknow msg type %d received by proposer\n", msg->type);
        }
    }
}

int
vh_init() {
    //Create the emtpy values list
    vh_list_size = 0;
    vh_list_head = NULL;
    vh_list_tail = NULL;
    
    
    // Start listening on net where clients send values
    for_leader = udp_receiver_new(PAXOS_SUBMIT_NET);
    if (for_leader == NULL) {
        printf("Error creating proposer network receiver\n");
        return -1;
    }
    event_set(&leader_msg_event, for_leader->sock, EV_READ|EV_PERSIST, vh_handle_newmsg, NULL);
    event_add(&leader_msg_event, NULL);    
    
    return 0;
}

void 
vh_shutdown() {
    
}


int vh_pending_list_size() {
    return vh_list_size;
}


void vh_enqueue_value(char * value, size_t value_size) {
    //Create wrapper
    vh_value_wrapper * new_vw = PAX_MALLOC(sizeof(vh_value_wrapper));
    //Copy value in
    new_vw->value = PAX_MALLOC(value_size);
    memcpy(new_vw->value, value, value_size);
    new_vw->value_size = value_size;
    
    new_vw->next = NULL;
    
    /* List is empty*/
	if (vh_list_head == NULL && vh_list_tail == NULL) {
		vh_list_head = new_vw;
		vh_list_tail = new_vw;
        vh_list_size = 1;

	/* List is not empty*/
	} else {
		vh_list_tail->next = new_vw;
		vh_list_tail = new_vw;
        vh_list_size += 1;
	}
    LOG(DBG, ("Value of size %lu enqueued\n", value_size));
}

vh_value_wrapper * 
vh_get_next_pending() {
    /* List is empty*/
	if (vh_list_head == NULL && vh_list_tail == NULL) {
        return NULL;
    }
    
    /* Pop */
    vh_value_wrapper * first_vw = vh_list_head;
    vh_list_head = first_vw->next;

    /* Also last element */
    if(vh_list_tail == first_vw) {
        vh_list_tail = NULL;
    }
    vh_list_size -= 1;
    LOG(DBG, ("Popping value of size %lu\n", first_vw->value_size));
    return first_vw;
}

void 
vh_push_back_value(vh_value_wrapper * vw) {
    /* Adds as list head*/
    vw->next = vh_list_head;
    
    /* List is empty*/
	if (vh_list_head == NULL && vh_list_tail == NULL) {
		vh_list_head = vw;
		vh_list_tail = vw;
        vh_list_size = 1;

	/* List is not empty*/
	} else {
		vh_list_head = vw;
        vh_list_size += 1;
	}
}

void vh_notify_client(unsigned int result, vh_value_wrapper * vw) {
    // TODO
    // printf("NOTIFY CLIENT!\n");
}