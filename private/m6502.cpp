#include "m6502.h"

m6502::Word m6502::CPU::AddrZeroPage(s32& Cycles, Mem& memory)
{
    Byte ZeroPageAddr = FetchByte(Cycles, memory);
}

m6502::s32 m6502::CPU::Execute(s32 Cycles, Mem& memory)
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
                    A = FetchByte(Cycles, memory);
                    LoadRegisterSetStatus(A);
                }
                break;
                case INS_LDA_ZP:
                {
                    Word Address = AddrZeroPage(Cycles, memory);
                    A = ReadByte(Cycles, Address, memory);
                    LoadRegisterSetStatus(A);
                }
                break;
                case INS_LDA_ZPX:
                {
                    Byte ZeroPageAddress = FetchByte(Cycles, memory);
                    ZeroPageAddress += X;
                    Cycles--;
                    A = ReadByte(Cycles, ZeroPageAddress, memory);
                    LoadRegisterSetStatus(A);
                }
                break;
                case INS_LDA_ABS:
                {
                    Word AbsAddress = FetchWord(Cycles, memory);
                    A = ReadByte(Cycles, AbsAddress,memory);
                    LoadRegisterSetStatus(A);
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
                    LoadRegisterSetStatus(A);
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
                    LoadRegisterSetStatus(A);
                }
                break;
                case INS_LDA_INDX:
                {
                    Byte ZPAddress = FetchByte(Cycles, memory);
                    ZPAddress += X;
                    Cycles--;
                    Word EffectiveAddress = ReadWord(Cycles,ZPAddress, memory);
                    A = ReadByte(Cycles, EffectiveAddress, memory);
                    LoadRegisterSetStatus(A);
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
                    LoadRegisterSetStatus(A);
                }
                break;

                //Load X Register
                case INS_LDX_IM:
                {
                    X = FetchByte(Cycles, memory);
                    LoadRegisterSetStatus(X);
                }
                break;

                case INS_LDX_ZP:
                {
                    Word Address = AddrZeroPage(Cycles, memory);
                    X = ReadByte(Cycles, Address, memory);
                    LoadRegisterSetStatus(X);
                }
                break;

                //Load Y Register
                case INS_LDY_IM:
                {
                    Y = FetchByte(Cycles, memory);
                    LoadRegisterSetStatus(Y);
                }

                case INS_LDY_ZP:
                {
                    Word Address = AddrZeroPage(Cycles, memory);
                    Y = ReadByte(Cycles, Address, memory);
                    LoadRegisterSetStatus(Y);
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
        const s32 NumCyclesUsed = CyclesRequested - Cycles;
	    return NumCyclesUsed;
    }