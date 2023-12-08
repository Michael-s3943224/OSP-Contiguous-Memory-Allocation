#ifndef BETTER_BESTFIT_SIM_H
#define BETTER_BESTFIT_SIM_H
#include "frag_sim.h"
#include <bit>
// the class for better best fit
// inherit from frag sim
class better_bestfit_sim: public frag_sim {
    // override the find_alloc method
    virtual std::list<allocation*>::const_iterator find_alloc(const std::list<allocation*>& free_chunks, std::size_t chunk_size);
};
#endif