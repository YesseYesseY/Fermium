class AFortTeamPrivateInfo : public AInfo
{
};

class AFortTeamInfo : public AInfo
{
    PROP_REF_REFLECTION(TArray<AController*>, TeamMembers)
    PROP_REF_REFLECTION(uint8, Team);
    PROP_REF_REFLECTION(AFortTeamPrivateInfo*, PrivateInfo);
};
