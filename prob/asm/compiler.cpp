#include <cstdio>
#include "ast.hpp"
#include "parser.hpp"

extern int yyparse();

namespace vm
{
    Registers   CPU::registers;
    Memory      CPU::memory;
}

Expressions& prog()
{
    static Expressions prog_;

    return prog_;
}

int main(int argc, char** argv)
{
    using namespace vm;
    
    yyparse();

    const Expressions& program = prog();
    for (Expressions::const_iterator exp = program.begin();
         exp != program.end();
         ++exp) {
        Expression* e = *exp;
        e->run();
    }
    printf("Registers: \n");
    for (int i = 0; i < 4; ++i) {
        printf("R%d: %f\n", i + 1, CPU::registers[i]);
    }
}
