#include "frag_sim.h"

frag_sim::frag_sim() {

}

void frag_sim::cleanup() {
    //free all allocations objects in free chunks
    for (const allocation* a : free_chunks) {
        delete a;
    }
    //free all allocations objects in occupied chunks
    for (const allocation* a : occupied_chunks) {
        delete a;
    }
    free_chunks.clear();
    occupied_chunks.clear();
}

void frag_sim::print_lists() {
    // print the free chunks
    std::cout << "---=== FREE CHUNKS ===---" << std::endl;
    for (const allocation* a : free_chunks) {
        std::cout << a->toAddressTotalSizeString() << std::endl;
    }
    if (free_chunks.empty()) {
        std::cout << "\tEMPTY" << std::endl;
    }
    // print the occupied chunks
    std::cout << "---=== OCCUPIED CHUNKS ===---" << std::endl;
    for (const allocation* a : occupied_chunks) {
        std::cout << a->toFullString() << std::endl;
    }
    if (occupied_chunks.empty()) {
        std::cout << "\tEMPTY" << std::endl;
    }
}

void frag_sim::print_memory_stats() {
    // the initial value for the accumlate function
    std::size_t initial_size = 0;

    // get the total number of bytes in the free_chunks list
    std::size_t free_chunks_bytes = std::accumulate(free_chunks.begin(), free_chunks.end(), initial_size, 
        [](const std::size_t accum, const allocation* a){ return accum + a->getTotalSize(); });
    // get the total number of bytes in the occupied_chunks list
    std::size_t occupied_chunks_bytes = std::accumulate(occupied_chunks.begin(), occupied_chunks.end(), initial_size, 
        [](const std::size_t accum, const allocation* a){ return accum + a->getTotalSize(); });
    // get the total number of bytes in both lists
    std::size_t total_bytes = free_chunks_bytes + occupied_chunks_bytes;

    // get the total number of used bytes in occupied_chunks list
    std::size_t occupied_used_bytes = std::accumulate(occupied_chunks.begin(), occupied_chunks.end(), initial_size, 
        [](const std::size_t accum, const allocation* a){ return accum + a->getUsedSize(); });
    // get the total number of unused bytes in occupied_chunks list
    std::size_t occupied_unused_bytes = occupied_chunks_bytes - occupied_used_bytes;

    //get the total unused bytes
    std::size_t total_unused_bytes = free_chunks_bytes + occupied_unused_bytes;

    // print the results
    std::cout << "---=== MEMORY STATS ===---" << std::endl;
    std::cout << "free chunks bytes: " << free_chunks_bytes << std::endl;
    std::cout << "occupied chunks bytes: " << occupied_chunks_bytes << std::endl;
    std::cout << "total allocated bytes: " << total_bytes << std::endl;
    std::cout << "occupied used bytes: " << occupied_used_bytes << std::endl;
    std::cout << "occupied unused bytes: " << occupied_unused_bytes << std::endl;
    std::cout << "total unused bytes: " << total_unused_bytes << std::endl;
}

void* frag_sim::alloc(std::size_t chunk_size) {
    // find the allocation iterator
    // this method is implemented by the superclass
    // ASSUMPTION: find_alloc implemented by the superclass can be trusted
    auto allocIt = find_alloc(free_chunks, chunk_size);

    //check if the chunk size if between a range
    if (chunk_size < MIN_BYTES || chunk_size > MAX_BYTES) {
        std::string error = "Number of bytes must be between " + std::to_string(MIN_BYTES) + " and " + std::to_string(MAX_BYTES);
        throw std::runtime_error(error);
    }

    //if there we found a chunk that can fit the new process
    // set the used size to the process size
    // update the lists
    // and return the address of the chunk
    if (allocIt != free_chunks.end()) {
        allocation* alloc = *allocIt;
        alloc->setUsedSize(chunk_size);
        free_chunks.erase(allocIt);
        occupied_chunks.push_back(alloc);
        //return the address
        return alloc->getAddress();
    }

    //else we're going to add the newly created occupied chunk
    //round the chunk size to the nearest pow 2 (ceiling)
    //then clamp it to MIN_CHUNK_SIZE in case its still lower than the minimum
    std::size_t roundedChunkSize = std::max(MIN_CHUNK_SIZE, std::bit_ceil(chunk_size));

    //move the heap frontier
    void* newHeapFrontier = sbrk(roundedChunkSize);
    //check if the sbrk call failed
    //I was getting segfaults because I wasn't checking this????
    //nope the segfaults are still happening at the end of the program with large number of lines for the input datafile???? WTF????
    if (newHeapFrontier == (void*)SBRK_FAIL) {
        std::string error = "sbrk call failed";
        throw std::runtime_error(error);
    }

    // allocate and push back the new chunk
    allocation* newChunk = new allocation(newHeapFrontier, roundedChunkSize, chunk_size);
    occupied_chunks.push_back(newChunk);

    //return the address
    return newHeapFrontier;
}

