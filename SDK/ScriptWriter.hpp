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

    void VarL(void* Value)
    {
        Write<uint8>(0x00);
        Write<void*>(Value);
    }

    void VarI(void* Value)
    {
        Write<uint8>(0x01);
        Write<void*>(Value);
    }

    void CallMath(void* Value)
    {
        Write<uint8>(0x68);
        Write<void*>(Value);
    }

    void EndFunction()
    {
        Write<uint8>(0x16);
    }

    void Let(void* Value)
    {
        Write<uint8>(0x0F);
        Write<void*>(Value);
    }

    void Jump(int32 Value)
    {
        Write<uint8>(0x06);
        Write<int32>(Value);
    }

    void PopExecIfNot()
    {
        Write<uint8>(0x4F);
    }

    void FinalFunction(void* Value)
    {
        Write<uint8>(0x1C);
        Write<void*>(Value);
    }

    void Int64Const(int64 Value)
    {
        Write<uint8>(0x35);
        Write<int64>(Value);
    }

    void FloatConst(float Value)
    {
        Write<uint8>(0x1E);
        Write<float>(Value);
    }

    void IntConst(int32 Value)
    {
        Write<uint8>(0x1D);
        Write<int32>(Value);
    }

    void PopExec()
    {
        Write<uint8>(0x4D);
    }
};
