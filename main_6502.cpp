#include <stdio.h>
#include <stdlib.h>

using Byte = unsigned char; // 8 bit
using Word = unsigned short; // 16 bit

using u32 = unsigned int; // 32 bit

struct Mem
{
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void Initialize()
    {
        for(u32 i = 0; i < MAX_MEM; i++)
        {
            Data[i] = 0;
        }
    }
    
    // read 1 byte
    Byte operator[](u32 Address) const
    {   
        return Data[Address];
    }

    // read 1 byte
    Byte& operator[](u32 Address)
    {   
        return Data[Address];
    }
};

struct CPU
{
    Word PC; // program counter
    Word SP; // stack pointer

    Byte A, X, Y; //accumulator, Index Register X, Index Register Y

    //processor status flags
    Byte C: 1; // carry flag
    Byte Z: 1; // zero flag
    Byte I: 1; // interrupt disable flag
    Byte D: 1; // decimal mode flag
    Byte B: 1; // break command flag
    Byte V: 1; // overflow flag
    Byte N: 1; // negative flag

    void Reset(Mem& memory)
    {
        PC = 0xFFFC;
        SP = 0x0100;
        D = 0;
        C = Z = I = D = B = V = N = 0; //clear all flags on reset
        A = X = Y = 0;
        memory.Initialize();
    }

    Byte Fetch(u32& Cycles, Mem& memory)
    {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;
    }

    //opcodes
    static constexpr Byte INS_LDA_IM = 0xA9;

    void Execute(u32 Cycles, Mem& memory)
    {
        while(Cycles > 0)
        {
            Byte Instruction = Fetch(Cycles, memory); // 8 bit instruction grabbed from PC
            switch(Instruction)
            {
                case INS_LDA_IM:
                {
                    Byte Value = Fetch(Cycles, memory);
                    A = Value;
                    Z = (A == 0);
                    N = (A & 0b10000000) > 0;
                    printf("%d", Value);
                }
                break;
                default:
                {
                    printf("Instruction not handled %d", Instruction);
                }
                break;
            }
        }
    }
};

int main()
{
    Mem mem;
    CPU cpu;
    cpu.Reset(mem);
    //hardcode test program
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x42;
    cpu.Execute(2, mem);
    return 0;
}

