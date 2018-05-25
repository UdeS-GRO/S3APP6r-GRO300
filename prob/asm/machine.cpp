#include "compiler.hpp"

int main(int argc, char** argv)
{
    using namespace vm;

    std::string fname;

    if (argc >= 2) {
        fname = std::string(argv[1]);
    }

    Compiler c(fname);

    c.run();

    return 0;
}

