class UWorld;

class UGameViewportClient : public UObject
{
    PROP_REF_REFLECTION(UObject*, ViewportConsole);
    PROP_REF_REFLECTION(UWorld*, World);
};
