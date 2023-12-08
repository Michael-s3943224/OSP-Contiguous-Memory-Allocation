#include <iostream>
#include "bestfit_sim.h"
#include <stdexcept>
#include <unistd.h> 

#define DATAFILE_INDEX 1
#define CMD_ARG_LEN 2

// driver code for best fit algorithm
int main(int argc, char** argv) {
    // make sure the number of command line arguments is correct
    if (argc != CMD_ARG_LEN) {
        std::string error = "ERROR: the command is ./bestfit <datafile>";
        std::cout << error << std::endl;
        return EXIT_FAILURE;
    }

    // extract the datafile name
    std::string datafile = argv[DATAFILE_INDEX];

    // create instance of the bestfit algorithm
    bestfit_sim bestfit;

    // run the bestfit algorithm
    try{
        bestfit.run(datafile);
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // successfully return
    return EXIT_SUCCESS;
}