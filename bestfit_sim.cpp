#include "bestfit_sim.h"

std::list<allocation*>::const_iterator bestfit_sim::find_alloc(const std::list<allocation*>& free_chunks, std::size_t chunk_size) {
    //look for the free chunk with the smallest 
    auto smallestfit = free_chunks.end();
    for (auto it = free_chunks.begin(); it != free_chunks.end(); ++it) {
        //get the free chunk pointer
        allocation* fPointer = *it;
        //make sure the free chunk is bigger than the chunk size before considering
        if (fPointer->getTotalSize() >= chunk_size &&
            // if the current free chunk is a better fit than the current bestfit, replace it
            (smallestfit == free_chunks.end() || fPointer->getTotalSize() < (*smallestfit)->getTotalSize())) {
                smallestfit = it;
        }
    }
    return smallestfit;
}