void frag_sim::dealloc(void* address) {
    //find the memory chunk
    std::list<allocation*>::iterator foundIt = occupied_chunks.end();
    for (auto it = occupied_chunks.begin(); it != occupied_chunks.end(); ++it) {
        if ((*it)->getAddress() == address) {
            foundIt = it;
            break;
        }
    }
    //if we can't find the memory chunk, throw an error
    if (foundIt == occupied_chunks.end()) {
        std::string error = "Cannot find chunk at address";
        throw std::runtime_error(error);
    }

    // get the found pointer
    allocation* found = *foundIt;
    //set the used number of bytes to 0
    found->setUsedSize(INITIAL_SIZE);
    //add allocation to free chunks
    free_chunks.push_back(found);
    //remove allocation from occupied chunks
    occupied_chunks.remove(found);

    //NOTE: we shouldn't call brk here, because we might need to reuse the chunk for later
}

void frag_sim::do_delloc_cmd() {
    // make sure we actually have memory to deallocate
    if (occupied_chunks.empty()) {
        std::string error = "Cannot deallocate memory when no memory has been allocated";
        throw std::runtime_error(error);
    }
    dealloc(occupied_chunks.back()->getAddress());
}

std::size_t frag_sim::get_alloc_cmd_value(const std::string& line) {
    // extract the string that represent the number of bytes for the process
    std::size_t N = strlen(ALLOC_PREFIX);
    std::string byteNumStr = line.substr(N, line.length() - N);

    // try parse the byte number
    std::size_t byteNum {};
    try {
        byteNum = std::stoul(byteNumStr);
    } catch (const std::exception& e) {
        // if the parsing fails, throw an error with my own custom error message
        std::string error = "Failed to parse the number of bytes for alloc";
        throw std::runtime_error(error);
    }
    return byteNum;
}

void frag_sim::do_alloc_cmd(std::size_t size) {
    // there's only one method call in here
    // for consistency I guess?
    alloc(size);
}

void frag_sim::process_file_line(const std::string& line) {
    // read deallocate command
    if (line.starts_with(DEALLOC_PREFIX)) {
        do_delloc_cmd();
    // read allocate command
    } else if (line.starts_with(ALLOC_PREFIX)) {
        std::size_t byteNum = get_alloc_cmd_value(line);
        do_alloc_cmd(byteNum);
    // read a nonexisting command
    } else {
        std::string error = "Invalid command!";
        throw std::runtime_error(error);
    }
}

void frag_sim::load_file(const std::string& fileName) {
    // load the file
    std::ifstream file = std::ifstream(fileName, std::ifstream::binary | std::ifstream::in);
    // if the file doesn't exist, throw an error
    if (!file) {
        std::string error = "ERROR: File can not be found";
        throw std::runtime_error(error);
    }

    //read the file line by line
    std::string line;
    int firstLineNum = 1;
    for (int lineNum = firstLineNum; std::getline(file, line); ++lineNum) {
        // another awkward try catch block
        try {
            process_file_line(line);
            /*print_lists();
            std::cout << std::endl;*/
        } catch (const std::exception& e) {
            // make sure we actually close the file even if an exception is thrown
            file.close();
            // then throw the error
            std::string error = "ERROR: at file line " + std::to_string(lineNum) + ": " + e.what();
            throw std::runtime_error(error);
        }
    }

    // don't forget to close the file!
    file.close();
}

void frag_sim::run(const std::string& fileName) {
    // find the initial heap frontier
    void* initialHeapFrontier = sbrk(INITIAL_HEAP_FRONTIER_OFFSET);
    // check if the initial sbrk call didn't fail
    if (initialHeapFrontier == (void*)SBRK_FAIL) {
        std::string error = "ERROR: Initial sbrk call failed";
        throw std::runtime_error(error);
    }

    // start the timer
    auto start = std::chrono::high_resolution_clock::now();
    // another awkward try catch
    try {
        // load the file and insert into occupied/free chunks
        load_file(fileName);
    } catch (const std::runtime_error& e) {
        // make sure we actually reset the heap frontier even if an exception is thrown
        // std::ignore is a hack so I can get around the compiler warning of not using the return value
        std::ignore = brk(initialHeapFrontier);
        throw e;
    }
    // end the timer
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::milliseconds>(end - start);

    // ---=== PRINT RESULTS ===---
    // print out the lists
    print_lists();
    // print out the memory stats
    print_memory_stats();
    // print out the time taken
    std::cout << "---=== TIME TAKEN ===---" << std::endl;
    std::cout << "total time: " << duration.count() << " ms" << std::endl;

    // cleanup before brk call in case allocation objects collide with processes in heap?
    // also need to make sure I clear the vectors since the elements are also stored in the heap
    // this is because if I reset the heap frontier and there are still objects beyond it, I can no longer access it?
    // for some reason this now removes the segfault when my program ends
    // but the real question is: why didn't valgrind report any memory leaks?
    cleanup();

    //reset the heap frontier
    int status = brk(initialHeapFrontier);
    // check if the brk command was successful
    if (status != BRK_SUCCESS) {
        std::string error = "ERROR: Final brk call to move back the Heap frontier failed";
        throw std::runtime_error(error);
    }
}