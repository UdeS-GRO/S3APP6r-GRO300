#include <cstdio>
#include "ast.hpp"
#include "parser.hpp"

extern int yyparse();

namespace vm
{
    std::array<float, 4> registers_;
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

    registers_[R1] = 1.0;
    registers_[R2] = 1.0;

    const Expressions& program = prog();
    for (Expressions::const_iterator exp = program.begin();
         exp != program.end();
         ++exp) {
        Expression* e = *exp;
        e->run();
    }
    printf("Registers: \n");
    for (int i = 0; i < 4; ++i) {
        printf("R%d: %f\n", i + 1, registers_[i]);
    }
}
