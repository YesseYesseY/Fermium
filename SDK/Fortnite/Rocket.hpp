class ABP_GeodeScripting_C : public AActor
{
    STATIC_CLASS_GET(ABP_GeodeScripting_C, L"/Game/Athena/Maps/Test/Events/BP_GeodeScripting.BP_GeodeScripting_C");

    void TestLaunch(float Seconds)
    {
        static auto Func = ClassPrivate->GetFunction("TestLaunch");
        ProcessEvent(Func, &Seconds);
    }
};
