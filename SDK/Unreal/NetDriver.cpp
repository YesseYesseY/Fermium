#include "../SDK.hpp"

void UNetDriver::TickFlushHook(UNetDriver* NetDriver, float DeltaSeconds)
{
    if (auto ReplicationDriver = NetDriver->GetReplicationDriver())
    {
        ReplicationDriver->ServerReplicateActors(DeltaSeconds);
    }

    NetDriver->TickFlush(DeltaSeconds);
}
