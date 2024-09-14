#include <stdio.h>
#include <stdlib.h>

namespace m6502
{
	using SByte = char; 
	using Byte = unsigned char; //8 bits
	using Word = unsigned short; //16 bits

	using u32 = unsigned int; //32bit
	using s32 = signed int; //64 bit

	struct Mem;
	struct CPU;
	struct StatusFlags;
}

struct m6502::Mem
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

    // write 1 byte
    Byte& operator[](u32 Address)
    {   
        return Data[Address];
    }

    // write two bytes
    void WriteWord(s32& Cycles, Word Value, u32 Address)
    {
        Data[Address] = Value & 0xFF;
        Data[Address+1] = (Value >> 8);
        Cycles -= 2;
    }
};

struct m6502::CPU
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

    // grabs instruction byte, increments PC
    Byte FetchByte(s32& Cycles, Mem& memory)
    {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;
    }

    // grabs instruction byte, increments PC
    Word FetchWord(s32& Cycles, Mem& memory)
    {
        //6502 is little endian (first byte is least significant)
        Word Data = memory[PC];
        PC++;

        Data |= (memory[PC] << 8);
        PC++;
        Cycles-=2;
        return Data;
    }

    // read byte without incrementing PC
    Byte ReadByte(s32& Cycles, Word Address, Mem& memory)
    {
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    // read word without incrementing PC
    Word ReadWord(s32& Cycles, Word Address, Mem& memory)
    {
        Byte LoByte = ReadByte(Cycles,Address,memory);
        Byte HiByte = ReadByte(Cycles,Address+1,memory);
        return LoByte | (HiByte << 8);
    }



    //opcodes
    static constexpr Byte 
    //Load Accumulator
    INS_LDA_IM = 0xA9,
    INS_LDA_ZP = 0xA5,
    INS_LDA_ZPX = 0xB5,
    INS_LDA_ABS = 0xAD,
    INS_LDA_ABSX = 0xBD,
    INS_LDA_ABSY = 0xB9,
    INS_LDA_INDX = 0xA1,
    INS_LDA_INDY = 0xB1,

    // Load X Register
    INS_LDX_IM = 0xA2,
    INS_LDX_ZP = 0xA6,
    INS_LDX_ZPY = 0xB6,
    INS_LDX_ABS = 0xAE,
    INS_LDX_ABSY = 0xBE,

    // Load Y Register
    INS_LDY_IM = 0xA0,
    INS_LDY_ZP = 0xA4,
    INS_LDY_ZPX = 0xB4,
    INS_LDY_ABS = 0xAC,
    INS_LDY_ABSX = 0xBC,
    INS_JSR = 0x20;



    void LoadRegisterSetStatus(Byte Register)
    {
        Z = (Register == 0);
        N = (Register & 0b10000000) > 0;
    }

    /** @return the number of cycles that were used */
	s32 Execute( s32 Cycles, Mem& memory );

    // get address from zero page
    m6502::Word m6502::CPU::AddrZeroPage(s32& Cycles, Mem& memory);
};

