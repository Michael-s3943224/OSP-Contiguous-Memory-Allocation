#ifndef FIRSTFIT_SIM_H
#define FIRSTFIT_SIM_H
#include "frag_sim.h"
// the class for first fit
// inherit from frag sim
class firstfit_sim: public frag_sim {
    // override the find_alloc method
    virtual std::list<allocation*>::const_iterator find_alloc(const std::list<allocation*>& free_chunks, std::size_t chunk_size);
};
#endif