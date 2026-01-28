class AActor : public UObject
{
public:
    FTransform GetTransform()
    {
        static auto Func = ClassPrivate->GetFunction("GetTransform");
        FTransform Ret;
        ProcessEvent(Func, &Ret);
        return Ret;
    }
};
