// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "offsets.h"

using namespace nlohmann;

#define Print(x) std::cout << x << std::endl
#define Ptr uintptr_t
#define Offset uintptr_t

#define Netvar(x) offsets["netvars"][x]
#define Signature(x) offsets["signatures"][x]





DWORD WINAPI HackThread(HMODULE hModule)
{
    //Create console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    SetConsoleTitle(TEXT("CSGO External Glow Hack | Made by Kye#5000"));
    json offsets = GetcsgoOffsets();//Downloads offsets from hazedumper
    system("Color A");
    Print("CSGO Glow Demo - Internal");
    Print("Press F1 to eject dll");

    
    //Get module base
    Ptr clientdll = (Ptr)GetModuleHandle(L"client.dll");//Use null instead of program name
    Ptr enginedll = (Ptr)GetModuleHandle(L"engine.dll");//Use null instead of program name

    //Get Local Player
    Ptr LocalPlayer = *(Ptr*)(clientdll + Signature("dwLocalPlayer"));
    //Get our client state
    Ptr clientstate = *(Ptr*)(enginedll + Signature("dwClientState"));



    //Hack loop
    while (true)
    {
        if (GetAsyncKeyState(VK_F1) & 1)
            break;

        int MaxPlayerCount = *(int*)(clientstate + Signature("dwClientState_MaxPlayer"));
        if (MaxPlayerCount < 1)//Basically sleep the hack if ur in menu and such
        {
            Sleep(500);
            continue;
        }

        //Get our glow object
        Ptr glowObject = *(Ptr*)(clientdll + Signature("dwGlowObjectManager"));
        //Get our team number
        int myTeam = *(int*)(LocalPlayer + Netvar("m_iTeamNum"));

        for (int i = 0; i < MaxPlayerCount; i++)
        {
            //The current entity
            Ptr entity = *(Ptr*)(clientdll + Signature("dwEntityList") + i * 0x10);

            if (entity != NULL)//Our game will crash if we get a null entity so yeah just leave this check here
            {
                bool bDormant = *(bool*)(entity + Signature("m_bDormant"));//If entity is actually loaded in by the game
                if (!bDormant)
                {
                    int glowIndex = *(int*)(entity + Netvar("m_iGlowIndex"));
                    

                    Ptr CurrentEntityGlowOffset = glowObject + (glowIndex * 0x38);
                    GlowStruct currentGlow = *(GlowStruct*)(CurrentEntityGlowOffset);


                    //These variables will be the same for any of the teams
                    currentGlow.Alpha = 1.7f;
                    currentGlow.renderOccluded = true;
                    currentGlow.renderUnoccluded = false;
                    currentGlow.m_bFullBloomRender = false;
                    currentGlow.i32GlowStyle = 0;


                    //Get our team number
                    int entityTeam = *(int*)(entity + Netvar("m_iTeamNum"));
                    if (myTeam == entityTeam)
                    {
                        currentGlow.Red = 0.0f;
                        currentGlow.Green = 1.0f;
                        currentGlow.Blue = 0.0f;
                    }
                    else
                    {
                        currentGlow.Red = 1.0f;
                        currentGlow.Green = 0.0f;
                        currentGlow.Blue = 0.0f;
                        bool IsDefusing = *(bool*)(entity + Netvar("m_bIsDefusing"));
                        if (IsDefusing)
                        {
                            currentGlow.Red = 255.0f;
                            currentGlow.Green = 255.0f;
                            currentGlow.Blue = 255.0f;
                        }
                        //Our teammates are shown on map so we only have to write the radar to our enemies. (Basically this: https://youtu.be/5VOkRJk1GVg)
                        *(bool*)(entity + Netvar("m_bSpotted")) = true;
                    }

                    //Actually write the glow
                    *(GlowStruct*)(CurrentEntityGlowOffset) = currentGlow;


                    //Set the cham colors to what the glow is
                    ChamStruct currentCham = {};
                    //*255 idea from: https://stackoverflow.com/a/46575472/12897035
                    currentCham.Red = (UINT8)round(currentGlow.Red * 255);
                    currentCham.Green = (UINT8)round(currentGlow.Green * 255);
                    currentCham.Blue = (UINT8)round(currentGlow.Blue * 255);

                    //Actually write the cham
                    *(ChamStruct*)(entity + Netvar("m_clrRender")) = currentCham;
                }
            }
        }
        //Sleep(1);
    }
    //Cleanup & freeze
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

