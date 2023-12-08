#ifndef ALLOCATION_H
#define ALLOCATION_H
#include <iostream>
#include <sstream>
#include <stdexcept>
#define INITIAL_SIZE 0
class allocation {
    private:
        // the first byte address of the allocation
        void* address;
        // the total size of the allocation (should be in powers of 2)
        std::size_t total_size;
        // the number of bytes that is actually occupied by a process
        std::size_t used_size;
    public:
        // getters and setters
        void* getAddress() const;
        std::size_t getTotalSize() const;
        std::size_t getUsedSize() const;
        void setUsedSize(std::size_t size);

        // constructors
        allocation(void* address, std::size_t total_size);
        allocation(void* address, std::size_t total_size, std::size_t used_size);

        // printing for occupied list to also show used_size
        std::string toFullString() const;
        // printing for free list to not show used_size
        std::string toAddressTotalSizeString() const;

        // print
        friend std::ostream& operator<< (std::ostream& os, const allocation& a);
};
#endif