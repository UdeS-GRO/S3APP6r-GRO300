#include <vector>
#include <iostream>
#include <array>

namespace vm
{
    extern std::array<float, 4> registers_;
};

enum Instruction
{
    ADD,
    SUB,
    MUL,
    DIV,
    LDA,
    STO,
    LDC
};

enum Register
{
    R1 = 0,
    R2,
    R3,
    R4
};

struct Expression
{
    virtual void run()
    {
        printf("NOP\n");
    }
};

struct Exp_2R: public Expression
{
    Exp_2R(Register r1, Register r2):
        r1_(r1), r2_(r2)
    {
    }

    Register r1_;
    Register r2_;
};

struct Exp_ADD: public Exp_2R
{
    Exp_ADD(Register r1, Register r2): Exp_2R(r1, r2)
    {}

    void run()
    {
        using namespace vm;
        registers_[r1_] = registers_[r1_] + registers_[r2_];
    }
};

struct Exp_SUB: public Exp_2R
{
    Exp_SUB(Register r1, Register r2): Exp_2R(r1, r2)
    {}

    void run()
    {
        using namespace vm;
        registers_[r1_] = registers_[r1_] - registers_[r2_];
    }
};

struct Exp_MUL: public Exp_2R
{
    Exp_MUL(Register r1, Register r2): Exp_2R(r1, r2)
    {}

    void run()
    {
        using namespace vm;
        registers_[r1_] = registers_[r1_] * registers_[r2_];
    }
};

struct Exp_DIV: public Exp_2R
{
    Exp_DIV(Register r1, Register r2): Exp_2R(r1, r2)
    {}

    void run()
    {
        using namespace vm;
        registers_[r1_] = registers_[r1_] / registers_[r2_];
    }
};

typedef std::vector<Expression*> Expressions;

Expressions& prog(); // returns a pointer to the current program (expressions).

