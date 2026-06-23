#include "Memcury.hpp"

auto Memcury::Scanner::FindPatternEx(HANDLE handle, const char* pattern, const char* mask, uint64_t begin, uint64_t end) -> Scanner
{
    auto scan = [](const char* pattern, const char* mask, char* begin, unsigned int size) -> char*
    {
        size_t patternLen = strlen(mask);
        for (unsigned int i = 0; i < size - patternLen; i++)
        {
            bool found = true;
            for (unsigned int j = 0; j < patternLen; j++)
            {
                if (mask[j] != '?' && pattern[j] != *(begin + i + j))
                {
                    found = false;
                    break;
                }
            }

            if (found)
                return (begin + i);
        }
        return nullptr;
    };

    uint64_t match = NULL;
    SIZE_T bytesRead;
    char* buffer = nullptr;
    MEMORY_BASIC_INFORMATION mbi = { 0 };

    uint64_t curr = begin;

    for (uint64_t curr = begin; curr < end; curr += mbi.RegionSize)
    {
        if (!VirtualQueryEx(handle, (void*)curr, &mbi, sizeof(mbi)))
            continue;

        if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS)
            continue;

        buffer = new char[mbi.RegionSize];

        if (ReadProcessMemory(handle, mbi.BaseAddress, buffer, mbi.RegionSize, &bytesRead))
        {
            char* internalAddr = scan(pattern, mask, buffer, (unsigned int)bytesRead);

            if (internalAddr != nullptr)
            {
                match = curr + (uint64_t)(internalAddr - buffer);
                break;
            }
        }
    }
    delete[] buffer;

    MemcuryAssertM(match != 0, "FindPatternEx return nullptr");

    return Scanner(match);
}

auto Memcury::Scanner::FindPatternEx(HANDLE handle, const char* sig) -> Scanner
{
    char pattern[100];
    char mask[100];

    char lastChar = ' ';
    unsigned int j = 0;

    for (unsigned int i = 0; i < strlen(sig); i++)
    {
        if ((sig[i] == '?' || sig[i] == '*') && (lastChar != '?' && lastChar != '*'))
        {
            pattern[j] = mask[j] = '?';
            j++;
        }

        else if (isspace(lastChar))
        {
            pattern[j] = lastChar = (char)strtol(&sig[i], 0, 16);
            mask[j] = 'x';
            j++;
        }
        lastChar = sig[i];
    }
    pattern[j] = mask[j] = '\0';

    auto module = (uint64_t)GetModuleHandle(nullptr);

    return FindPatternEx(handle, pattern, mask, module, module + Memcury::PE::GetNTHeaders()->OptionalHeader.SizeOfImage);
}

auto Memcury::Scanner::FindPattern(const char* signature) -> Scanner
{
    PE::Address add { nullptr };

    const auto sizeOfImage = PE::GetNTHeaders()->OptionalHeader.SizeOfImage;
    auto patternBytes = ASM::pattern2bytes(signature);
    const auto scanBytes = reinterpret_cast<std::uint8_t*>(PE::GetModuleBase());

    const auto s = patternBytes.size();
    const auto d = patternBytes.data();

    for (auto i = 0ul; i < sizeOfImage - s; ++i)
    {
        bool found = true;
        for (auto j = 0ul; j < s; ++j)
        {
            if (scanBytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }

        if (found)
        {
            add = reinterpret_cast<uintptr_t>(&scanBytes[i]);
            break;
        }
    }

    MemcuryAssertM(add != 0, "FindPattern return nullptr");

    return Scanner(add);
}

auto Memcury::Scanner::FindPatternRel(const char* signature, uintptr_t relAddr) -> Scanner
{
    PE::Address add { nullptr };

    const auto sizeOfImage = PE::GetNTHeaders()->OptionalHeader.SizeOfImage;
    auto patternBytes = ASM::pattern2bytes(signature);
    const auto scanBytes = reinterpret_cast<std::uint8_t*>(PE::GetModuleBase());

    const auto s = patternBytes.size();
    const auto d = patternBytes.data();

    for (auto i = 0ul; i < sizeOfImage - s; ++i)
    {
        bool found = true;
        for (auto j = 0ul; j < s; ++j)
        {
            if (scanBytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }

        if (found)
        {
            uintptr_t currAddr = reinterpret_cast<uintptr_t>(&scanBytes[i]);
            uintptr_t relPtr = currAddr + s + 4 + *reinterpret_cast<int32_t*>(currAddr + s);
            if (relPtr == relAddr)
            {
                add = currAddr;
                break;
            }
        }
    }

    MemcuryAssertM(add != 0, "FindPatternRel return nullptr");

    return Scanner(add);
}

auto Memcury::Scanner::ScanForEither(std::vector<std::vector<uint8_t>> opcodesToFindArr, bool forward, int toSkip, int* foundIndex) -> Scanner
{
    const auto scanBytes = _address.GetAs<std::uint8_t*>();

    for (auto i = (forward ? 1 : -1); forward ? (i < 2048) : (i > -2048); forward ? i++ : i--)
    {
        bool found = true;

        int idx = 0;
        for (; idx < opcodesToFindArr.size(); idx++)
        {
            found = true;

            auto opcodesToFind = opcodesToFindArr[idx];
            for (int k = 0; k < opcodesToFind.size() && found; k++)
                found = opcodesToFind[k] == scanBytes[i + k];

            if (found)
                break;
        }

        if (found)
        {
            if (foundIndex)
                *foundIndex = idx;
            _address = &scanBytes[i];

            if (toSkip != 0)
            {
                return ScanForEither(opcodesToFindArr, forward, toSkip - 1, foundIndex);
            }

            break;
        }
    }

    return *this;
}

auto Memcury::Scanner::ScanForAny(std::vector<std::vector<uint8_t>> opcodesToFind, bool forward, int toSkip) -> Scanner
{
    auto base = _address.Get();

    for (auto thing : opcodesToFind)
    {
        ScanFor(thing, forward, toSkip);
        if (base != _address.Get())
            return *this;
    }

    MessageBoxA(NULL, "ScanForAny failed all checks!", "Memcury", MB_OK);

    return *this;
}


auto Memcury::Scanner::FindFunctionBoundary(bool forward) -> Scanner
{
    const auto scanBytes = _address.GetAs<std::uint8_t*>();

    for (auto i = (forward ? 1 : -1); forward ? (i < 2048) : (i > -2048); forward ? i++ : i--)
    {
        auto& byte = scanBytes[i];
        auto& next = forward ? scanBytes[i + 1] : scanBytes[i - 1];

        if ( // ASM::byteIsA(scanBytes[i], ASM::MNEMONIC::JMP_REL8) ||
             // ASM::byteIsA(scanBytes[i], ASM::MNEMONIC::JMP_REL32) ||
             // ASM::byteIsA(scanBytes[i], ASM::MNEMONIC::JMP_EAX) ||
             // ASM::byteIsA(scanBytes[i], ASM::MNEMONIC::RETN_REL8) ||
            (ASM::byteIsA(byte, ASM::MNEMONIC::RETN) && ASM::byteIsA(next, ASM::MNEMONIC::INT3))
            || (ASM::byteIsA(byte, ASM::MNEMONIC::INT3) && ASM::byteIsA(next, ASM::MNEMONIC::INT3)))
        {
            _address = (uintptr_t)&scanBytes[i + 1];
            break;
        }
    }

    return *this;
}
