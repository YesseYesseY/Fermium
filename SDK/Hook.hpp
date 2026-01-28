#include <Minhook.h>

namespace Hook
{
    template <typename T = void*>
    void Function(uintptr_t Addr, void* Hook, T* Original = nullptr)
    {
        MH_CreateHook((LPVOID)Addr, Hook, (LPVOID*)Original);
        MH_EnableHook((LPVOID)Addr);
    }

    template <typename T = void*>
    void VTable(void** vTable, int32 Index, void* Hook, T* Original = nullptr)
    {
        auto Addr = (LPVOID)(int64(vTable) + (Index * 8));
        if (Original)
            *Original = (T)vTable[Index];
        DWORD yes;
        VirtualProtect(Addr, 8, PAGE_EXECUTE_READWRITE, &yes);
        vTable[Index] = Hook;
        VirtualProtect(Addr, 8, yes, &yes);
    }
}
