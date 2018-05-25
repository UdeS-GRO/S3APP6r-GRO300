#include <vector>
#include <iostream>
#include <array>
#include <unordered_map>

namespace vm
{
    typedef std::array<float, 4>                    Registers;
    typedef std::unordered_map<std::string, float>  Memory;

    struct CPU
    {
        static Registers    registers;
        static Memory       memory;
    };
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
        CPU::registers[r1_] = CPU::registers[r1_] + CPU::registers[r2_];
    }
};

struct Exp_SUB: public Exp_2R
{
    Exp_SUB(Register r1, Register r2): Exp_2R(r1, r2)
    {}

    void run()
    {
        using namespace vm;
        CPU::registers[r1_] = CPU::registers[r1_] - CPU::registers[r2_];
    }
};

struct Exp_MUL: public Exp_2R
{
    Exp_MUL(Register r1, Register r2): Exp_2R(r1, r2)
    {}

    void run()
    {
        using namespace vm;
        CPU::registers[r1_] = CPU::registers[r1_] * CPU::registers[r2_];
    }
};

struct Exp_DIV: public Exp_2R
{
    Exp_DIV(Register r1, Register r2): Exp_2R(r1, r2)
    {}

    void run()
    {
        using namespace vm;
        CPU::registers[r1_] = CPU::registers[r1_] / CPU::registers[r2_];
    }
};

struct Exp_LDA: public Expression
{
    Register    r1_;
    std::string addr_;

    Exp_LDA(Register r1, const std::string& addr):
        r1_(r1),
        addr_(addr)
    {}

    void run()
    {
        using namespace vm;

        Memory::iterator i = CPU::memory.find(addr_);
        if (i == CPU::memory.end()) {
            std::cerr   << "Runtime error: Adress undefined: " 
                        << addr_ << std::endl;
            return;
        }

        CPU::registers[r1_] = i->second;
    }

};

struct Exp_STO: public Expression
{
    std::string addr_;
    Register    r1_;

    Exp_STO(const std::string& addr, Register r1):
        addr_(addr),
        r1_(r1)
    {}

    void run()
    {
        using namespace vm;

        CPU::memory[addr_] = CPU::registers[r1_];
    }

};

struct Exp_LDC: public Expression
{
    Register    r1_;
    float       val_;

    Exp_LDC(Register r1, float val):
        r1_(r1),
        val_(val)
    {}

    void run()
    {
        using namespace vm;

        CPU::registers[r1_] = val_;
    }

};
typedef std::vector<Expression*> Expressions;

Expressions& prog(); // returns a pointer to the current program (expressions).

