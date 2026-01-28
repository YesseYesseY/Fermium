class UEngine : public UObject
{
    STATIC_CLASS(L"/Script/Engine.Engine");

    static UEngine* GetEngine()
    {
        static UEngine* Ret = nullptr;
        if (!Ret)
            Ret = UObject::FindFirstObjectOfClass<UEngine>(StaticClass());

        return Ret;
    }

    PROP_REF_REFLECTION(UGameViewportClient*, GameViewport);
    PROP_REF_REFLECTION(UClass*, ConsoleClass);
};
