#define _CRT_SECURE_NO_WARNINGS  // To Suppress error about fopen() etc.. remove this when the compiler is not Visual C/C++

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_FILE_NAME_LEN 128
#define MAX_CMD_SIZE 128

#define MAIN_BUCKET_SIZE 16
#define OVERFLOW_BUCKET_SIZE 16
#define SYNONYM_CHAIN_BUCKET_SIZE (MAIN_BUCKET_SIZE+OVERFLOW_BUCKET_SIZE)


#define EMPTY_BUCKET -1
#define NULL_LINK -1

typedef struct synonym_chain_bucket {
    int key;
    int prev_link_idx; //Link index to the previous bucket 
    int next_link_idx; //Link index to the next bucket
}synonym_chain_bucket;


//Buckets for synonym chain - begin
synonym_chain_bucket synonym_chain_buckets[SYNONYM_CHAIN_BUCKET_SIZE];
//Buckets for synonym chain - end

//Hash function - DO NOT MODIFY - begin
int hash_function(int key);
//Hash function - DO NOT MODIFY - end

//Utility functions - begin
void clear_buckets();
void print_buckets();
//Utility functions - end

//USER DEFINED FUNCTIONS - BEGIN
void clear_bucket(int bucket_idx);
int last_bucket_idx(int hash_key);
//USER DEFINED FUNCTIONS - BEGIN

//Functions to implement - begin
bool insert_key(int key_val);
int find_key(int key_val);
bool remove_key(int key_val);
//Functions to implement - end



int main(int argc, char** argv)
{
    FILE* input_cmd_fp = NULL;
    char* input_cmd_file = NULL;

    if (argc < 2) {
        printf("Usage: hash [in_cmd_file]\n");
        return 0;
    }

    input_cmd_file = argv[1];
    input_cmd_fp = fopen(input_cmd_file, "r");

    char input_cmd[MAX_CMD_SIZE + 1];
    memset(input_cmd, 0, sizeof(char) * (MAX_CMD_SIZE + 1));
    int key_val = -1;

    printf("Hi. I am a hash manager.\n");
    if (input_cmd_fp != NULL) {
        printf("Input cmd file %s is opened\n", input_cmd_file);
        // Input command
        // [cmd] [key]\n
        // cmd =>  f : find, i : insert, r : remove        
        // key =>  integer key value

        // special command
        // clear 0: clear hash bucket
        // print 0: print hash bucket

        //Bucket init
        clear_buckets();

        while (true) {
            int read_data_size = fscanf(input_cmd_fp, "%s %d\n", input_cmd, &key_val);
            if (read_data_size < 1) {
                break;
            }

            //Cmd execute            
            printf("> cmd: %s", input_cmd);
            if (strcmp(input_cmd, "i") == 0) {
                //Insert
                printf(", key:%d\n", key_val);
                bool success = insert_key(key_val);
                if (success) {
                    printf("Inserted\n");
                }
                else {
                    printf("buckets are full\n");
                }
            }
            else if (strcmp(input_cmd, "r") == 0) {
                //Remove
                printf(", key:%d\n", key_val);
                bool success = remove_key(key_val);
                if (success) {
                    printf("Key %d is removed\n", key_val);
                }
                else {
                    printf("Key %d does not exist\n", key_val);
                }
            }
            else if (strcmp(input_cmd, "f") == 0) {
                //Remove
                printf(", key:%d\n", key_val);
                int bucket_idx = find_key(key_val);
                if (bucket_idx > -1) {
                    if (bucket_idx < MAIN_BUCKET_SIZE) {
                        printf("Key %d is found at bucket [%d] in synomym_chain_main\n", key_val, bucket_idx);
                    }
                    else {
                        printf("Key %d is found at bucket [%d] in synomym_chain_overflow\n", key_val, bucket_idx);
                    }
                }
                else {
                    printf("Key %d does not exist\n", key_val);
                }
            }
            else if (strcmp(input_cmd, "clear") == 0) {
                printf(" 0\n");
                clear_buckets();
            }
            else if (strcmp(input_cmd, "print") == 0) {
                printf(" 0\n");
                print_buckets();
            }

        }
        fclose(input_cmd_fp);

    }
    else {
        printf("Input cmd file %s cannot be opened\n", input_cmd_file);
    }

    printf("All finished. Bye~\n");
    return 0;
}

// TYPE: [제산] Devide and Remainder Hash function
int hash_function(int key) {
    int address = key % MAIN_BUCKET_SIZE;
    return address;
}

bool insert_key(int key_val) {
    bool succeed = false;

    int hash_key = hash_function(key_val);

    // hash_key를 가지는 버킷이 비어있는 경우, 바로 insert 한다.
    if (synonym_chain_buckets[hash_key].key == EMPTY_BUCKET) {
        synonym_chain_buckets[hash_key].key = key_val;
        succeed = true;
    }
    // hask_key를 가지는 버킷에 값이 하나라도 들어있는 경우,
    // 오버플로우 구역을 순차탐색 하면서 빈 공간에 key_val을 insert하고 서로 연결해준다.
    else {
        int last_idx = last_bucket_idx(hash_key);
        for (int i = OVERFLOW_BUCKET_SIZE; i < SYNONYM_CHAIN_BUCKET_SIZE; ++i) {
            if (synonym_chain_buckets[i].key == EMPTY_BUCKET) {
                synonym_chain_buckets[i].key = key_val;
                synonym_chain_buckets[i].prev_link_idx = last_idx;
                synonym_chain_buckets[last_idx].next_link_idx = i;
                succeed = true;
                break;
            }
        }
    }

    return succeed;
}

