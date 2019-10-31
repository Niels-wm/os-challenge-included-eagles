

uint64_t reversehashing2(uint64_t start, uint64_t end, uint8_t *hash);
void initReverseHashing(pthread_mutex_t* htLock);
void* reversehashing (void *arg);
