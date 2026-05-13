#include "../SDK.hpp"

void UKismetSystemLibrary::ExecuteConsoleCommand(const FString& Cmd, APlayerController* Player)
{
    static auto Func = StaticClass()->GetFunction("ExecuteConsoleCommand");
    struct {
        UWorld* WorldContext;
        FString Cmd;
        APlayerController* Player;
    } args { UWorld::GetWorld(), Cmd, Player };
    Default()->ProcessEvent(Func, &args);
}

void UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(uint64 Handle)
{
    static auto Func = StaticClass()->GetFunction("K2_ClearAndInvalidateTimerHandle");
    struct {
        UObject* Context;
        uint64 Handle;
    } args { UWorld::GetWorld(), Handle };
    Default()->ProcessEvent(Func, &args);
}
