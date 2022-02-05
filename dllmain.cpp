#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include <string>
#include "xorstr.h"
#include "utils.h"
#include "GameSDK.h"
#include "PresentGui.h"
#include <intrin.h>

bool ShowMenu = false;
typedef signed int(__thiscall* tPostRender) (UObject*, UCanvas*);
tPostRender oPostRender = NULL;
signed int __stdcall hPostRender(UObject* pCallObject, UCanvas* pUFunc)
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
		ShowMenu = !ShowMenu;

	DrawGUI((UCanvas*)pUFunc, ShowMenu);

	return oPostRender(pCallObject, pUFunc);
}

VOID Main() {
	AUWorld = Utils::FindSiganture(xorstr("48 89 05 ? ? ? ? 48 8B 4B 78"));
	AUWorld = RELATIVE_ADDRESS(AUWorld, 7);

	FreeObject = Utils::FindSiganture(xorstr("48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 30 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 48 8B 07 4C 8B 40 30 48 8D 05 ? ? ? ? 4C 3B C0"));

	NameByIndex = Utils::FindSiganture(xorstr("48 89 5C 24 18 48 89 6C 24 20 56 57 41 56 48 81 EC 40 08"));

	BoneMatrix = Utils::FindSiganture(xorstr("E8 ? ? ? ? 48 8B 47 30 F3 0F 10 45"));
	BoneMatrix = RELATIVE_ADDRESS(BoneMatrix, 5);

	ProjectWorldToScreen = Utils::FindSiganture(xorstr("E8 ? ? ? ? 48 8B 5C 24 ? 41 88 07 48 83 C4 30"));
	ProjectWorldToScreen = RELATIVE_ADDRESS(ProjectWorldToScreen, 5);

	LineOfSightToSAi = Utils::FindSiganture(xorstr("48 8B C4 48 89 58 20 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 20 45 8A E9"));

	ProcessEventSig = Utils::FindSiganture(xorstr("E8 ? ? ? ? 44 88 A3 ? ? ? ? EB 05"));
	ProcessEventSig = RELATIVE_ADDRESS(ProcessEventSig, 5);

	UObjectArray = Utils::FindSiganture(xorstr("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1"));
	UObjectArray = RELATIVE_ADDRESS(UObjectArray, 7);

	auto UWorld = *(uint64_t*)(AUWorld);
	auto GameInstance = *(uint64_t*)(UWorld + 0x190);
	auto LocalPlayers = *(uint64_t*)(GameInstance + 0x38);
	auto ULocalPlayer = *(uint64_t*)(LocalPlayers);
	auto UViewportClient = *(uint64_t*)(ULocalPlayer + 0x70);
	auto vpVt = *(void***)(UViewportClient);

	RobotoFont = (UObject*)FindAllObjects(xorstr("Fonts/Roboto_"), xorstr("Roboto_"));
	K2_DrawTextA = FindObject("K2_DrawText");
	K2_DrawLineA = FindObject("K2_DrawLine");

	DWORD PEOutput;
	VirtualProtect(&vpVt[107], 8, 0x40, &PEOutput);
	oPostRender = reinterpret_cast<decltype(oPostRender)>(vpVt[107]);
	vpVt[107] = &hPostRender;
	VirtualProtect(&vpVt[107], 8, PEOutput, 0);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		Main();
	}

	return TRUE;
}