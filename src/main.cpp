//===============================================================================================
//	CrimeCraft Hack By The1domo
//	i used TheFeckless UE3 SDK Generator v1.4_Beta-Rev.51
//	Credits: TheFeckless, uNrEaL, Tamimego, SystemFiles, R00T88, _silencer, K@N@VEL, R13
//	Thanks: HOOAH07, lowHertz
//	Forums: www.uc-forum.com, www.gamedeception.net, gh05tn3t.net
//===============================================================================================
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "Utils.h"
#include "SdkHeaders.h"
#include "Detours.h"

FVector					CameraLocation;
FRotator				CameraRotation;

UGameEngine             *GameEngine             = NULL;
ULocalPlayer            *LocalPlayer            = NULL;

ATgPlayerController		*Controller				= NULL;
ATgPawn					*PlayerPawn				= NULL;

UObject					*pCallObject			= NULL;
UFunction				*pUFunc					= NULL;
void					*pParms					= NULL;
void					*pResult				= NULL;

#define UCONST_Pi 3.1415926
#define URotationToRadians( URotation )		( ( URotation ) * ( UCONST_Pi / 32768.0f ) ) 

typedef void(__fastcall* tProcessEvent) (UObject*, unsigned long, UFunction*, void*, void*);

tProcessEvent pOriginalProcessEvent = (tProcessEvent)0x11347C20;

#define GObjects_Pattern		"\xa1\x00\x00\x00\x00\x8b\x00\x00\x8b\x00\x00\x25\x00\x02\x00\x00"
#define GObjects_Mask			"x????x??x??xxxxx"
#define GObjects_Offset			0x1

#define GNames_Pattern			"\x8b\x0d\x00\x00\x00\x00\x83\x3c\x81\x00\x74"
#define GNames_Mask				"xx????xxxxx"
#define GNames_Offset			0x2

char* allMaps[] = {
	// payload
	"Push_Dust_P",
	"Push_IceFloe_P",
	"Push_IceFloe3_P",
	"Push_Toxicity",
	"push_Ravine_P",
	// where push_atoll... :pepehands:

	// control
	"Ticket_Datafarm_P",
	"Ticket_Datafarm2",
	"Ticket_Datafarm3",
	"SeaSide_Ticket_P",
	"SeaSide_Ticket2_P",
	"SeaSide_Ticket3",
	"Ticket_Volcano_P",

	// breach
	"3P_Beachhead3_P",
	"Climate_Control_P",
	"3P_Climate_Control3_P",
	"3P_VolcanoAssault_P",
	"Ice_GorgeA01_v2",
	"3P_Him_Arena_P",

	// scramble
	"Rot_BlackwaterLoch_P",
	"Rot_Redistribution04",
	"Rot_Redistribution03",
	"Rot_Redistribution05",
	"Rot_Trafalgar_P",

	// acquisition
	"CTR_DuelStrike_P",
	"CTR_DuelStrike2_P",
	"CTR_DuelStrike3_P",
	"CTR_Recursive_P", // unreleased acquisition map

	// 4v4
	"MissileComplex_4v4_P",
	"Ticket_HimLab_4v4",
	"Ticket_Osprey_4v4_P",
	"Ticket_Silo_4v4_P",

	// AvA
	"HEX_AVA_2pt_Theft_Factory1_P",
	"HEX_AVA_2pt_Theft_Lab1",
	"HEX_AVA_Defense1_P",
	"HEX_AVA_Defense2_P",
	"HEX_AVA_Factory1_P",
	"HEX_AVA_Factory2_P",
	"HEX_AVA_Factory3_P",
	"HEX_AVA_Lab1_P",
	"HEX_AVA_Lab2_P",
	"HEX_AVA_Lab3_P",
	"HEX_AVA_Missile1_P",
	"HEX_AVA_Plant_P",
	"HEX_AVA_Plant2_P",
	"HEX_AVA_Plant3_P",
	"HEX_AVA_Push_Factory1_P",
	"HEX_AVA_Push_Lab1_P",
	"HEX_AVA_Ticket_Neutral",

	// raid
	"Moving_Target00",
	"Canyon_Defense00",
	"Oasis_Checkpoint",
	"Raid_Halloween_Oasis_P",
	"Raid_DomeCityDefense_P",
	"Raid_Commonwealth_Facility", // not working

	// spec ops
	"1P_CPFactory01_P",
	"1P_CPFactory02_P",
	"1P_CPFactory05_P",
	"1P_CPLab01_P",
	"1P_CPLab02_P",
	"1P_CPLab03",
	"1P_CPLab04_P",
	"1P_CPLab05_P",
	"1P_CPMine01_P",
	"1P_CPMine02_P",
	"1P_CPMine03_P",
	"1P_CPMine04_P",
	"1P_CPMine05_P",
	"1P_CPColony01_P",
	"1P_SDColony01_P",
	"1P_SDColony02_P",
	"1P_SDColony02_Quest",
	"1P_SDColony03_P", // unreleased PvE map
	"1P_SDColony04_P",
	"1P_SDColony05_P",
	"1P_SDColony06_P",
	"1P_SDDweller01_P",
	"1P_SDDweller02_P",
	"1P_SDDweller02_Portalled_P",
	"1P_SDDweller03_P",
	"1P_SDDweller03_Portalled_P",
	"1P_SDDweller04_P",

	// open world
	"DomeCity_V3_MAIN",
	"SD_Zone_P",
	"DomeNorth_Zone_P",
	"DomeNorth_Zone_V2_P",
	"Dome3_VR_Arena_P",

	// tutorial
	"Adrenaline_P",
	"AgencyZero_P",
	"Inception_ALL",
	"Skylark_P",

	// dev maps
	"DEV_landmarkMESH_A01", // crashes
	"DomeCheck_P",
	"Seaside_Openzone",
};

