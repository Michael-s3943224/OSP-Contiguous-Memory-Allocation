#include "allocation.h"

// constructors
allocation::allocation(void* address, std::size_t total_size): 
    address(address), total_size(total_size), used_size(INITIAL_SIZE) {

}

allocation::allocation(void* address, std::size_t total_size, std::size_t used_size):
    address(address), total_size(total_size), used_size(used_size) {

}

// to string conversions
std::string allocation::toAddressTotalSizeString() const {
    //I don't know how to cast my pointers to a string
    //so I'm using this stringstream class to do it for me
    //don't know about performance, tho....
    std::stringstream ss;
    ss << "address: " << this->address << " total size: " << this->total_size;
    return ss.str();
}

std::string allocation::toFullString() const {
    return toAddressTotalSizeString() + " used size: " + std::to_string(this->used_size);
}

// print
std::ostream& operator<<(std::ostream& os, const allocation& a) {
    std::cout << a.toFullString();
    return os;
}

// getters and setters
void* allocation::getAddress() const {
    return address;
}

std::size_t allocation::getTotalSize() const {
    return total_size;
}

std::size_t allocation::getUsedSize() const {
    return used_size;
}

void allocation::setUsedSize(std::size_t size) {
    // enforce the restriction
    if (size > total_size) {
        std::string error = "ERROR: cannot set used size to be larger than total size";
        throw std::runtime_error(error);
    }
    used_size = size;
}