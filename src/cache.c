/**
 * File Name: cache.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */

#include "../include/cache.h"
#include "../include/memory.h"
#include "../include/utils.h"

#include <math.h>

cache_s *init_cache() {
    cache_s *cache = malloc(sizeof(cache_s));
    cache->enabled = 0;
    cache->cache_size = 0;
    cache->block_size = 0;
    cache->associativity = 0;
    strcpy(cache->replacement_policy, "");
    strcpy(cache->write_policy, "");
    cache->num_sets = 0;
    cache->num_lines = 0;
    cache->index_bits = 0;
    cache->byte_offset_bits = 0;
    cache->tag_bits = 0;
    cache->lines = NULL;
    cache->sets = NULL;
    cache->access = 0;
    cache->hit = 0;
    cache->miss = 0;
    cache->cache_output_file = NULL;
    return cache;
}

void reset_cache(cache_s *cache, char *config_file) {
    if (cache->enabled) {
        free_cache(cache);
    }
    cache = init_cache(config_file);
    
}

void enable_cache(cache_s *cache, char *config_file, char *input_file) {
    cache->enabled = 1;
    FILE *file = fopen(config_file, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", config_file);
        return;
    }
    if (fscanf(file, "%lu", &cache->cache_size) != 1) {
        perror("Error reading cache size");
        fclose(file);
        return;
    }
    if (fscanf(file, "%lu", &cache->block_size) != 1) {
        perror("Error reading block size");
        fclose(file);
        return;
    }
    if (fscanf(file, "%lu", &cache->associativity) != 1) {
        perror("Error reading associativity");
        fclose(file);
        return;
    }
    if (fscanf(file, "%s", cache->replacement_policy) != 1) {
        perror("Error reading replacement policy");
        fclose(file);
        return;
    }
    if (fscanf(file, "%s", cache->write_policy) != 1) {
        perror("Error reading write policy");
        fclose(file);
        return;
    }
    fclose(file);

    cache->num_sets = cache->cache_size / (cache->block_size * cache->associativity);
    cache->num_lines = cache->cache_size / cache->block_size;
    cache->index_bits = log2(cache->num_sets);
    cache->byte_offset_bits = log2(cache->block_size);
    cache->tag_bits = 20 - cache->index_bits - cache->byte_offset_bits;

    cache->sets = malloc(cache->num_sets * sizeof(cache_line_s *));
    for (uint64_t i = 0; i < cache->num_sets; i++) {
        cache->sets[i] = malloc(cache->associativity * sizeof(cache_line_s));
    }
    if (cache->sets == NULL) {
        perror("Error allocating cache sets");
        return;
    }
    for (uint64_t i = 0; i < cache->num_sets; i++) {
        for (uint64_t j = 0; j < cache->associativity; j++) {
            cache->sets[i][j].valid = 0;
            cache->sets[i][j].dirty = 0;
            cache->sets[i][j].last_used = 0;
            cache->sets[i][j].arrival_time = 0;
            cache->sets[i][j].block = malloc(cache->block_size * sizeof(uint8_t));
        }
    }

    cache->access = 0;
    cache->hit = 0;
    cache->miss = 0;

    char *filename = NULL;

    {
        const char *base_name = strrchr(input_file, '/');
        base_name = (base_name) ? base_name + 1 : input_file; 

        size_t len = strlen(base_name);
        if (len > 2 && strcmp(base_name + len - 2, ".s") == 0) {
            len -= 2; 
        }

        filename = malloc(len + strlen(".output") + 1);
        if (filename == NULL) {
            perror("malloc");
            exit(1);
        }

        snprintf(filename, len + strlen(".output") + 1, "%.*s.output", (int)len, base_name);
    }

    


    cache->cache_output_file = filename;
}

void disable_cache(cache_s *cache) {
    cache->enabled = 0;
}

void toggle_cache(cache_s *cache, char *output_file) {
    if (cache->enabled) {
        disable_cache(cache);
    } else {
        enable_cache(cache, "cache.config", output_file);
    }
}

void print_status(cache_s *cache) {
    if (cache->enabled) {
        printf("Enabled\n");
        printf("Cache Size: %lu\n", cache->cache_size);
        printf("Block Size: %lu\n", cache->block_size);
        printf("Associativity: %lu\n", cache->associativity);
        printf("Replacement Policy: %s\n", cache->replacement_policy);
        printf("Write Policy: %s\n", cache->write_policy);
    } else {
        printf("Disabled\n");
    }
}