int find_key(int key_val) {
    int bucket_idx = -1;

    int hash_key = hash_function(key_val);
    // 일반 구역에 찾는 값이 있는 경우
    if (synonym_chain_buckets[hash_key].key == key_val) {
        bucket_idx = hash_key;
    }
    // 오버플로우 구역에서 탐색해야 하는 경우
    else {
        // 순차탐색
        for (int i = OVERFLOW_BUCKET_SIZE; i < SYNONYM_CHAIN_BUCKET_SIZE; ++i) {
            if (synonym_chain_buckets[i].key == key_val) {
                bucket_idx = i;
                break;
            }
        }
    }

    return bucket_idx;
}

bool remove_key(int key_val) {
    bool succeed = false;

    int find_idx = find_key(key_val);
    // key_val 값을 가지는 버킷이 존재하지 않는 경우
    if (find_idx == -1) {
        return succeed;
    }
    // 일반 구역에 저장된 버킷의 값일 경우(버킷의 prev_link_idx가 NULL_LINK인 경우)
    else if (0 <= find_idx && find_idx < MAIN_BUCKET_SIZE) {
        // 삭제하려는 버킷에 연결된 버킷이 없을 경우
        if (synonym_chain_buckets[find_idx].next_link_idx == NULL_LINK) {
            clear_bucket(find_idx);
        }
        // 삭제하려는 버킷에 연결된 버킷이 있을 경우
        else {
            int next_idx = synonym_chain_buckets[find_idx].next_link_idx;
            synonym_chain_buckets[find_idx].key = synonym_chain_buckets[next_idx].key;
            synonym_chain_buckets[find_idx].next_link_idx = synonym_chain_buckets[next_idx].next_link_idx;

            int next_next_idx = synonym_chain_buckets[next_idx].next_link_idx;
            if (synonym_chain_buckets[next_next_idx].key != EMPTY_BUCKET) {
                synonym_chain_buckets[next_next_idx].prev_link_idx = find_idx;
            }

            clear_bucket(next_idx);
        }
    }
    // 오버플로우 구역에 저장된 버킷의 값일 경우
    else if (OVERFLOW_BUCKET_SIZE <= find_idx && find_idx < SYNONYM_CHAIN_BUCKET_SIZE) {
        // 같은 hash_key를 가지는 값들 중 마지막 순서의 값일 경우
        if (synonym_chain_buckets[find_idx].next_link_idx == NULL_LINK) {
            int prev_idx = synonym_chain_buckets[find_idx].prev_link_idx;
            synonym_chain_buckets[prev_idx].next_link_idx = NULL_LINK;

            clear_bucket(find_idx);
        }
        // 같은 hash_key를 가지는 값들 중 중간에 있는 값일 경우
        else {
            int prev_idx = synonym_chain_buckets[find_idx].prev_link_idx;
            int next_idx = synonym_chain_buckets[find_idx].next_link_idx;
            
            synonym_chain_buckets[prev_idx].next_link_idx = next_idx;
            synonym_chain_buckets[next_idx].prev_link_idx = prev_idx;

            clear_bucket(find_idx);
        }
    }

    succeed = true;

    return succeed;
}

///////////////// USER DEFINED FUNCTIONS - BEGIN

void clear_bucket(int bucket_idx) {
    synonym_chain_buckets[bucket_idx].key = EMPTY_BUCKET;
    synonym_chain_buckets[bucket_idx].prev_link_idx = NULL_LINK;
    synonym_chain_buckets[bucket_idx].next_link_idx = NULL_LINK;
}

int last_bucket_idx(int hash_key) {
    while (synonym_chain_buckets[hash_key].next_link_idx != NULL_LINK) {
        hash_key = synonym_chain_buckets[hash_key].next_link_idx;
    }
    return hash_key;
}

///////////////// USER DEFINED FUNCTIONS - BEGIN


///////////////// UTIL FUNCTIONS - BEGIN

void clear_buckets() {
    for (int i = 0; i < SYNONYM_CHAIN_BUCKET_SIZE; i++) {
        synonym_chain_buckets[i].key = EMPTY_BUCKET;
        synonym_chain_buckets[i].prev_link_idx = NULL_LINK;
        synonym_chain_buckets[i].next_link_idx = NULL_LINK;
    }
}

void print_buckets() {
    printf(">>>>> Synonym chain buckets ([bucket_num]: key,)\n");
    printf(">> Main bucket => overflow buckets.\n");
    for (int i = 0; i < MAIN_BUCKET_SIZE; i++) {
        if (synonym_chain_buckets[i].key > EMPTY_BUCKET) {
            if (synonym_chain_buckets[i].next_link_idx == NULL_LINK) {
                printf("[%2d]: %d |", i, synonym_chain_buckets[i].key);
            }
            else {
                printf("[%2d]: %d |", i, synonym_chain_buckets[i].key);
                int next = i;
                while ((next = synonym_chain_buckets[next].next_link_idx) > -1) {
                    printf("=> [%2d]: %d, ", next, synonym_chain_buckets[next].key);
                }
            }
            printf("\n");
        }
        else {
            printf("[%2d]: E\n", i);
        }
    }
    printf("\n");
}

///////////////// UTIL FUNCTIONS - END
