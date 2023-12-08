#include "firstfit_sim.h"

std::list<allocation*>::const_iterator firstfit_sim::find_alloc(const std::list<allocation*>& free_chunks, std::size_t chunk_size) {
    //look for the first free chunk which can fit the new process
    auto found = free_chunks.end();
    for (auto it = free_chunks.begin(); it != free_chunks.end(); ++it) {
        if ((*it)->getTotalSize() >= chunk_size) {
            found = it;
            break;
        }
    }
    return found;
}