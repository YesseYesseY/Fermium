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
