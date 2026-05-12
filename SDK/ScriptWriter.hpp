class ScriptWriter
{
public:
    uintptr_t Addr;

    ScriptWriter(void* addr)
    {
        Addr = (uintptr_t)addr;
    }

    void SetAddr(uintptr_t Value)
    {
        Addr = Value;
    }

    void Skip(int32 Value)
    {
        Addr += Value;
    }

    template <typename T>
    void Write(T Value)
    {
        *(T*)(Addr) = Value;
        Addr += sizeof(T);
    }

    void WriteVarL(void* Value)
    {
        Write<uint8>(0x00);
        Write<void*>(Value);
    }

    void WriteVarI(void* Value)
    {
        Write<uint8>(0x01);
        Write<void*>(Value);
    }

    void WriteCallMath(void* Value)
    {
        Write<uint8>(0x68);
        Write<void*>(Value);
    }

    void WriteEndFunction()
    {
        Write<uint8>(0x16);
    }

    void WriteLet(void* Value)
    {
        Write<uint8>(0x0F);
        Write<void*>(Value);
    }

    void WriteJump(int32 Value)
    {
        Write<uint8>(0x06);
        Write<int32>(Value);
    }

    void WritePopExecIfNot()
    {
        Write<uint8>(0x4F);
    }

    void WriteFinalFunction(void* Value)
    {
        Write<uint8>(0x1C);
        Write<void*>(Value);
    }

    void WriteInt64Const(int64 Value)
    {
        Write<uint8>(0x35);
        Write<int64>(Value);
    }

    void WriteFloatConst(float Value)
    {
        Write<uint8>(0x1E);
        Write<float>(Value);
    }

    void WriteIntConst(int32 Value)
    {
        Write<uint8>(0x1D);
        Write<int32>(Value);
    }

    void WritePopExec()
    {
        Write<uint8>(0x4D);
    }
};
