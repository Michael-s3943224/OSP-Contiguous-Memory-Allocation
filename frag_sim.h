#ifndef FRAG_SIM_H
#define FRAG_SIM_H
#include <iostream>
#include <list>
#include <unistd.h>
#include <string.h>
#include <stdexcept>
#include <fstream>
#include <cmath>
#include <numeric>
#include <chrono>
#include <tuple> //for the std::ignore
#include <bit> //for bit ceil
#include <iterator>
#include "allocation.h"

#define INITIAL_HEAP_FRONTIER_OFFSET 0
#define ALLOC_PREFIX "alloc: "
#define DEALLOC_PREFIX "dealloc"
#define MIN_BYTES 1UL
#define MAX_BYTES 512UL
#define BRK_SUCCESS 0
#define SBRK_FAIL -1
#define MIN_CHUNK_SIZE 32UL

class frag_sim {
    protected: 
        // method to be implemented by super class
        // takes in the free_chunks as a parameter so I can make free_chunks private
        // so that the super class cannot modify free_chunks in an unpredictable way
        virtual std::list<allocation*>::const_iterator find_alloc(const std::list<allocation*>& free_chunks, std::size_t chunk_size) = 0;
    private:
        // the list to store the occupied chunks
        std::list<allocation*> occupied_chunks;
        // the list to store the free chunks
        std::list<allocation*> free_chunks;
        // this will be call find_alloc which is implemented by the super class
        void* alloc(std::size_t chunk_size);
        // dealloc the chunk based on the address
        void dealloc(void* address);
        // load the file and insert processes into either the occupied or free chunks
        void load_file(const std::string& fileName);
        // process line from load_file
        void process_file_line(const std::string& line);
        // is called when load_file reaches "delloc" in the datafile
        // exists to make load_file method shorter (because of all the damn error checking)
        void do_delloc_cmd();
        // to get the byte number from the "alloc:" line
        static std::size_t get_alloc_cmd_value(const std::string& s);
        // is called when load_file reaches "alloc:" in the datafile
        // exists to make load_file method shorter (because of all the damn error checking)
        void do_alloc_cmd(std::size_t size);
        // print the lists
        void print_lists();
        // print memory stats
        void print_memory_stats();
        // free all memory and clears all lists
        void cleanup();
    public:
        // constructor
        frag_sim();
        // run the entire simulation
        void run(const std::string& fileName);
};
#endif