void Log(char* Function) {
	FILE* fp = fopen("C:\\mylog.txt", "a");
	fprintf(fp, "\n");
	fprintf(fp, Function);
	fclose(fp);
}

ATgPlayerController* GetPlayerController() {
	if (Controller == NULL) {
		LocalPlayer = GameEngine->GamePlayers.Data[0];
		Controller = reinterpret_cast<ATgPlayerController*>(LocalPlayer->Actor);
		PlayerPawn = reinterpret_cast<ATgPawn*>(LocalPlayer->Actor->Pawn);
	}
	return Controller;
}

void LogAllObjects() {
    for (int i=0; i<UObject::GObjObjects()->Count; i++) {
        if (UObject::GObjObjects()->Data[i]) {
            Log(UObject::GObjObjects()->Data[i]->GetFullName());
        }
    }
}

void LoadToxicity() {
	GetPlayerController()->ConsoleCommand(FString(L"START Push_Toxicity.ut3?game=Engine.GameInfo?listen=true?bIsLanMatch=true?name=testplayer?team=255?DefaultCharacter=TgEngine.LocalPlayer?PlayerControllerClassName=TgGame.TgPlayerController"), 1);
}

void DoSomething() {
	GetPlayerController()->ConsoleCommand(FString(L"START 1P_SDColony03_P.ut3?game=TgGame.TgGame_OpenWorld?listen=true?bIsLanMatch=true?name=testplayer?team=255?DefaultCharacter=TgEngine.LocalPlayer?PlayerControllerClassName=TgGame.TgPlayerController"), 1);
}


void DisablePhysics() {
	// GetPlayerController()->SetPhysics(0);

	//class APawn* SpawnTemplatePlayer ( class ATgPlayerController* pTgPC, struct FName sName );
	// ATgGame* game = reinterpret_cast<ATgGame*>(GameEngine->GetCurrentWorldInfo());
	// game->SpawnTemplatePlayer(Controller, FName(L"TgGame.TgPlayer"));
	// GetPlayerController()->eventCheatFly(1);
}

FColor MakeColor(int R, int G, int B, int A)
{
	FColor ReturnedColor;
	ReturnedColor.R = R;
	ReturnedColor.G = G;
	ReturnedColor.B = B;
	ReturnedColor.A = A;
	return ReturnedColor;
}

FColor ColorRed					= MakeColor(255,0,0,255);
FColor ColorGreen				= MakeColor(0,255,0,255);
FColor ColorBlue				= MakeColor(0,0,255,255);
FColor ColorYellow				= MakeColor(255,255,0,255);
FColor ColorLilac				= MakeColor(0,255,255,255);
FColor ColorGray				= MakeColor(96,96,96,255);
FColor ColorLightGray			= MakeColor(120,120,120,255);
FColor ColorWhite				= MakeColor(255,255,255,255);
FColor ColorBlack				= MakeColor(0,0,0,0);

static void DrawString( UCanvas* pCanvas, float X, float Y, const FColor& Color, bool Center, wchar_t* String )
{
    float OldCurX        = pCanvas->CurX;
    float OldCurY        = pCanvas->CurY;
    FColor OldColor        = pCanvas->DrawColor;

    pCanvas->CurX        = X;
    pCanvas->CurY        = Y;
    pCanvas->DrawColor  = Color;

    if ( Center )
    {
        float XL, YL;
        pCanvas->TextSize( String, &XL, &YL );
        pCanvas->CurX -= XL / 2;
    }

    pCanvas->DrawText(String, false, 1, 1 );
    
    pCanvas->CurX      = OldCurX;
    pCanvas->CurY      = OldCurY;
    pCanvas->DrawColor  = OldColor;
}

void GetAxes( FRotator Angles, FVector& vRight, FVector& vUp, FVector& vForward )
{
    float        angle;
                
    angle = URotationToRadians(Angles.Yaw);
        
    float SinYaw = sin(angle);    
    float CosYaw = cos(angle);
        
    angle = URotationToRadians(Angles.Pitch);
    
    float SinPitch = sin(angle);
    float CosPitch = cos(angle);
        
    angle = URotationToRadians(Angles.Roll);
        
    float SinRoll = sin(angle);
    float CosRoll = cos(angle);
        
   vRight.X = CosPitch * CosYaw;
   vRight.Y = CosPitch * SinYaw;
   vRight.Z = SinPitch;

   vUp.X = SinRoll * SinPitch * CosYaw + CosRoll * -SinYaw;
   vUp.Y = SinRoll * SinPitch * SinYaw + CosRoll * CosYaw;
   vUp.Z = -SinRoll * CosPitch;
  
   vForward.X = -( CosRoll * SinPitch * CosYaw + SinRoll * SinYaw );
   vForward.Y = -( CosRoll * SinPitch * SinYaw + SinRoll * -CosYaw );
   vForward.Z = CosRoll * CosPitch;                    
}

