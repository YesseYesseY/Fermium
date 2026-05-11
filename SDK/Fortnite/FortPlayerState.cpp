#include "../SDK.hpp"

void AFortPlayerState::ApplyCharacterParts(TArray<UCustomCharacterPart*>& Parts)
{
    if (HasCharacterParts())
    {
        for (int i = 0; i < 6; i++)
            GetCharacterParts().Parts[i] = nullptr;

        for (auto Part : Parts)
            GetCharacterParts().Parts[Part->GetCharacterPartType()] = Part;

        static auto OnRepFunc = ClassPrivate->GetFunction("OnRep_CharacterParts");
        ProcessEvent(OnRepFunc);
    }
    else if (HasCharacterData())
    {
        for (int i = 0; i < 6; i++)
            GetCharacterData().GetParts()[i] = nullptr;

        for (auto Part : Parts)
            GetCharacterData().GetParts()[Part->GetCharacterPartType()] = Part;

        static auto OnRepFunc = ClassPrivate->GetFunction("OnRep_CharacterData");
        ProcessEvent(OnRepFunc);
    }
}

void AFortPlayerState::ApplyCID(UAthenaCharacterItemDefinition* CID)
{
    if (!CID)
        return;

    auto Hero = CID->GetHeroDefinition();
    if (!Hero)
        return;

    TArray<UCustomCharacterPart*> Parts;
    for (auto Spec : Hero->GetSpecializations())
    {
        auto CharParts = Spec.Get()->GetCharacterParts();
        for (int i = 0; i < CharParts.Num(); i++)
        {
            auto Part = CharParts[i].Get();
            Parts.Add(Part);
        }
    }
    ApplyCharacterParts(Parts);
    Parts.Free();
}
