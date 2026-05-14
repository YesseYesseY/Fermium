namespace Events
{
    void Init()
    {
        if (GameVersion == 8.40f)
        {
            ABP_SnowAlwaysRelevant_C::Get()->LoadSleepy();
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
            Writer.WriteJump(ScriptSize);

            Script.Reserve(512);
            Writer = ScriptWriter(Script.GetData());
            Writer.Skip(ScriptSize);

            auto FTruncProp = Func->GetProp("CallFunc_FTrunc_ReturnValue");
            auto DamageProp = Func->GetProp("K2Node_Event_Damage_1");
            auto Conv64Prop = Func->GetProp("CallFunc_Conv_IntToInt64_ReturnValue");
            auto LessEqualProp = Func->GetProp("CallFunc_Conv_IntToBool_ReturnValue");
            auto FloatProp = Func->GetProp("CallFunc_Conv_IntToBool_ReturnValue");
            auto ProgressProp = SleepyProp->ClassPrivate->GetProp("Progress");

            Writer.WriteLet(FTruncProp);
            Writer.WriteVarL(FTruncProp);
            Writer.WriteCallMath(UObject::FindFunction(L"/Script/Engine.KismetMathLibrary:FTrunc"));
            Writer.WriteVarL(DamageProp);
            Writer.WriteEndFunction();

            Writer.WriteLet(Conv64Prop);
            Writer.WriteVarL(Conv64Prop);
            Writer.WriteCallMath(UObject::FindFunction(L"/Script/Engine.KismetMathLibrary:Conv_IntToInt64"));
            Writer.WriteVarL(FTruncProp);
            Writer.WriteEndFunction();

            Writer.WriteLet(Conv64Prop);
            Writer.WriteVarL(Conv64Prop);
            Writer.WriteCallMath(UObject::FindFunction(L"/Script/Engine.KismetMathLibrary:Subtract_Int64Int64"));
            Writer.WriteVarI(UObject::FindObject(L"/Script/FortniteGame.BuildingPropBigHealth:BigHealth"));
            Writer.WriteVarL(FTruncProp);
            Writer.WriteEndFunction();

            Writer.WriteLet(LessEqualProp);
            Writer.WriteVarL(LessEqualProp);
            Writer.WriteCallMath(UObject::FindFunction(L"/Script/Engine.KismetMathLibrary:LessEqual_Int64Int64"));
            Writer.WriteVarL(Conv64Prop);
            Writer.WriteInt64Const(0);
            Writer.WriteEndFunction();

            Writer.WritePopExecIfNot();
            Writer.WriteVarL(LessEqualProp);

            Writer.WriteLet(DamageProp);
            Writer.WriteVarL(DamageProp);
            Writer.WriteCallMath(UObject::FindFunction(L"/Script/Engine.KismetMathLibrary:Add_FloatFloat"));
            Writer.WriteVarI(ProgressProp);
            Writer.WriteFloatConst(0.001f);
            Writer.WriteEndFunction();

            Writer.WriteFinalFunction(ABP_Sleepy_Prop_C::StaticClass()->GetFunction("EdgeServerSetProgress"));
            Writer.WriteVarL(DamageProp);
            Writer.WriteEndFunction();

            Writer.WriteFinalFunction(ABP_Sleepy_Prop_C::StaticClass()->GetFunction("EdgeServerApplyDamage"));
            Writer.WriteIntConst(250);
            Writer.WriteEndFunction();

            Writer.WritePopExec();
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