void invalidate(cache_s *cache) {
    for (uint64_t i = 0; i < cache->num_sets; i++) {
        for (uint64_t j = 0; j < cache->associativity; j++) {
            cache->sets[i][j].valid = 0;
            cache->sets[i][j].dirty = 0;
            cache->sets[i][j].last_used = 0;
            cache->sets[i][j].arrival_time = 0;
            cache->sets[i][j].tag = 0;
        }
    }
}

void cache_dump(cache_s *cache, char *dump_file) {
    FILE *file = fopen(dump_file, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", dump_file);
        return;
    }
    for (uint64_t i = 0; i < cache->num_sets; i++) {
        for (uint64_t j = 0; j < cache->associativity; j++) {
            if (cache->sets[i][j].valid) {
                if (strcmp(cache->write_policy, "WB") == 0) {
                    if (cache->sets[i][j].dirty) {
                        fprintf(file, "Set: 0x%lx, Tag: 0x%lx, %s\n", i, cache->sets[i][j].tag, "Dirty");
                    } else {
                        fprintf(file, "Set: 0x%lx, Tag: 0x%lx, %s\n", i, cache->sets[i][j].tag, "Clean");
                    }
                } else {
                    fprintf(file, "Set: 0x%lx, Tag: 0x%lx, %s\n", i, cache->sets[i][j].tag, "Clean");
                }
            }
        }
    }
    fclose(file);
}

void print_meta_data(cache_s *cache) {

    for (uint64_t i = 0; i < cache->num_sets; i++) {
        for (uint64_t j = 0; j < cache->associativity; j++) {
            if (cache->sets[i][j].valid) {
                if (strcmp(cache->write_policy, "WB") == 0) {
                    if (cache->sets[i][j].dirty) {
                        printf("Set: 0x%lx, Tag: 0x%lx, %s\n", i, cache->sets[i][j].tag, "Dirty");
                    } else {
                        printf("Set: 0x%lx, Tag: 0x%lx, %s\n", i, cache->sets[i][j].tag, "Clean");
                    }
                } else {
                    printf("Set: 0x%lx, Tag: 0x%lx\n", i, cache->sets[i][j].tag);
                }
            }
        }
    }
}

void cache_statistics(cache_s *cache) {
    printf("D-cache statistics: Accesses=%ld, Hit=%ld, Miss=%ld, Hit Rate=%f\n", cache->access, cache->hit, cache->miss, (float)cache->hit / (float)cache->access);
    return;
}

