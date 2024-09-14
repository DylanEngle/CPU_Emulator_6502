#include <stdio.h>
#include <stdlib.h>

using Byte = unsigned char; // 8 bit
using Word = unsigned short; // 16 bit

using u32 = unsigned int; // 32 bit
using s32 = signed int; // 64 bit

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
    INS_JSR = 0x20;

    void LoadRegisterSetStatus()
    {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;
    }

    void Execute(s32 Cycles, Mem& memory)
    {
        const s32 CyclesRequested = Cycles;
        while(Cycles > 0)
        {
            Byte Instruction = FetchByte(Cycles, memory); // 8 bit instruction grabbed from PC
            switch(Instruction)
            {
                //Load Accumulator
                case INS_LDA_IM:
                {
                    Byte Value = FetchByte(Cycles, memory);
                    A = Value;
                    LoadRegisterSetStatus();
                }
                break;
                case INS_LDA_ZP:
                {
                    Byte ZeroPageAddress = FetchByte(Cycles, memory);
                    A = ReadByte(Cycles, ZeroPageAddress, memory);
                    LoadRegisterSetStatus();
                }
                break;
                case INS_LDA_ZPX:
                {
                    Byte ZeroPageAddress = FetchByte(Cycles, memory);
                    ZeroPageAddress += X;
                    Cycles--;
                    A = ReadByte(Cycles, ZeroPageAddress, memory);
                    LoadRegisterSetStatus();
                }
                break;
                case INS_LDA_ABS:
                {
                    Word AbsAddress = FetchWord(Cycles, memory);
                    A = ReadByte(Cycles, AbsAddress,memory);
                    LoadRegisterSetStatus();
                }
                break;
                case INS_LDA_ABSX:
                {
                    Word AbsAddress = FetchWord(Cycles, memory);
                    Word AbsAddressX = AbsAddress + X;
                    A = ReadByte(Cycles, AbsAddressX,memory);
                    if(AbsAddressX - AbsAddress >= 0xFF)
                    {
                        Cycles--;
                    }
                    LoadRegisterSetStatus();
                }
                break;
                case INS_LDA_ABSY:
                {
                    Word AbsAddress = FetchWord(Cycles, memory);
                    Word AbsAddressY = AbsAddress + Y;
                    A = ReadByte(Cycles, AbsAddressY,memory);
                    if(AbsAddressY - AbsAddress >= 0xFF)
                    {
                        Cycles--;
                    }
                    LoadRegisterSetStatus();
                }
                break;
                case INS_LDA_INDX:
                {
                    Byte ZPAddress = FetchByte(Cycles, memory);
                    ZPAddress += X;
                    Cycles--;
                    Word EffectiveAddress = ReadWord(Cycles,ZPAddress, memory);
                    A = ReadByte(Cycles, EffectiveAddress, memory);
                    LoadRegisterSetStatus();
                }
                break;
                case INS_LDA_INDY:
                {
                    Byte ZPAddress = FetchByte(Cycles, memory);
                    Word EffectiveAddress = ReadWord(Cycles,ZPAddress, memory);
                    Word EffectiveAddressY = EffectiveAddress + Y;
                    A = ReadByte(Cycles,EffectiveAddressY, memory);
                    if(EffectiveAddressY - EffectiveAddress >= 0xFF)
                    {
                        Cycles--;
                    }
                    LoadRegisterSetStatus();
                }
                break;

                case INS_LDX_IM:
                {
                    
                }
                break;



                case INS_JSR:
                {
                   Word SubAddr = FetchWord(Cycles, memory);
                   memory.WriteWord(Cycles,SP, PC-1);
                   SP++;
                   Cycles--;
                   PC = SubAddr;
                   Cycles--;

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

