#include "m6502.h"

m6502::s32 m6502::CPU::Execute(s32 Cycles, Mem &memory)
{
    /** Load a Register with the value from the memory address */
	auto LoadRegister = 
		[&Cycles,&memory,this]
		( Word Address, Byte& Register )
	{
		Register = ReadByte( Cycles, Address, memory );
		LoadRegisterSetStatus( Register );
	};


    const s32 CyclesRequested = Cycles;
    while (Cycles > 0)
    {
        Byte Instruction = FetchByte(Cycles, memory); // 8 bit instruction grabbed from PC
        switch (Instruction)
        {
        // Load Accumulator
        case INS_LDA_IM:
        {
            A = FetchByte(Cycles, memory);
            LoadRegisterSetStatus(A);
        }
        break;
        case INS_LDA_ZP:
        {
            Word Address = AddrZeroPage(Cycles, memory);
            LoadRegister(Address,A);
        }
        break;
        case INS_LDA_ZPX:
        {
            Word ZeroPageAddress = AddrZeroPageX(Cycles, memory);
            LoadRegister(ZeroPageAddress, A);
        }
        break;
        case INS_LDA_ABS:
        {
            Word AbsAddress = AddrAbsolute(Cycles, memory);
            LoadRegister(AbsAddress,A);
        }
        break;
        case INS_LDA_ABSX:
        {
            Word AbsAddress = AddrAbsoluteX(Cycles, memory);
            LoadRegister(AbsAddress,A);
        }
        break;
        case INS_LDA_ABSY:
        {
            Word AbsAddress = AddrAbsoluteY(Cycles, memory);
            LoadRegister(AbsAddress,A);
        }
        break;
        case INS_LDA_INDX:
        {
            Word EffectiveAddress = AddrIndirectX(Cycles,memory);
            LoadRegister(EffectiveAddress,A);
        }
        break;
        case INS_LDA_INDY:
        {
            Word EffectiveAddressY = AddrIndirectY(Cycles, memory);
            LoadRegister(EffectiveAddressY,A);
        }
        break;

        // Load X Register
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

        case INS_LDX_ZPY:
        {
            Word Address = AddrZeroPageY(Cycles, memory);
            X = ReadByte(Cycles, Address, memory);
            LoadRegisterSetStatus(X);
        }
        break;

        case INS_LDX_ABS:
        {
            Word Address = AddrAbsolute(Cycles, memory);
            X = ReadByte(Cycles, Address, memory);
            LoadRegisterSetStatus(X);
        }
        break;

        case INS_LDX_ABSY:
        {
            Word AbsAddress = AddrAbsoluteY(Cycles, memory);
            X = ReadByte(Cycles, AbsAddress, memory);
            LoadRegisterSetStatus(X);
        }
        break;

        // Load Y Register
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

        case INS_LDY_ZPX:
        {
            Word ZeroPageAddress = AddrZeroPageX(Cycles, memory);
            Y = ReadByte(Cycles, ZeroPageAddress, memory);
            LoadRegisterSetStatus(Y);
        }
        break;

        case INS_LDY_ABS:
        {
            Word Address = AddrAbsolute(Cycles, memory);
            Y = ReadByte(Cycles, Address, memory);
            LoadRegisterSetStatus(Y);
        }
        break;

        case INS_LDY_ABSX:
        {
            Word AbsAddress = AddrAbsoluteX(Cycles, memory);
            Y = ReadByte(Cycles, AbsAddress, memory);
            LoadRegisterSetStatus(Y);
        }
        break;

        //Store Accumulator in Memory
        case INS_STA_ZP:
        {
            Word Address = AddrZeroPage(Cycles, memory);
            WriteByte(A, Cycles, Address, memory);
        }
        break;

        case INS_STA_ZPX:
        {
            Word Address = AddrZeroPageX(Cycles, memory);
            WriteByte(A, Cycles, Address, memory);
        }
        break;

        case INS_STA_ABS:
        {
            Word Address = AddrAbsolute(Cycles, memory);
            WriteByte(A, Cycles, Address, memory);
        }
        break;

        case INS_STA_ABSX:
        {
            Word Address = AddrAbsoluteX5(Cycles, memory);
            WriteByte(A, Cycles, Address, memory);
        }
        break;

        case INS_STA_ABSY:
        {
            Word Address = AddrAbsoluteY5(Cycles, memory);
            WriteByte(A, Cycles, Address, memory);
        }
        break;

        case INS_STA_INDX:
        {
            Word Address = AddrIndirectX(Cycles, memory);
            WriteByte(A, Cycles, Address, memory);
        }
        break;

        case INS_STA_INDY:
        {
            Word Address = AddrIndirectY6(Cycles, memory);
            WriteByte(A, Cycles, Address, memory);
        }
        break;

        //Store X Register in Memory
        case INS_STX_ZP:
        {
            Word Address = AddrZeroPage(Cycles, memory);
            WriteByte(X, Cycles, Address, memory);
        }
        break;

        case INS_STX_ZPY:
        {
            Word Address = AddrZeroPageY(Cycles, memory);
            WriteByte(X, Cycles, Address, memory);
        }
        break;

        case INS_STX_ABS:
        {
            Word Address = AddrAbsolute(Cycles, memory);
            WriteByte(X, Cycles, Address, memory);
        }
        break;

        //Store Y Register in Memory
        case INS_STY_ZP:
        {
            Word Address = AddrZeroPage(Cycles, memory);
            WriteByte(Y, Cycles, Address, memory);
        }
        break;

        case INS_STY_ZPX:
        {
            Word Address = AddrZeroPageX(Cycles, memory);
            WriteByte(Y, Cycles, Address, memory);
        }
        break;

        case INS_STY_ABS:
        {
            Word Address = AddrAbsolute(Cycles, memory);
            WriteByte(Y, Cycles, Address, memory);
        }
        break;

        //jump to subroutine
        case INS_JSR:
        {
            Word SubAddr = FetchWord(Cycles, memory);
            PushPCToStack(Cycles,memory);
            PC = SubAddr;
            Cycles--;
        }
        break;

        //return from subroutine
        case INS_RTS:
        {
            Word Address = PopWordFromStack(Cycles, memory);
            PC = Address+1;
            Cycles-=2;
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

m6502::Word m6502::CPU::AddrZeroPage(s32& Cycles, Mem& memory)
{
    Word ZeroPageAddr = FetchByte(Cycles, memory);
    return ZeroPageAddr;
}

m6502::Word m6502::CPU::AddrZeroPageX(s32& Cycles, Mem& memory)
{
    Word ZeroPageAddr = FetchByte(Cycles, memory);
    ZeroPageAddr += X;
    Cycles--;
    return ZeroPageAddr;
}

m6502::Word m6502::CPU::AddrZeroPageY(s32& Cycles, Mem& memory)
{
    Word ZeroPageAddr = FetchByte(Cycles, memory);
    ZeroPageAddr += Y;
    Cycles--;
    return ZeroPageAddr;
}

m6502::Word m6502::CPU::AddrAbsolute(s32& Cycles, Mem& memory)
{
    Word AbsAddress = FetchWord(Cycles, memory);
    return AbsAddress;
}

m6502::Word m6502::CPU::AddrAbsoluteX(s32& Cycles, Mem& memory)
{
    Word AbsAddress = FetchWord(Cycles, memory);
    Word AbsAddressX = AbsAddress + X;

    if (AbsAddressX - AbsAddress >= 0xFF)
    {
        Cycles--;
    }
    return AbsAddressX;
}

m6502::Word m6502::CPU::AddrAbsoluteX5(s32& Cycles, Mem& memory)
{
    Word AbsAddress = FetchWord(Cycles, memory);
    Word AbsAddressX = AbsAddress + X;
    Cycles--;
    return AbsAddressX;
}

m6502::Word m6502::CPU::AddrAbsoluteY(s32 &Cycles, Mem &memory)
{
    Word AbsAddress = FetchWord(Cycles, memory);
    Word AbsAddressY = AbsAddress + Y;

    if (AbsAddressY - AbsAddress >= 0xFF)
    {
        Cycles--;
    }
    return AbsAddressY;
}

m6502::Word m6502::CPU::AddrAbsoluteY5(s32 &Cycles, Mem &memory)
{
    Word AbsAddress = FetchWord(Cycles, memory);
    Word AbsAddressY = AbsAddress + Y;
    Cycles--;
    return AbsAddressY;
}

m6502::Word m6502::CPU::AddrIndirectX(s32& Cycles, Mem& memory)
{
    Byte ZPAddress = FetchByte(Cycles, memory);
    ZPAddress += X;
    Cycles--;
    Word EffectiveAddress = ReadWord(Cycles, ZPAddress, memory);
    return EffectiveAddress;
}

m6502::Word m6502::CPU::AddrIndirectY(s32& Cycles, Mem& memory)
{
    Byte ZPAddress = FetchByte(Cycles, memory);
    Word EffectiveAddress = ReadWord(Cycles, ZPAddress, memory);
    Word EffectiveAddressY = EffectiveAddress + Y;
           
    if (EffectiveAddressY - EffectiveAddress >= 0xFF)
    {
        Cycles--;
    }
    return EffectiveAddressY;
}

m6502::Word m6502::CPU::AddrIndirectY6(s32& Cycles, Mem& memory)
{
    Byte ZPAddress = FetchByte(Cycles, memory);
    Word EffectiveAddress = ReadWord(Cycles, ZPAddress, memory);
    Word EffectiveAddressY = EffectiveAddress + Y; 
    Cycles--;
    return EffectiveAddressY;
}



