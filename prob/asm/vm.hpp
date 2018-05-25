#ifndef VM_HPP
#define VM_HPP

#include <array>
#include <unordered_map>
#include <vector>

namespace vm
{
    typedef std::array<float, 4>                    Registers;
    typedef std::unordered_map<std::string, float>  Memory;

    struct CPU
    {
        static Registers   registers;
        static Memory      memory;
    };

}


#endif 
