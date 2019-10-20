#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#define SIZE 10
#define HASH_SIZE 32
uint8_t* hashArray[SIZE] = {NULL};

int hashIndex(uint64_t key){
    return key % SIZE;
}

uint8_t * find(uint64_t key) {
    return hashArray[hashIndex(key)];
}

void insert(uint64_t key, const uint8_t* value){
    int hashKey = hashIndex(key);
    if (hashArray[hashKey] == NULL) {
        hashArray[hashKey] = malloc(HASH_SIZE* sizeof(uint8_t));
    }

    for (int i = 0; i < HASH_SIZE; ++i) {
        hashArray[hashKey][i] = value[i];
        printf("%d\n", hashArray[hashKey][i]);
    }

    printf("\nHEJ\n");
}

int main(void){
    uint8_t hash[HASH_SIZE];
    uint64_t value = 12302;

    for (int i = 0; i < HASH_SIZE; ++i) {
        hash[i] = i;
    }

    insert(value, hash);

    uint8_t* newHash = find(value);
    if (newHash == NULL) {
        printf("YAY\n");
    } else {
        printf("AWW\n");
        for (int j = 0; j < HASH_SIZE; ++j) {
            printf("%d\n", newHash[j]);
        }
    }



}
