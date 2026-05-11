namespace Events
{
    void Start()
    {
        if (GameVersion >= 5.30f && GameVersion < 6.00f)
        {
            // TODO Lightning
            static bool SpawnedCube = false;
            static auto Cube = ACUBE_C::Get();
            if (!SpawnedCube)
            {
                SpawnedCube = true;
                Cube->SpawnCube();
            }
            else
            {
                static int Step = 0;
                Cube->Next(Step++);
                // TODO ? Sink
            }
        }
    }
}