uint8_t *access_cache(cache_s *cache, uint64_t address, memory_s *memory, char *filename) {
    //uint64_t byte_offset = address & ((1 << cache->byte_offset_bits) - 1);
    uint64_t index = (address >> cache->byte_offset_bits) & ((1 << cache->index_bits) - 1);
    uint64_t tag = (address >> (cache->byte_offset_bits + cache->index_bits)) & ((1 << cache->tag_bits) - 1);

    index = index % cache->num_sets;
    cache->access++;

    // check if the block is in the cache, cache hit
    // assumed aligned access
    for (uint64_t i = 0; i < cache->associativity; i++) {
        if (cache->sets[index][i].valid && cache->sets[index][i].tag == tag) {
            cache->hit++;

            FILE *file = fopen(filename, "a");
            if (file == NULL) {
                printf("Error: Could not open file %s\n", filename);
                return NULL;
            }

            if (cache->sets[index][i].dirty) {
                fprintf(file, "R: Address: 0x%lx, Set: 0x%lx, Hit, Tag: 0x%lx, %s\n", address, index, tag, "Dirty");
            } else {
                fprintf(file, "R: Address: 0x%lx, Set: 0x%lx, Hit, Tag: 0x%lx, %s\n", address, index, tag, "Clean");
            }
            fclose(file);

            // update metadata if LRU, last used to current access
            if (strcmp(cache->replacement_policy, "LRU") == 0) {
                cache->sets[index][i].last_used = cache->access;
            }
            return cache->sets[index][i].block;
        }
    }

    cache->miss++;

    // cache miss
    // replacement policy
    // FIFO, LRU, RAND
    if (strcmp(cache->replacement_policy, "FIFO") == 0) {

        // find the first arrived block to replace
        uint64_t min = cache->sets[index][0].arrival_time;
        uint64_t first_set = 0;
        for (uint64_t i = 0; i < cache->associativity; i++) {
            if (cache->sets[index][i].arrival_time < min) {
                min = cache->sets[index][i].arrival_time;
                first_set = i;
            }
        }
        
        // write back the block if dirty, write policy WB
        // Byte by byte write back
        if (strcmp(cache->write_policy, "WB") == 0 && cache->sets[index][first_set].valid && cache->sets[index][first_set].dirty) {
            uint64_t dirty_address = (cache->sets[index][first_set].tag << (cache->index_bits + cache->byte_offset_bits)) | (index << cache->byte_offset_bits);
            for (uint64_t i = 0; i < cache->block_size; i++) {
                uint64_t value = cache->sets[index][first_set].block[i];
                write_memory(memory, dirty_address + i, value, 1);
            }
        }

        // set metadata for the new block
        cache->sets[index][first_set].valid = 1;
        cache->sets[index][first_set].dirty = 0;
        cache->sets[index][first_set].tag = tag;
        cache->sets[index][first_set].arrival_time = cache->access;

        // read the block from memory
        // Byte by byte read
        // read the memory block (address to address + block_size) bytes
        uint64_t address = (tag << (cache->index_bits + cache->byte_offset_bits)) | (index << cache->byte_offset_bits);
        for (uint64_t i = 0; i < cache->block_size; i++) {
            cache->sets[index][first_set].block[i] = read_memory(memory, address + i);
        }

        FILE *file = fopen(filename, "a");
        if (file == NULL) {
            printf("Error: Could not open file %s\n", filename);
            return NULL;
        }
        fprintf(file, "R: Address: 0x%lx, Set: 0x%lx, Miss, Tag: 0x%lx, %s\n", address, index, tag, "Clean");
        fclose(file);
        return cache->sets[index][first_set].block;

    } else if (strcmp(cache->replacement_policy, "LRU") == 0) {

        // find the least recently used block to replace
        uint64_t min = cache->sets[index][0].last_used;
        uint64_t lr_set = 0;
        for (uint64_t i = 1; i < cache->associativity; i++) {
            if (cache->sets[index][i].last_used < min) {
                min = cache->sets[index][i].last_used;
                lr_set = i;
            }
        }

        // write back the block if dirty, write policy WB
        // Byte by byte write back
        if (strcmp(cache->write_policy, "WB") == 0 && cache->sets[index][lr_set].valid && cache->sets[index][lr_set].dirty) {
            uint64_t dirty_address = (cache->sets[index][lr_set].tag << (cache->index_bits + cache->byte_offset_bits)) | (index << cache->byte_offset_bits);
            for (uint64_t i = 0; i < cache->block_size; i++) {
                uint64_t value = cache->sets[index][lr_set].block[i];
                write_memory(memory, dirty_address + i, value, 1);
            }
        }

        // set metadata for the new block
        cache->sets[index][lr_set].valid = 1;
        cache->sets[index][lr_set].dirty = 0;
        cache->sets[index][lr_set].tag = tag;
        cache->sets[index][lr_set].last_used = cache->access;

        // read the block from memory
        // Byte by byte read
        // read the memory block (address to address + block_size) bytes
        uint64_t address = (tag << (cache->index_bits + cache->byte_offset_bits)) | (index << cache->byte_offset_bits);
        for (uint64_t i = 0; i < cache->block_size; i++) {
            cache->sets[index][lr_set].block[i] = read_memory(memory, address + i);
        }

        FILE *file = fopen(filename, "a");
        if (file == NULL) {
            printf("Error: Could not open file %s\n", filename);
            return NULL;
        }
        fprintf(file, "R: Address: 0x%lx, Set: 0x%lx, Miss, Tag: 0x%lx, %s\n", address, index, tag, "Clean");
        fclose(file);


        return cache->sets[index][lr_set].block;

    } else if (strcmp(cache->replacement_policy, "RAND") == 0) {
        // random replacement policy
        uint64_t random_set = rand() % cache->associativity;
    
        // write back the block if dirty, write policy WB
        // Byte by byte write back
        if (strcmp(cache->write_policy, "WB") == 0 && cache->sets[index][random_set].valid && cache->sets[index][random_set].dirty) {
            uint64_t dirty_address = (cache->sets[index][random_set].tag << (cache->index_bits + cache->byte_offset_bits)) | (index << cache->byte_offset_bits);
            for (uint64_t i = 0; i < cache->block_size; i++) {
                uint64_t value = cache->sets[index][random_set].block[i];
                write_memory(memory, dirty_address + i, value, 1);
            }
        }

        // set metadata for the new block
        cache->sets[index][random_set].valid = 1;
        cache->sets[index][random_set].dirty = 0;
        cache->sets[index][random_set].tag = tag;

        // read the block from memory
        // Byte by byte read
        // read the memory block (address to address + block_size) bytes
        uint64_t address = (tag << (cache->index_bits + cache->byte_offset_bits)) | (index << cache->byte_offset_bits);
        for (uint64_t i = 0; i < cache->block_size; i++) {
            cache->sets[index][random_set].block[i] = read_memory(memory, address + i);
        }    

        FILE *file = fopen(filename, "a");
        if (file == NULL) {
            printf("Error: Could not open file %s\n", filename);
            return NULL;
        }
        fprintf(file, "R: Address: 0x%lx, Set: 0x%lx, Miss, Tag: 0x%lx, %s\n", address, index, tag, "Clean");
        fclose(file);

        return cache->sets[index][random_set].block;
    }
    return NULL;
}

