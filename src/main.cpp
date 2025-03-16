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
	GetPlayerController()->ConsoleCommand(FString(L"START Push_Toxicity.ut3?game=TgEditor.EdGame?listen=true?bIsLanMatch=true?name=testplayer?team=255?DefaultCharacter=TgEngine.LocalPlayer"), 1);
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
		LoadToxicity();
		Sleep(250);
	}
}

void PostRender ( UCanvas* pCanvas )
{
	if ( pCanvas == NULL || GameEngine == NULL || LocalPlayer == NULL )
		return;
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

