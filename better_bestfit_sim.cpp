#include "better_bestfit_sim.h"

std::list<allocation*>::const_iterator better_bestfit_sim::find_alloc(const std::list<allocation*>& free_chunks, std::size_t chunk_size) {
    auto smallestfit = free_chunks.end();
    std::size_t optimalBestfit = std::max(MIN_CHUNK_SIZE, std::bit_ceil(chunk_size));

    for (auto it = free_chunks.begin(); it != free_chunks.end(); ++it) {
        //get the free chunk pointer
        allocation* fPointer = *it;
        //make sure the free chunk is bigger than the chunk size before considering
        if (fPointer->getTotalSize() >= chunk_size &&
            // if the current free chunk is a better fit than the current bestfit, replace it
            (smallestfit == free_chunks.end() || fPointer->getTotalSize() < (*smallestfit)->getTotalSize())) {
            
            smallestfit = it;

            //if we found the optimal best fit, then we can just break out of the loop
            if (fPointer->getTotalSize() == optimalBestfit) {
                break;
            }

        }
    }

    return smallestfit;
}