FVector WorldToScreen(UCanvas* Canvas, FVector WorldLocation)
{
	FVector X,Y,Z,D,Out,Delta,Transformed;

	GetAxes(CameraRotation,X,Y,Z);

	Delta = Controller->Subtract_VectorVector( WorldLocation, CameraLocation );
	Transformed.X = Controller->Dot_VectorVector( Delta, Y );
	Transformed.Y = Controller->Dot_VectorVector( Delta, Z );
	Transformed.Z = Controller->Dot_VectorVector( Delta, X );

	if( Transformed.Z < 1.00f )
		Transformed.Z = 1.00f;

	float FOVAngle = Controller->FOVAngle;

	Out.X = ( Canvas->ClipX / 2.0f ) + Transformed.X * ( ( Canvas->ClipX / 2.0f ) / Controller->Tan( FOVAngle * CONST_Pi / 360.0f ) ) / Transformed.Z;
	Out.Y = ( Canvas->ClipY / 2.0f ) + -Transformed.Y * ( ( Canvas->ClipX / 2.0f ) / Controller->Tan( FOVAngle * CONST_Pi / 360.0f ) ) / Transformed.Z;
	Out.Z = 0;
	return Out;
}

void Tick(float Delta) {
	if (GetAsyncKeyState(VK_END)) {
		DoSomething();
		Sleep(250);
	}

	if (GetAsyncKeyState(VK_INSERT)) {
		DisablePhysics();
		Sleep(250);
	}
}

void PostRender ( UCanvas* pCanvas )
{
	if ( pCanvas == NULL || GameEngine == NULL || LocalPlayer == NULL )
		return;

	if (Controller == NULL)
		return;

	// char buffer[512];
	// sprintf(buffer, "Controller->Location: %f, %f, %f", Controller->Location.X, Controller->Location.Y, Controller->Location.Z);
	//
	// const size_t cSize = strlen(buffer) + 1;
	// wchar_t* wc = new wchar_t[cSize];
	// mbstowcs (wc, buffer, cSize);
	//
	// DrawString(pCanvas, 100, 50, ColorWhite, false, wc);
}

bool FindGameTables( void )
{
    MODULEINFO miGame = Utils::GetModuleInfo ( NULL );
    GObjects = *(unsigned long*) ( Utils::FindPattern( (unsigned long) miGame.lpBaseOfDll, miGame.SizeOfImage, (unsigned char*) GObjects_Pattern, (char*) GObjects_Mask ) + GObjects_Offset );
    GNames = *(unsigned long*) ( Utils::FindPattern( (unsigned long) miGame.lpBaseOfDll, miGame.SizeOfImage, (unsigned char*) GNames_Pattern, (char*) GNames_Mask ) + GNames_Offset );

    return true;
}


void __fastcall hkProcessEvent(UObject* PObject, unsigned long uReserved, UFunction* PFunction, void* pParams, void* pResult) {
	if (PObject && PFunction) {			
		if (strcmp("Function Engine.GameViewportClient.PostRender", PFunction->GetFullName()) == 0) {
			PostRender(((UGameViewportClient_eventPostRender_Parms *)pParams)->Canvas );
		} else if (strcmp("Function Engine.GameViewportClient.Tick", PFunction->GetFullName()) == 0) {
			Tick(((UGameViewportClient_eventTick_Parms*)pParams)->DeltaTime);
		} else {
			// Log(PFunction->GetFullName());
		}
    }

	pOriginalProcessEvent(PObject, uReserved, PFunction, pParams, pResult);
}

unsigned long ModuleThread( void* )
{
    while ( !GetAsyncKeyState( VK_HOME ) )
        Sleep( 100 );

    if ( FindGameTables() )
    {
        while ( !(GameEngine = UObject::FindObject<UGameEngine>("GameEngine Transient.GameEngine")))
        {
            Sleep( 100 );
        }

        while ( GameEngine->GameViewport == NULL )
            Sleep( 100 );

        LocalPlayer = GameEngine->GamePlayers.Data[0];
		Controller = reinterpret_cast<ATgPlayerController*>(LocalPlayer->Actor);
		PlayerPawn = reinterpret_cast<ATgPawn*>(LocalPlayer->Actor->Pawn);

		pOriginalProcessEvent = (tProcessEvent)DetourFunction((PBYTE)pOriginalProcessEvent, (PBYTE)hkProcessEvent);
    }

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			CreateThread( 0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(ModuleThread), 0, 0, 0 );
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

