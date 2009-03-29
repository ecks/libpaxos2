#ifndef ACCEPTOR_STABLE_STORAGE_H_C2XN5QX9
#define ACCEPTOR_STABLE_STORAGE_H_C2XN5QX9

int stablestorage_init(int acceptor_id);
void stablestorage_do_recovery();
int stablestorage_shutdown();

void stablestorage_tx_begin();
void stablestorage_tx_end();
acceptor_record * stablestorage_get_record(iid_t iid);
int stablestorage_update_record(accept_req * ar);

#endif /* end of include guard: ACCEPTOR_STABLE_STORAGE_H_C2XN5QX9 */
