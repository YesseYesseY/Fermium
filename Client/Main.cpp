#include <Windows.h>
#include <iostream>
#include <fstream>

#include <SDK.hpp>

bool (*UWorldExecOriginal)(UWorld* World, int64 a2, const wchar_t* Cmd, int64 a4);
bool UWorldExecHook(UWorld* World, int64 a2, const wchar_t* Cmd, int64 a4)
{
    if (wcscmp(Cmd, L"givemecheats") == 0)
    {
        auto PlayerController = World->GetOwningGameInstance()->GetLocalPlayers()[0]->GetPlayerController();
        PlayerController->GetCheatManager() = UGameplayStatics::SpawnObject(PlayerController->GetCheatClass(), PlayerController);
        return true;
    }
    else if (wcscmp(Cmd, L"dumpobjects") == 0)
    {
        UObject::DumpObjects();
        return true;
    }

    return UWorldExecOriginal(World, a2, Cmd, a4);
}

void (*CallServerMoveOriginal)(APawn* Vehicle, FReplicatedPhysicsPawnState* Args);
void CallServerMoveHook(APawn* Vehicle, FReplicatedPhysicsPawnState* Args)
{
    Args->GetRotation() = Vehicle->GetTransform().Rotation;

    CallServerMoveOriginal(Vehicle, Args);
}

bool ReturnTrueHook()
{
    return true;
}

void crashit()
{
    ((AFortPickup*)UWorld::GetWorld())->TossPickup({}, (AFortPawn*)UWorld::GetWorld(), INT32_MAX, true, UINT8_MAX, UINT8_MAX);
}

DWORD MainThread(HMODULE Module)
{
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stdout);
    freopen_s(&Dummy, "CONIN$", "r", stdin);

    InitSDK(false);

    auto Engine = UEngine::GetEngine();
    auto GameViewport = Engine->GetGameViewport();
    GameViewport->GetViewportConsole() = UGameplayStatics::SpawnObject(Engine->GetConsoleClass(), GameViewport);

    // UWorld::Exec
    {
        auto Scanner = Memcury::Scanner::FindStringRef(L"FLUSHPERSISTENTDEBUGLINES");
        auto Addr = Scanner.ScanForEither({{ 0x48, 0x8B, 0xC4 }, { 0x48, 0x89, 0x5C }}, false).Get();

        if (!Addr)
        {
            MsgBox("Failed to find UWorld::Exec. Custom commands will not work");
        }
        else
        {
            Hook::Function(Addr, UWorldExecHook, &UWorldExecOriginal);
        }
    }

    if (GameVersion >= 13.0f)
    {
        auto ServerMoveAddr = Memcury::Scanner::FindStringRef(L"ServerMove", true).ScanFor({ 0x48, 0x8D, 0x0D }).RelativeOffset(3).Get();
        auto CallServerMoveAddr = Memcury::Scanner::FindPatternRel("48 8B 15", ServerMoveAddr).ScanForEither({{ 0x48, 0x8B, 0xC4 }, { 0x48, 0x89, 0x5C}}, false).Get();
        Hook::Function(CallServerMoveAddr, CallServerMoveHook, &CallServerMoveOriginal);
    }

    // Hook::Function(0x143f17000, crashit);

    while (!(GetAsyncKeyState(VK_F5) & 0x8000)) Sleep(100);

    UKismetSystemLibrary::ExecuteConsoleCommand(L"log LogFortInventory VeryVerbose");
    UKismetSystemLibrary::ExecuteConsoleCommand(L"open 127.0.0.1");

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0);
        break;
    }

    return TRUE;
}
