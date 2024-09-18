#pragma once

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
};

struct m6502::CPU
{
    Word PC; // program counter
    Byte SP; // stack pointer

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
        SP = 0xFF;
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
    Byte ReadByte(s32& Cycles, Word Address, const Mem& memory)
    {
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    // read word without incrementing PC
    Word ReadWord(s32& Cycles, Word Address, const Mem& memory)
    {
        Byte LoByte = ReadByte(Cycles,Address,memory);
        Byte HiByte = ReadByte(Cycles,Address+1,memory);
        return LoByte | (HiByte << 8);
    }

    //write one byte to memory
    void WriteByte(Byte Value, s32& Cycles, Word Address, Mem& memory)
    {
        memory[Address] = Value;
        Cycles--;
    }

    // write two bytes to memory
    void WriteWord(Word Value, s32& Cycles, Word Address, Mem& memory)
    {
        memory[Address] = Value & 0xFF;
        memory[Address+1] = (Value >> 8);
        Cycles -= 2;
    }

    //return stack pointer as full 16 bit address
    Word SPToWord() const
    {
        return 0x100 | SP;
    }

    //push the pc -1 onto stack
    void PushPCToStack(s32& Cycles, Mem& memory)
    {
        WriteWord(PC-1, Cycles, SPToWord(), memory);
        SP-=2;
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

    //Store Accumulator in Memory
    INS_STA_ZP = 0x85,
    INS_STA_ZPX = 0x95,
    INS_STA_ABS = 0x8D,
    INS_STA_ABSX = 0x9D,
    INS_STA_ABSY = 0x99,
    INS_STA_INDX = 0x81,
    INS_STA_INDY = 0x91,

    //Store X Register in Memory
    INS_STX_ZP = 0x86,
    INS_STX_ZPY = 0x96,
    INS_STX_ABS = 0x8E,

    //Store Y Register in Memory
    INS_STY_ZP = 0x84,
    INS_STY_ZPX = 0x94,
    INS_STY_ABS = 0x8C,

    //Jump to Subroutine
    INS_JSR = 0x20,
    
    //Return from Subroutine
    INS_RTS = 0x60,
    
    
    
    ;



    void LoadRegisterSetStatus(Byte Register)
    {
        Z = (Register == 0);
        N = (Register & 0b10000000) > 0;
    }

    /** @return the number of cycles that were used */
	s32 Execute( s32 Cycles, Mem& memory );

    // get address from zero page
    Word AddrZeroPage(s32& Cycles, Mem& memory);

    //get address from zero page with x offset
    Word AddrZeroPageX(s32& Cycles, Mem& memory);

    //get address from zero page with y offset
    Word AddrZeroPageY(s32& Cycles, Mem& memory);

    //get address from absolute
    Word AddrAbsolute(s32& Cycles, Mem& memory);

    // get address from absolute with x offset
    Word AddrAbsoluteX(s32& Cycles, Mem& memory);

    // get address from absolute with x offset, always consume 5 cycles
    Word AddrAbsoluteX5(s32& Cycles, Mem& memory);

    //get address from absolute with y offset
    Word AddrAbsoluteY(s32& Cycles, Mem& memory);

    //get address from absolute with y offset, always consume 5 cycles
    Word AddrAbsoluteY5(s32& Cycles, Mem& memory);

    //get addresss from Indexed Indirect X
    Word AddrIndirectX(s32& Cycles, Mem& memory);

    //get address from Indexed Indirect Y
    Word AddrIndirectY(s32& Cycles, Mem& memory);

    //get address from Indexed Indirect Y, always consume 6 cycles
    Word AddrIndirectY6(s32& Cycles, Mem& memory);
};