void write_cache(cache_s *cache, uint64_t address, uint8_t *data, memory_s *memory, uint64_t bytes, char *filename) {
    uint64_t byte_offset = address & ((1 << cache->byte_offset_bits) - 1);
    uint64_t index = (address >> cache->byte_offset_bits) & ((1 << cache->index_bits) - 1);
    uint64_t tag = (address >> (cache->byte_offset_bits + cache->index_bits)) & ((1 << cache->tag_bits) - 1);

    index = index % cache->num_sets;
    cache->access++;
    // check if the block is in the cache, cache hit
    for (uint64_t i = 0; i < cache->associativity; i++) {
        if (cache->sets[index][i].valid && cache->sets[index][i].tag == tag) {
            cache->hit++;

            if (strcmp(cache->write_policy, "WT") == 0) {
                // write the data to memory
                uint64_t value = 0;
                for (uint64_t j = 0; j < bytes; j++) {
                    value |= data[j] << (j * 8);
                }
                write_memory(memory, address, value, bytes);
            } 
            else if (strcmp(cache->write_policy, "WB") == 0) {
                // write the data to cache
                // from byte_offset to byte_offset + bytes
                cache->sets[index][i].dirty = 1;
                for (uint64_t j = 0; j < bytes; j++) {
                    cache->sets[index][i].block[byte_offset + j] = data[j];
                }
            }

            // update metadata if LRU, last used to current access
            if (strcmp(cache->replacement_policy, "LRU") == 0) {
                cache->sets[index][i].last_used = cache->access;
            }

            FILE *file = fopen(filename, "a");
            if (file == NULL) {
                printf("Error: Could not open file %s\n", filename);
                return;
            }

            if (cache->sets[index][i].dirty) {
                fprintf(file, "W: Address: 0x%lx, Set: 0x%lx, Hit, Tag: 0x%lx, %s\n", address, index, tag, "Dirty");
            } else {
                fprintf(file, "W: Address: 0x%lx, Set: 0x%lx, Hit, Tag: 0x%lx, %s\n", address, index, tag, "Clean");
            }

            fclose(file);

            return;
        }
    }

    cache->miss++;
    if (strcmp(cache->replacement_policy, "FIFO") == 0) {
        // find the first arrived block to replace
        uint64_t min = cache->sets[index][0].arrival_time;
        uint64_t first_set = 0;
        for (uint64_t i = 0; i < cache->associativity; i++) {
            if (cache->sets[index][i].arrival_time < min) {
                min = cache->sets[index][i].arrival_time;
                first_set = i;
            }
        }

        if (strcmp(cache->write_policy, "WB") == 0) {
            // write back the block if dirty
            if (cache->sets[index][first_set].valid && cache->sets[index][first_set].dirty) {
                uint64_t block_address = (cache->sets[index][first_set].tag << (cache->index_bits + cache->byte_offset_bits)) | (index << cache->byte_offset_bits);
                for (uint64_t i = 0; i < cache->block_size; i++) {
                    uint64_t value = cache->sets[index][first_set].block[i];
                    write_memory(memory, block_address + i, value, 1);
                }
            }

            cache->sets[index][first_set].valid = 1;
            cache->sets[index][first_set].tag = tag;
            cache->sets[index][first_set].arrival_time = cache->access;
            cache->sets[index][first_set].dirty = 1;


            // fetch the block from memory
            uint64_t block_address = (tag << (cache->index_bits + cache->byte_offset_bits)) | (index << cache->byte_offset_bits);
            for (uint64_t i = 0; i < cache->block_size; i++) {
                cache->sets[index][first_set].block[i] = read_memory(memory, block_address + i);
            }
            for (uint64_t j = 0; j < bytes; j++) {
                cache->sets[index][first_set].block[byte_offset + j] = data[j];
            }
            
        } 
        else if (strcmp(cache->write_policy, "WT") == 0) {
            // write the block to memory
            uint64_t value = 0;
            for (uint64_t j = 0; j < bytes; j++) {
                value |= data[j] << (j * 8);
            }
            write_memory(memory, address, value, bytes);
        }
        
        FILE *file = fopen(filename, "a");
        if (file == NULL) {
            printf("Error: Could not open file %s\n", filename);
            return;
        }
        fprintf(file, "W: Address: 0x%lx, Set: 0x%lx, Miss, Tag: 0x%lx, %s\n", address, index, tag, "Dirty");
        fclose(file);

    } else if (strcmp(cache->replacement_policy, "LRU") == 0) {
        // find the least recently used block to replace
        uint64_t min = cache->sets[index][0].last_used;
        uint64_t lr_set = 0;
        for (uint64_t i = 1; i < cache->associativity; i++) {
            if (cache->sets[index][i].last_used < min) {
                min = cache->sets[index][i].last_used;
                lr_set = i;
            }
        }

        if (strcmp(cache->write_policy, "WB") == 0) {
            // write back the block if dirty
            if (cache->sets[index][lr_set].valid && cache->sets[index][lr_set].dirty) {
                uint64_t dirty_address = (cache->sets[index][lr_set].tag << (cache->index_bits + cache->byte_offset_bits)) | (index << cache->byte_offset_bits);
                for (uint64_t i = 0; i < cache->block_size; i++) {
                    uint64_t value = cache->sets[index][lr_set].block[i];
                    write_memory(memory, dirty_address + i, value, 1);
                }
            }

            cache->sets[index][lr_set].valid = 1;
            cache->sets[index][lr_set].tag = tag;
            cache->sets[index][lr_set].last_used = cache->access;
            cache->sets[index][lr_set].dirty = 1;

            // fetch the block from memory
            uint64_t address = (tag << (cache->index_bits + cache->byte_offset_bits)) | (index << cache->byte_offset_bits);
            for (uint64_t i = 0; i < cache->block_size; i++) {
                cache->sets[index][lr_set].block[i] = read_memory(memory, address + i);
            }

            for (uint64_t j = 0; j < bytes; j++) {
                cache->sets[index][lr_set].block[byte_offset + j] = data[j];
            }

        } 
        else if (strcmp(cache->write_policy, "WT") == 0) {
            // write the block to memory
            uint64_t value = 0;
            for (uint64_t j = 0; j < bytes; j++) {
                value |= data[j] << (j * 8);
            }
            write_memory(memory, address, value, bytes);
        } 


        FILE *file = fopen(filename, "a");
        if (file == NULL) {
            printf("Error: Could not open file %s\n", filename);
            return;
        }
        fprintf(file, "W: Address: 0x%lx, Set: 0x%lx, Miss, Tag: 0x%lx, %s\n", address, index, tag, "Dirty");
        fclose(file);

    } else if (strcmp(cache->replacement_policy, "RAND") == 0) {
        uint64_t random_index = rand() % cache->associativity;

        if (strcmp(cache->write_policy, "WB") == 0) {
            // write back the block if dirty
            if (cache->sets[index][random_index].dirty) {
                uint64_t dirty_address = (cache->sets[index][random_index].tag << (cache->index_bits + cache->byte_offset_bits)) | (index << cache->byte_offset_bits);
                for (uint64_t i = 0; i < cache->block_size; i++) {
                    uint64_t value = cache->sets[index][random_index].block[i];
                    write_memory(memory, dirty_address + i, value, 1);
                }
            }
            cache->sets[index][random_index].valid = 1;
            cache->sets[index][random_index].tag = tag;
            cache->sets[index][random_index].last_used = cache->access;
            cache->sets[index][random_index].dirty = 1;

            // write the value to memory
            uint64_t value = 0;
            for (uint64_t j = 0; j < bytes; j++) {
                value |= data[j] << (j * 8);
            }
            write_memory(memory, address, value, bytes);


            // fetch the block from memory
            uint64_t address = (tag << (cache->index_bits + cache->byte_offset_bits)) | (index << cache->byte_offset_bits);
            for (uint64_t i = 0; i < cache->block_size; i++) {
                cache->sets[index][random_index].block[i] = read_memory(memory, address + i);
            }


            for (uint64_t j = 0; j < bytes; j++) {
                cache->sets[index][random_index].block[byte_offset + j] = data[j];
            }
        } 
        else if (strcmp(cache->write_policy, "WT") == 0) {
            // write the block to memory
            uint64_t value = 0;
            for (uint64_t j = 0; j < bytes; j++) {
                value |= data[j] << (j * 8);
            }

            write_memory(memory, address, value, bytes);
        } 

        FILE *file = fopen(filename, "a");
        if (file == NULL) {
            printf("Error: Could not open file %s\n", filename);
            return;
        }
        fprintf(file, "W: Address: 0x%lx, Set: 0x%lx, Miss, Tag: 0x%lx, %s\n", address, index, tag, "Dirty");
        fclose(file);
    }
    return;
}

