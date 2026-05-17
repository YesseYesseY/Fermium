namespace Events
{
    void Init()
    {
        if (GameVersion == 8.40f)
        {
            ABP_SnowAlwaysRelevant_C::Get()->LoadSleepy();
        }
        else if (GameVersion == 8.50f)
        {
            auto Sneezy = ABP_Sneezy_FloorProp_Area_C::Get();
            Sneezy->SetDancefloorEnabled(true);
        }
        else if (GameVersion >= 15.00f && GameVersion < 16.00f)
        {
            auto Portals = UGameplayStatics::GetAllActorsOfClass<ABP_Pickle_Portal_Parent_C>(ABP_Pickle_Portal_Parent_C::StaticClass());
            for (auto Portal : Portals)
            {
                Portal->GetProgress() = 1.0f;
                Portal->OnRep_Progress();
                Portal->ForceNetUpdate();
            }
            Portals.Free();

            if (GameVersion == 15.50f)
            {
                auto ZP = ABP_ZeroPoint_Destabalize_RWs_C::Get();
                auto ZP2 = ABP_ZeroPoint_2Point0_C::Get();
                ZP2->GetProgress() = 1.0f;
                ZP2->OnRep_Progress();
                UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(ZP->GetProgressTimer());
                ZP->GetTimer() = 30.0f;
                ZP->OnRep_Timer();
                ZP->ForceNetUpdate();
                ZP2->ForceNetUpdate();
            }
        }
    }

    void PostInit()
    {
        if (GameVersion == 8.40f)
        {
            static auto Sleepy = ABP_Sleepy_M_C::Get();
            static auto SleepyProp = ABP_Sleepy_Prop_C::Get();
            auto Func = ABP_Sleepy_Prop_C::StaticClass()->GetFunction("ExecuteUbergraph_BP_Sleepy_Prop");
            auto& Script = Func->GetScript();

            auto ScriptSize = Script.Num();
            auto Writer = ScriptWriter(Script.GetData());
            Writer.Skip(2676);
            Writer.Jump(ScriptSize);

            Script.Reserve(512);
            Writer = ScriptWriter(Script.GetData());
            Writer.Skip(ScriptSize);

            auto FTruncProp = Func->GetProp("CallFunc_FTrunc_ReturnValue");
            auto DamageProp = Func->GetProp("K2Node_Event_Damage_1");
            auto Conv64Prop = Func->GetProp("CallFunc_Conv_IntToInt64_ReturnValue");
            auto LessEqualProp = Func->GetProp("CallFunc_Conv_IntToBool_ReturnValue");
            auto FloatProp = Func->GetProp("CallFunc_Conv_IntToBool_ReturnValue");
            auto ProgressProp = SleepyProp->ClassPrivate->GetProp("Progress");

            Writer.Let(FTruncProp);
            Writer.VarL(FTruncProp);
            Writer.CallMath(UObject::FindFunction(L"/Script/Engine.KismetMathLibrary:FTrunc"));
            Writer.VarL(DamageProp);
            Writer.EndFunction();

            Writer.Let(Conv64Prop);
            Writer.VarL(Conv64Prop);
            Writer.CallMath(UObject::FindFunction(L"/Script/Engine.KismetMathLibrary:Conv_IntToInt64"));
            Writer.VarL(FTruncProp);
            Writer.EndFunction();

            Writer.Let(Conv64Prop);
            Writer.VarL(Conv64Prop);
            Writer.CallMath(UObject::FindFunction(L"/Script/Engine.KismetMathLibrary:Subtract_Int64Int64"));
            Writer.VarI(UObject::FindObject(L"/Script/FortniteGame.BuildingPropBigHealth:BigHealth"));
            Writer.VarL(FTruncProp);
            Writer.EndFunction();

            Writer.Let(LessEqualProp);
            Writer.VarL(LessEqualProp);
            Writer.CallMath(UObject::FindFunction(L"/Script/Engine.KismetMathLibrary:LessEqual_Int64Int64"));
            Writer.VarL(Conv64Prop);
            Writer.Int64Const(0);
            Writer.EndFunction();

            Writer.PopExecIfNot();
            Writer.VarL(LessEqualProp);

            Writer.Let(DamageProp);
            Writer.VarL(DamageProp);
            Writer.CallMath(UObject::FindFunction(L"/Script/Engine.KismetMathLibrary:Add_FloatFloat"));
            Writer.VarI(ProgressProp);
            Writer.FloatConst(0.001f);
            Writer.EndFunction();

            Writer.FinalFunction(ABP_Sleepy_Prop_C::StaticClass()->GetFunction("EdgeServerSetProgress"));
            Writer.VarL(DamageProp);
            Writer.EndFunction();

            Writer.FinalFunction(ABP_Sleepy_Prop_C::StaticClass()->GetFunction("EdgeServerApplyDamage"));
            Writer.IntConst(250);
            Writer.EndFunction();

            Writer.PopExec();
        }
        else if (GameVersion == 8.50f)
        {
            auto Sneezy = ABP_Sneezy_FloorProp_Area_C::Get();
            auto Func = Sneezy->ClassPrivate->GetFunction("ExecuteUbergraph_BP_Sneezy_FloorProp_Area");
            auto& Script = Func->GetScript();
            auto ScriptSize = Script.Num();
            auto Writer = ScriptWriter(Script.GetData());
            Writer.Skip(622);
            Writer.Jump(ScriptSize);

            Script.Reserve(512);
            Writer = ScriptWriter(Script.GetData());
            Writer.Skip(ScriptSize);

            auto DancerCountProp = Sneezy->ClassPrivate->GetProp("DancerCount");
            auto IntToFloatProp = Func->GetProp("CallFunc_Abs_ReturnValue");
            auto DivideProp = Func->GetProp("CallFunc_Divide_FloatFloat_ReturnValue");

            Writer.FinalFunction(Sneezy->ClassPrivate->GetFunction("FlushNetDormancy"));
            Writer.EndFunction();

            Writer.Let(IntToFloatProp);
            Writer.VarL(IntToFloatProp);
            Writer.CallMath(UObject::FindFunction(L"/Script/Engine.KismetMathLibrary:Conv_IntToFloat"));
            Writer.VarI(DancerCountProp);
            Writer.EndFunction();

            Writer.Let(DivideProp);
            Writer.VarL(DivideProp);
            Writer.CallMath(UObject::FindFunction(L"/Script/Engine.KismetMathLibrary:Divide_FloatFloat"));
            Writer.VarL(IntToFloatProp);
            Writer.FloatConst((float)UGameplayStatics::GetGameMode()->GetNumPlayers());
            Writer.EndFunction();

            Writer.FinalFunction(Sneezy->ClassPrivate->GetFunction("EdgeServerMeterLevel"));
            Writer.VarL(DivideProp);
            Writer.EndFunction();

            Writer.PopExec();
        }
    }

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
