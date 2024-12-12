/**
 * File Name: cache.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef CACHE_H
#define CACHE_H

#include <stdint.h>
#include "../include/memory.h"

typedef struct cache_line {
    uint64_t tag;
    uint8_t *block;
    uint64_t valid;
    uint64_t dirty;
    uint64_t last_used;
    uint64_t arrival_time;
} cache_line_s;


typedef struct cache_struct {
    _Bool enabled;                  

    uint64_t cache_size;            
    uint64_t block_size;            
    uint64_t associativity;         
    char replacement_policy[10];    
    char write_policy[10];          

    uint64_t num_sets;              
    uint64_t num_lines;
    uint64_t index_bits;            
    uint64_t byte_offset_bits;      
    uint64_t tag_bits;

    cache_line_s **lines;           
    cache_line_s **sets;           

    uint64_t access;                
    uint64_t hit;                   
    uint64_t miss;                 
    char *cache_output_file; 
} cache_s;



cache_s *init_cache();

void reset_cache(cache_s *cache, char *config_file);

void enable_cache(cache_s *cache, char *config_file, char *input_file);

void disable_cache(cache_s *cache);

void toggle_cache(cache_s *cache, char *output_file);

void print_status(cache_s *cache);

void invalidate(cache_s *cache);

void cache_dump(cache_s *cache, char *dump_file);

void cache_statistics(cache_s *cache);

uint8_t *access_cache(cache_s *cache, uint64_t address, memory_s *memory, char *filename);

void write_cache(cache_s *cache, uint64_t address, uint8_t *data, memory_s *memory, uint64_t bytes, char *filename);

void clear_wb_cache(cache_s *cache, memory_s *memory);

void print_cache_config(cache_s *cache);

void print_meta_data(cache_s *cache);

void print_cache(cache_s *cache);

void free_cache(cache_s *cache);



#endif // CACHE_H