void clear_wb_cache(cache_s *cache, memory_s *memory) {
    for (uint64_t i = 0; i < cache->num_sets; i++) {
        for (uint64_t j = 0; j < cache->associativity; j++) {
            if (cache->sets[i][j].valid && cache->sets[i][j].dirty) {
                uint64_t dirty_address = (cache->sets[i][j].tag << (cache->index_bits + cache->byte_offset_bits)) | (i << cache->byte_offset_bits);
                for (uint64_t k = 0; k < cache->block_size; k++) {
                    uint64_t value = cache->sets[i][j].block[k];
                    write_memory(memory, dirty_address + k, value, 1);
                }
            }
        }
    }
}

void print_cache(cache_s *cache) {
    for (uint64_t i = 0; i < cache->num_sets; i++) {
        for (uint64_t j = 0; j < cache->associativity; j++) {
            if (cache->sets[i][j].valid) {
                
                printf("Set: 0x%lx, Tag: 0x%lx, Valid: %ld, Dirty: %ld, Last Used: %ld, Arrival Time: %ld, ", 
                        i, 
                        cache->sets[i][j].tag, 
                        cache->sets[i][j].valid, 
                        cache->sets[i][j].dirty, 
                        cache->sets[i][j].last_used, 
                        cache->sets[i][j].arrival_time);
                printf("Block: ");
                for (uint64_t k = 0; k < cache->block_size; k++) {
                    printf("%02x ", cache->sets[i][j].block[k]);
                }
                printf("\n");
            }
        }
    }
}

void print_cache_config(cache_s *cache) {
    printf("Cache Size: %lu\n", cache->cache_size);
    printf("Block Size: %lu\n", cache->block_size);
    printf("Associativity: %lu\n", cache->associativity);
    printf("Replacement Policy: %s\n", cache->replacement_policy);
    printf("Write Policy: %s\n", cache->write_policy);
    printf("<==============================================================>\n");
    printf("Number of Sets: %lu\n", cache->num_sets);
    printf("Number of Lines: %lu\n", cache->num_lines);
    printf("Index Bits: %lu\n", cache->index_bits);
    printf("Byte Offset Bits: %lu\n", cache->byte_offset_bits);
    printf("Tag Bits: %lu\n", cache->tag_bits);
    printf("<==============================================================>\n");
    printf("Cache Status: %s\n", cache->enabled ? "Enabled" : "Disabled");
    printf("Cache Accesses: %lu\n", cache->access);
    printf("Cache Hits: %lu\n", cache->hit);
    printf("Cache Misses: %lu\n", cache->miss);
    printf("Cache Hit Rate: %f\n", (float)cache->hit / (float)cache->access);
    printf("<==============================================================>\n");
    return;
}

void free_cache(cache_s *cache) {
    (void)cache;
    return;
}


