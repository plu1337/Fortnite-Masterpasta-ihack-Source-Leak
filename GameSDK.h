#include "stdafx.h"
#include "vector3d.h"
#include "vector2d.h"

float FOVAngle;
uintptr_t AUWorld;
uintptr_t UObjectArray;
uintptr_t FreeObject;
uintptr_t NameByIndex;
uintptr_t BoneMatrix;
uintptr_t ProjectWorldToScreen;
uintptr_t LineOfSightToSAi;
uintptr_t ProcessEventSig;
uintptr_t GetViewpointAdr;
uintptr_t DrawHudsig;
Vector3 CamLoc;
Vector3 CamRot;

BOOL IsValidPointer(uintptr_t address)
{
	if (!IsBadWritePtr((LPVOID)address, (UINT_PTR)8)) return TRUE;
	else return FALSE;
}

class APlayerCameraManager
{
public:
	static float GetFOVAngle(uintptr_t PlayerCameraManager)
	{
		auto GetFOVAngle = reinterpret_cast<float(*)(UINT64, char*)>(*(ULONG_PTR*)(*(ULONG_PTR*)PlayerCameraManager + 0x740));
		return GetFOVAngle((ULONG_PTR)PlayerCameraManager, (char*)0);
	}

	static BOOLEAN LineOfSightTo(PVOID PlayerController, PVOID Actor, Vector3* ViewPoint) {

		auto LOSTo = reinterpret_cast<bool(__fastcall*)(PVOID PlayerController, PVOID Actor, Vector3 * ViewPoint)>(LineOfSightToSAi);

		return LOSTo(PlayerController, Actor, ViewPoint);
	}

	static bool GetPlayerViewPoint(uintptr_t PlayerController, Vector3* vCameraPos, Vector3* vCameraRot)
	{
		if (!PlayerController) return false;

		static uintptr_t pGetPlayerViewPoint = 0;
		if (!pGetPlayerViewPoint)
		{
			uintptr_t VTable = *(uintptr_t*)PlayerController;
			if (!VTable)  return false;

			pGetPlayerViewPoint = *(uintptr_t*)(VTable + 0x788);
			if (!pGetPlayerViewPoint)  return false;
		}

		auto GetPlayerViewPoint = reinterpret_cast<void(__fastcall*)(uintptr_t, Vector3*, Vector3*)>(pGetPlayerViewPoint);

		GetPlayerViewPoint((uintptr_t)PlayerController, vCameraPos, vCameraRot);

		return true;
	}
};

class USkeletalMeshComponent
{
public:
	static bool GetBoneLocation(uintptr_t CurrentActor, int id, Vector3* out)
	{
		uintptr_t mesh = *(uintptr_t*)(CurrentActor + 0x280);
		if (!mesh) return false;

		auto fGetBoneMatrix = ((FMatrix * (__fastcall*)(uintptr_t, FMatrix*, int))(BoneMatrix));
		fGetBoneMatrix(mesh, myMatrix, id);

		out->x = myMatrix->M[3][0];
		out->y = myMatrix->M[3][1];
		out->z = myMatrix->M[3][2];

		return true;
	}
};

class AController
{
public:
	static bool WorldToScreen(uintptr_t PlayerController, Vector3 WorldLocation, Vector3* out)
	{
		auto WorldToScreen = reinterpret_cast<bool(__fastcall*)(uintptr_t pPlayerController, Vector3 vWorldPos, Vector3 * vScreenPosOut, char)>(ProjectWorldToScreen);

		WorldToScreen((uintptr_t)PlayerController, WorldLocation, out, (char)0);

		if (out->x == 0 && out->y == 0 && out->z == 0) return false;
		else return true;
	}

	static void SetControlRotation(uintptr_t PlayerController, Vector3 NewRotation, bool bResetCamera = false)
	{
		auto SetControlRotation_ = (*(void(__fastcall**)(uintptr_t Controller, Vector3 NewRotation, bool bResetCamera))(*(uintptr_t*)PlayerController + 0x6F8));
		SetControlRotation_(PlayerController, NewRotation, bResetCamera);
	}
};

Vector3 Clamp(Vector3 r)
{
	if (r.y > 180.f)
		r.y -= 360.f;
	else if (r.y < -180.f)
		r.y += 360.f;

	if (r.x > 180.f)
		r.x -= 360.f;
	else if (r.x < -180.f)
		r.x += 360.f;

	if (r.x < -89.f)
		r.x = -89.f;
	else if (r.x > 89.f)
		r.x = 89.f;

	r.z = 0.f;

	return r;
}

class UClass {
public:
	BYTE _padding_0[0x40];
	UClass* SuperClass;
};

class UObject {
public:
	PVOID VTableObject;
	DWORD ObjectFlags;
	DWORD InternalIndex;
	UClass* Class;
	BYTE _padding_0[0x8];
	UObject* Outer;


	static bool ProcessEvent(uintptr_t address, void* fnobject, void* parms)
	{
		UObject* addr = reinterpret_cast<UObject*>(address); if (!addr) return false;
		auto vtable = *reinterpret_cast<void***>(addr); if (!vtable) return false;
		auto processEventFn = static_cast<void(*)(void*, void*, void*)>(vtable[0x4b]); if (!processEventFn) return false;
		processEventFn((void*)addr, (void*)fnobject, (void*)parms);
		return true;
	}

	inline BOOLEAN IsA(PVOID parentClass) {
		for (auto super = this->Class; super; super = super->SuperClass) {
			if (super == parentClass) {
				return TRUE;
			}
		}

		return FALSE;
	}
};

class FUObjectItem {
public:
	UObject* Object;
	DWORD Flags;
	DWORD ClusterIndex;
	DWORD SerialNumber;
	DWORD SerialNumber2;
};

class TUObjectArray {
public:
	FUObjectItem* Objects[9];
};

class GObjects {
public:
	TUObjectArray* ObjectArray;
	BYTE _padding_0[0xC];
	DWORD ObjectCount;
};

class FText {
public:
	char _padding_[0x28];
	PWCHAR Name;
	DWORD Length;

	inline PWCHAR c_str() {
		return Name;
	}
};

template<class T>
struct TArray
{


public:

	friend struct FString;

	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline const T& operator[](int i) const
	{
		return Data[i];
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}

	void Clear()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline void Add(T InputData)
	{
		Data = (T*)realloc(Data, sizeof(T) * (Count + 1));
		Data[Count++] = InputData;
		Max = Count;
	};

	T* Data;
	int32_t Count;
	int32_t Max;
};
struct FString : private TArray<wchar_t>
{
public:

	inline FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? (int32_t)std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{

		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}

};

struct FVector2D
{
	float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

	inline FVector2D operator/(float v) const
	{
		return FVector2D(X / v, Y / v);
	}

	inline FVector2D()
		: X(0), Y(0)
	{ }

	inline FVector2D(float x, float y)
		: X(x),
		Y(y)
	{ }

};

struct FLinearColor
{
	float                                              R;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              G;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              B;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              A;                                                        // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

	inline FLinearColor()
		: R(0), G(0), B(0), A(0)
	{ }

	inline FLinearColor(float r, float g, float b, float a)
		: R(r),
		G(g),
		B(b),
		A(a)
	{ }

};

struct FMinimalViewInfo
{
	Vector3 Location;
	Vector3 Rotation;
	float FOV;
	// etc
};

enum EStereoscopicPass
{
	eSSP_FULL,
	eSSP_LEFT_EYE,
	eSSP_RIGHT_EYE,
	eSSP_LEFT_EYE_SIDE,
	eSSP_RIGHT_EYE_SIDE,
};

static void FreeObjName(__int64 address)
{
	auto func = reinterpret_cast<__int64(__fastcall*)(__int64 a1)>(FreeObject);
	func(address);
}

static const char* GetObjectName(uintptr_t Object)
{
	if (Object == NULL)
		return ("");

	auto fGetObjName = reinterpret_cast<FString * (__fastcall*)(int* index, FString * res)>(NameByIndex);

	int index = *(int*)(Object + 0x18);

	FString result;
	fGetObjName(&index, &result);

	if (result.c_str() == NULL)
		return ("");

	auto result_str = result.ToString();

	if (result.c_str() != NULL)
		FreeObjName((__int64)result.c_str());

	return result_str.c_str();
}

static const char* GetUObjectNameLoop(UObject* Object)
{
	std::string name("");
	for (auto i = 0; Object; Object = Object->Outer, ++i) {

		auto fGetObjName = reinterpret_cast<FString * (__fastcall*)(int* index, FString * res)>(NameByIndex);

		int index = *(int*)(reinterpret_cast<uint64_t>(Object) + 0x18);

		FString fObjectName;
		fGetObjName(&index, &fObjectName);

		if (!fObjectName.IsValid()) {
			break;
		}

		auto objectName = fObjectName.ToString();


		name = objectName + std::string(i > 0 ? "." : "") + name;
		FreeObjName((uintptr_t)fObjectName.c_str());
	}

	return name.c_str();
}

static UObject* FindObject(std::string name) {
	auto objects = (GObjects*)UObjectArray;
	for (auto array : objects->ObjectArray->Objects) {
		auto fuObject = array;
		for (auto i = 0; i < 0x10000 && fuObject->Object; ++i, ++fuObject) {
			auto object = fuObject->Object;
			if (object->ObjectFlags != 0x41) {}
			else {
				if (strstr(GetUObjectNameLoop(object), name.c_str()))
					return (object);
			}
		}
	}
	return 0;
}

static UObject* FindObject2(const char* name, const char* name2) {
	auto objects = (GObjects*)UObjectArray;
	for (auto array : objects->ObjectArray->Objects) {
		auto fuObject = array;
		std::cout << "";
		for (auto i = 0; i < 0x10000 && fuObject->Object; ++i, ++fuObject)
		{
			std::cout << "";
			auto object = fuObject->Object;

			if (object->ObjectFlags != 0x41) {
				continue;
			}
			std::cout << "";

			if ((strstr(GetUObjectNameLoop(object), name)) and (strstr(GetUObjectNameLoop(object), name2))) {
				return static_cast<UObject*>(object);
			}
		}
	}

	return 0;
}

static void eraseSubStr(std::string& mainStr, const std::string& toErase)
{
	size_t pos = mainStr.find(toErase);
	if (pos != std::string::npos)
	{
		mainStr.erase(pos, toErase.length());
	}
}

static PVOID FindAllObjects(const char* name, const char* name2) {
	static GObjects* objects = nullptr;
	objects = (GObjects*)UObjectArray;
	for (auto array : objects->ObjectArray->Objects) {
		auto fuObject = array;
		std::cout << "";
		for (auto i = 0; i < 0x10000 && fuObject->Object; ++i, ++fuObject)
		{
			std::cout << "i: " << i << std::endl;
			auto object = fuObject->Object;

			std::string objname = GetUObjectNameLoop(object);
			if (strstr(objname.c_str(), name)) {
				eraseSubStr(objname, name);
				if (strstr(objname.c_str(), name2)) {
					return static_cast<UObject*>(object);
				}
			}
		}
	}
	return 0;
}

BOOL valid_pointer(DWORD64 address)
{
	if (!IsBadWritePtr((LPVOID)address, (UINT_PTR)8)) return TRUE;
	else return FALSE;
}


void Rapid(uintptr_t LocalPawn)
{
	if (valid_pointer(LocalPawn))
	{
		uint64_t CurrentWeapon = 0;
		float i = 0;
		float c = 0;

		CurrentWeapon = *(uintptr_t*)(LocalPawn + 0x5F8);

		if (CurrentWeapon) {
			i = *(float*)(CurrentWeapon + 0x9E4);
			c = *(float*)(CurrentWeapon + 0x9E8);
			*(float*)(CurrentWeapon + 0x9E8) = i + c - 0.121f;

			return;
		}
	}
}

bool CheckInScreen(uintptr_t PlayerController, uintptr_t CurrentActor, int Width, int Height) {
	Vector3 Pos;
	USkeletalMeshComponent::GetBoneLocation(CurrentActor, 66, &Pos);
	AController::WorldToScreen(PlayerController, Pos, &Pos);
	if (CurrentActor)
	{
		if (((Pos.x <= 0 or Pos.x > Width) and (Pos.y <= 0 or Pos.y > Height)) or ((Pos.x <= 0 or Pos.x > Width) or (Pos.y <= 0 or Pos.y > Height))) {
			return false;
		}
		else {
			return true;
		}

	}
}

UObject* RobotoFont;
UObject* K2_DrawLineA;
UObject* K2_DrawTextA; // gay

struct UCanvas_K2_DrawText_Params
{
	UObject* RenderFont;                                               // (Parm, ZeroConstructor, IsPlainOldData)
	FString                                     RenderText;                                               // (Parm, ZeroConstructor)
	Vector2                                   ScreenPosition;                                           // (Parm, ZeroConstructor, IsPlainOldData)
	Vector2                                   Scale;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
	FLinearColor                                RenderColor;                                              // (Parm, ZeroConstructor, IsPlainOldData)
	float                                       Kerning;                                                  // (Parm, ZeroConstructor, IsPlainOldData)
	FLinearColor                                ShadowColor;                                              // (Parm, ZeroConstructor, IsPlainOldData)
	Vector2                                   ShadowOffset;                                             // (Parm, ZeroConstructor, IsPlainOldData)
	bool                                        bCentreX;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
	bool                                        bCentreY;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
	bool                                        bOutlined;                                                // (Parm, ZeroConstructor, IsPlainOldData)
	FLinearColor                                OutlineColor;                                             // (Parm, ZeroConstructor, IsPlainOldData)
};

struct UCanvas_K2_DrawLine_Params
{
	Vector2                            ScreenPositionA;                                                // (Parm, ZeroConstructor, IsPlainOldData)
	Vector2                            ScreenPositionB;                                                // (Parm, ZeroConstructor, IsPlainOldData)
	float							   Thickness;
	FLinearColor                       RenderColor;                                             // (Parm, ZeroConstructor, IsPlainOldData)
};

class UCanvas
{
public:
	VOID K2_DrawLine(Vector2 ScreenPositionA, Vector2 ScreenPositionB, FLinearColor RenderColor, float Thickness)
	{
		UCanvas_K2_DrawLine_Params params;
		params.ScreenPositionA = ScreenPositionA;
		params.ScreenPositionB = ScreenPositionB;
		params.Thickness = Thickness;
		params.RenderColor = RenderColor;

		UObject::ProcessEvent(uintptr_t(this), K2_DrawLineA, &params);
	}

	VOID K2_DrawText(Vector2 ScreenPosition, FString RenderText, FLinearColor RenderColor, bool centered, bool bOutlined)
	{
		UCanvas_K2_DrawText_Params params;
		params.RenderFont = RobotoFont;
		params.RenderText = RenderText;
		params.ScreenPosition = ScreenPosition;
		params.Scale = Vector2{ 0.9f, 0.9f };
		params.RenderColor = RenderColor;
		params.Kerning = false;
		params.ShadowColor = { 0.0f, 0.0f, 0.0f, 1.f };
		params.ShadowOffset = Vector2{ -1, -1 };
		if (centered) {
			params.bCentreX = true;
			params.bCentreY = true;
		}
		else {
			params.bCentreX = false;
			params.bCentreY = false;
		}
		params.bOutlined = bOutlined;
		params.OutlineColor = { 0.0f, 0.0f, 0.0f, 1.f };
		UObject::ProcessEvent((uintptr_t)this, K2_DrawTextA, &params);
	}

	Vector2 K2_StrLen(class UObject* RenderFont, const struct FString& RenderText)
	{
		static UObject* fn = 0; if (!fn) fn = FindObject("K2_StrLen");

		struct
		{
			class UObject* RenderFont;
			struct FString                 RenderText;
			struct Vector2               ReturnValue;
		} params;

		params.RenderFont = RenderFont;
		params.RenderText = RenderText;

		UObject::ProcessEvent((uintptr_t)this, fn, &params);

		return params.ReturnValue;
	}

};

class AHUD
{
public:
	unsigned char _offset[0x268];
	UCanvas* Canvas;
};
#define MP_I 3.14159265358979323846264338327950288419716939937510582f

Vector3 CalcAim(Vector3 roothead)
{
	Vector3 RetVector = { 0,0,0 };
	if (roothead.x == 0 && roothead.y == 0) return Vector3(0, 0, 0);

	Vector3 VectorPos = roothead - CamLoc;

	float distance = VectorPos.Length();
	RetVector.x = -(((float)acos(VectorPos.z / distance) * (float)(180.0f / MP_I)) - 90.f);
	RetVector.y = (float)atan2(VectorPos.y, VectorPos.x) * (float)(180.0f / MP_I);

	return RetVector;
}



class UField : public UObject
{
public:
	class UField* Next;                                                     // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
};

class UStruct : public UField
{
public:
	char                                               pad_0030[0x10];                                           // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	class UStruct* SuperField;                                               // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	class UField* Children;                                                 // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	void* ChildProperties;                                          // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	int32_t                                            PropertySize;                                             // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	int32_t                                            MinAlignment;                                             // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	char                                               pad_0060[0x50];                                           // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
};

bool Object_IsA(UObject* obj, UObject* cmp)
{
	if (!cmp)
		return false;

	UINT i = 0;
	for (auto super = *(uint64_t*)((uint64_t)obj + offsetof(UObject, Class)); super; super = *(uint64_t*)(super + offsetof(UStruct, UStruct::SuperField)))
	{
		if (super == (uint64_t)cmp)
		{
			return true;
		}
		i++;
	}
	return false;
}

UObject* SC_FortPlayerPawn()
{
	static UObject* obj = 0;
	if (!obj)
		obj = (UObject*)FindObject("FortPlayerPawnAthena");
	return obj;
}

void DrawLine(UCanvas* pCanvas, int x1, int y1, int x2, int y2, int thickness, FLinearColor color)
{
	pCanvas->K2_DrawLine(Vector2(x1, y1), Vector2(x2, y2), color, thickness);
}

void DrawCorneredBox(UCanvas* pCanvas, int X, int Y, int W, int H, FLinearColor color, int thickness) {
	float lineW = (W / 3);
	float lineH = (H / 3);

	DrawLine(pCanvas, X, Y, X, Y + lineH, thickness, color);
	DrawLine(pCanvas, X, Y, X + lineW, Y, thickness, color);
	DrawLine(pCanvas, X + W - lineW, Y, X + W, Y, thickness, color);
	DrawLine(pCanvas, X + W, Y, X + W, Y + lineH, thickness, color);
	DrawLine(pCanvas, X, Y + H - lineH, X, Y + H, thickness, color);
	DrawLine(pCanvas, X, Y + H, X + lineW, Y + H, thickness, color);
	DrawLine(pCanvas, X + W - lineW, Y + H, X + W, Y + H, thickness, color);
	DrawLine(pCanvas, X + W, Y + H - lineH, X + W, Y + H, thickness, color);
}

FString GetPlayerName(uintptr_t player)
{
	static UObject* fn = 0; if (!fn) fn = FindObject("GetPlayerName");

	struct
	{
		struct FString                 ReturnValue;
	} params;


	UObject::ProcessEvent(player, fn, &params);

	auto ret = params.ReturnValue;
	return ret;
}

namespace HackColors
{
	FLinearColor AliceBlue = { 0.941176534f, 0.972549081f, 1.000000000f, 1.000000000f };
	FLinearColor AntiqueWhite = { 0.980392218f, 0.921568692f, 0.843137324f, 1.000000000f };
	FLinearColor Aqua = { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor Aquamarine = { 0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f };
	FLinearColor Azure = { 0.941176534f, 1.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor Beige = { 0.960784376f, 0.960784376f, 0.862745166f, 1.000000000f };
	FLinearColor Bisque = { 1.000000000f, 0.894117713f, 0.768627524f, 1.000000000f };
	FLinearColor Black = { 0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor BlackAAAAAAAAAAAAAAA = { 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f };
	FLinearColor BlanchedAlmond = { 1.000000000f, 0.921568692f, 0.803921640f, 1.000000000f };
	FLinearColor Blue = { 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor BlueViolet = { 0.541176498f, 0.168627456f, 0.886274576f, 1.000000000f };
	FLinearColor Brown = { 0.647058845f, 0.164705887f, 0.164705887f, 1.000000000f };
	FLinearColor BurlyWood = { 0.870588303f, 0.721568644f, 0.529411793f, 1.000000000f };
	FLinearColor CadetBlue = { 0.372549027f, 0.619607866f, 0.627451003f, 1.000000000f };
	FLinearColor Chartreuse = { 0.498039246f, 1.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor Chocolate = { 0.823529482f, 0.411764741f, 0.117647067f, 1.000000000f };
	FLinearColor Coral = { 1.000000000f, 0.498039246f, 0.313725501f, 1.000000000f };
	FLinearColor CornflowerBlue = { 0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f };
	FLinearColor Cornsilk = { 1.000000000f, 0.972549081f, 0.862745166f, 1.000000000f };
	FLinearColor Crimson = { 0.862745166f, 0.078431375f, 0.235294133f, 1.000000000f };
	FLinearColor Cyan = { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor DarkBlue = { 0.000000000f, 0.000000000f, 0.545098066f, 1.000000000f };
	FLinearColor DarkCyan = { 0.000000000f, 0.545098066f, 0.545098066f, 1.000000000f };
	FLinearColor DarkGoldenrod = { 0.721568644f, 0.525490224f, 0.043137256f, 1.000000000f };
	FLinearColor DarkGray = { 0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f };
	FLinearColor DarkGreen = { 0.000000000f, 0.392156899f, 0.000000000f, 1.000000000f };
	FLinearColor DarkKhaki = { 0.741176486f, 0.717647076f, 0.419607878f, 1.000000000f };
	FLinearColor DarkMagenta = { 0.545098066f, 0.000000000f, 0.545098066f, 1.000000000f };
	FLinearColor DarkOliveGreen = { 0.333333343f, 0.419607878f, 0.184313729f, 1.000000000f };
	FLinearColor DarkOrange = { 1.000000000f, 0.549019635f, 0.000000000f, 1.000000000f };
	FLinearColor DarkOrchid = { 0.600000024f, 0.196078449f, 0.800000072f, 1.000000000f };
	FLinearColor DarkRed = { 0.545098066f, 0.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor DarkSalmon = { 0.913725555f, 0.588235319f, 0.478431404f, 1.000000000f };
	FLinearColor DarkSeaGreen = { 0.560784340f, 0.737254918f, 0.545098066f, 1.000000000f };
	FLinearColor DarkSlateBlue = { 0.282352954f, 0.239215702f, 0.545098066f, 1.000000000f };
	FLinearColor DarkSlateGray = { 0.184313729f, 0.309803933f, 0.309803933f, 1.000000000f };
	FLinearColor DarkTurquoise = { 0.000000000f, 0.807843208f, 0.819607913f, 1.000000000f };
	FLinearColor DarkViolet = { 0.580392182f, 0.000000000f, 0.827451050f, 1.000000000f };
	FLinearColor DeepPink = { 1.000000000f, 0.078431375f, 0.576470613f, 1.000000000f };
	FLinearColor DeepSkyBlue = { 0.000000000f, 0.749019623f, 1.000000000f, 1.000000000f };
	FLinearColor DimGray = { 0.411764741f, 0.411764741f, 0.411764741f, 1.000000000f };
	FLinearColor DodgerBlue = { 0.117647067f, 0.564705908f, 1.000000000f, 1.000000000f };
	FLinearColor Firebrick = { 0.698039234f, 0.133333340f, 0.133333340f, 1.000000000f };
	FLinearColor FloralWhite = { 1.000000000f, 0.980392218f, 0.941176534f, 1.000000000f };
	FLinearColor ForestGreen = { 0.133333340f, 0.545098066f, 0.133333340f, 1.000000000f };
	FLinearColor Fuchsia = { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor Gainsboro = { 0.862745166f, 0.862745166f, 0.862745166f, 1.000000000f };
	FLinearColor GhostWhite = { 0.972549081f, 0.972549081f, 1.000000000f, 1.000000000f };
	FLinearColor Gold = { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
	FLinearColor Goldenrod = { 0.854902029f, 0.647058845f, 0.125490203f, 1.000000000f };
	FLinearColor Gray = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
	FLinearColor Green = { 0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f };
	FLinearColor GreenYellow = { 0.678431392f, 1.000000000f, 0.184313729f, 1.000000000f };
	FLinearColor Honeydew = { 0.941176534f, 1.000000000f, 0.941176534f, 1.000000000f };
	FLinearColor HotPink = { 1.000000000f, 0.411764741f, 0.705882370f, 1.000000000f };
	FLinearColor IndianRed = { 0.803921640f, 0.360784322f, 0.360784322f, 1.000000000f };
	FLinearColor Indigo = { 0.294117659f, 0.000000000f, 0.509803951f, 1.000000000f };
	FLinearColor Ivory = { 1.000000000f, 1.000000000f, 0.941176534f, 1.000000000f };
	FLinearColor Khaki = { 0.941176534f, 0.901960850f, 0.549019635f, 1.000000000f };
	FLinearColor Lavender = { 0.901960850f, 0.901960850f, 0.980392218f, 1.000000000f };
	FLinearColor LavenderBlush = { 1.000000000f, 0.941176534f, 0.960784376f, 1.000000000f };
	FLinearColor LawnGreen = { 0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f };
	FLinearColor LemonChiffon = { 1.000000000f, 0.980392218f, 0.803921640f, 1.000000000f };
	FLinearColor LightBlue = { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
	FLinearColor LightCoral = { 0.941176534f, 0.501960814f, 0.501960814f, 1.000000000f };
	FLinearColor LightCyan = { 0.878431439f, 1.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor LightGoldenrodYellow = { 0.980392218f, 0.980392218f, 0.823529482f, 1.000000000f };
	FLinearColor LightGreen = { 0.564705908f, 0.933333397f, 0.564705908f, 1.000000000f };
	FLinearColor LightGray = { 0.827451050f, 0.827451050f, 0.827451050f, 1.000000000f };
	FLinearColor LightPink = { 1.000000000f, 0.713725507f, 0.756862819f, 1.000000000f };
	FLinearColor LightSalmon = { 1.000000000f, 0.627451003f, 0.478431404f, 1.000000000f };
	FLinearColor LightSeaGreen = { 0.125490203f, 0.698039234f, 0.666666687f, 1.000000000f };
	FLinearColor LightSkyBlue = { 0.529411793f, 0.807843208f, 0.980392218f, 1.000000000f };
	FLinearColor LightSlateGray = { 0.466666698f, 0.533333361f, 0.600000024f, 1.000000000f };
	FLinearColor LightSteelBlue = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
	FLinearColor LightYellow = { 1.000000000f, 1.000000000f, 0.878431439f, 1.000000000f };
	FLinearColor Lime = { 0.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor LimeGreen = { 0.196078449f, 0.803921640f, 0.196078449f, 1.000000000f };
	FLinearColor Linen = { 0.980392218f, 0.941176534f, 0.901960850f, 1.000000000f };
	FLinearColor Magenta = { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor Maroon = { 0.501960814f, 0.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor MediumAquamarine = { 0.400000036f, 0.803921640f, 0.666666687f, 1.000000000f };
	FLinearColor MediumBlue = { 0.000000000f, 0.000000000f, 0.803921640f, 1.000000000f };
	FLinearColor MediumOrchid = { 0.729411781f, 0.333333343f, 0.827451050f, 1.000000000f };
	FLinearColor MediumPurple = { 0.576470613f, 0.439215720f, 0.858823597f, 1.000000000f };
	FLinearColor MediumSeaGreen = { 0.235294133f, 0.701960802f, 0.443137288f, 1.000000000f };
	FLinearColor MediumSlateBlue = { 0.482352972f, 0.407843173f, 0.933333397f, 1.000000000f };
	FLinearColor MediumSpringGreen = { 0.000000000f, 0.980392218f, 0.603921592f, 1.000000000f };
	FLinearColor MediumTurquoise = { 0.282352954f, 0.819607913f, 0.800000072f, 1.000000000f };
	FLinearColor MediumVioletRed = { 0.780392230f, 0.082352944f, 0.521568656f, 1.000000000f };
	FLinearColor MidnightBlue = { 0.098039225f, 0.098039225f, 0.439215720f, 1.000000000f };
	FLinearColor MintCream = { 0.960784376f, 1.000000000f, 0.980392218f, 1.000000000f };
	FLinearColor MistyRose = { 1.000000000f, 0.894117713f, 0.882353008f, 1.000000000f };
	FLinearColor Moccasin = { 1.000000000f, 0.894117713f, 0.709803939f, 1.000000000f };
	FLinearColor NavajoWhite = { 1.000000000f, 0.870588303f, 0.678431392f, 1.000000000f };
	FLinearColor Navy = { 0.000000000f, 0.000000000f, 0.501960814f, 1.000000000f };
	FLinearColor OldLace = { 0.992156923f, 0.960784376f, 0.901960850f, 1.000000000f };
	FLinearColor Olive = { 0.501960814f, 0.501960814f, 0.000000000f, 1.000000000f };
	FLinearColor OliveDrab = { 0.419607878f, 0.556862772f, 0.137254909f, 1.000000000f };
	FLinearColor Orange = { 1.000000000f, 0.647058845f, 0.000000000f, 1.000000000f };
	FLinearColor OrangeRed = { 1.000000000f, 0.270588249f, 0.000000000f, 1.000000000f };
	FLinearColor Orchid = { 0.854902029f, 0.439215720f, 0.839215755f, 1.000000000f };
	FLinearColor PaleGoldenrod = { 0.933333397f, 0.909803987f, 0.666666687f, 1.000000000f };
	FLinearColor PaleGreen = { 0.596078455f, 0.984313786f, 0.596078455f, 1.000000000f };
	FLinearColor PaleTurquoise = { 0.686274529f, 0.933333397f, 0.933333397f, 1.000000000f };
	FLinearColor PaleVioletRed = { 0.858823597f, 0.439215720f, 0.576470613f, 1.000000000f };
	FLinearColor PapayaWhip = { 1.000000000f, 0.937254965f, 0.835294187f, 1.000000000f };
	FLinearColor PeachPuff = { 1.000000000f, 0.854902029f, 0.725490212f, 1.000000000f };
	FLinearColor Peru = { 0.803921640f, 0.521568656f, 0.247058839f, 1.000000000f };
	FLinearColor Pink = { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
	FLinearColor Plum = { 0.866666734f, 0.627451003f, 0.866666734f, 1.000000000f };
	FLinearColor PowderBlue = { 0.690196097f, 0.878431439f, 0.901960850f, 1.000000000f };
	FLinearColor Purple = { 0.501960814f, 0.000000000f, 0.501960814f, 1.000000000f };
	FLinearColor Red = { 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor RosyBrown = { 0.737254918f, 0.560784340f, 0.560784340f, 1.000000000f };
	FLinearColor RoyalBlue = { 0.254901975f, 0.411764741f, 0.882353008f, 1.000000000f };
	FLinearColor SaddleBrown = { 0.545098066f, 0.270588249f, 0.074509807f, 1.000000000f };
	FLinearColor Salmon = { 0.980392218f, 0.501960814f, 0.447058856f, 1.000000000f };
	FLinearColor SandyBrown = { 0.956862807f, 0.643137276f, 0.376470625f, 1.000000000f };
	FLinearColor SeaGreen = { 0.180392161f, 0.545098066f, 0.341176480f, 1.000000000f };
	FLinearColor SeaShell = { 1.000000000f, 0.960784376f, 0.933333397f, 1.000000000f };
	FLinearColor Sienna = { 0.627451003f, 0.321568638f, 0.176470593f, 1.000000000f };
	FLinearColor Silver = { 0.752941251f, 0.752941251f, 0.752941251f, 1.000000000f };
	FLinearColor SkyBlue = { 0.529411793f, 0.807843208f, 0.921568692f, 1.000000000f };
	FLinearColor SlateBlue = { 0.415686309f, 0.352941185f, 0.803921640f, 1.000000000f };
	FLinearColor SlateGray = { 0.439215720f, 0.501960814f, 0.564705908f, 1.000000000f };
	FLinearColor Snow = { 1.000000000f, 0.980392218f, 0.980392218f, 1.000000000f };
	FLinearColor SpringGreen = { 0.000000000f, 1.000000000f, 0.498039246f, 1.000000000f };
	FLinearColor SteelBlue = { 0.274509817f, 0.509803951f, 0.705882370f, 1.000000000f };
	FLinearColor Tan = { 0.823529482f, 0.705882370f, 0.549019635f, 1.000000000f };
	FLinearColor Teal = { 0.000000000f, 0.501960814f, 0.501960814f, 1.000000000f };
	FLinearColor Thistle = { 0.847058892f, 0.749019623f, 0.847058892f, 1.000000000f };
	FLinearColor Tomato = { 1.000000000f, 0.388235331f, 0.278431386f, 1.000000000f };
	FLinearColor Transparent = { 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f };
	FLinearColor Turquoise = { 0.250980407f, 0.878431439f, 0.815686345f, 1.000000000f };
	FLinearColor Violet = { 0.933333397f, 0.509803951f, 0.933333397f, 1.000000000f };
	FLinearColor Wheat = { 0.960784376f, 0.870588303f, 0.701960802f, 1.000000000f };
	FLinearColor White = { 1.000000000f, 1.000000000f, 1.0f, 1.000000000f };
	FLinearColor WhiteSmoke = { 0.960784376f, 0.960784376f, 0.960784376f, 1.000000000f };
	FLinearColor Yellow = { 1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor YellowGreen = { 0.603921592f, 0.803921640f, 0.196078449f, 1.000000000f };
};

struct APlayerController_FOV_Params
{
	float                                              NewFOV;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
};

void FOV(uintptr_t _this, float NewFOV)
{
	static UObject* fn = NULL;
	if (!fn)
		fn = FindObject("FOV");

	APlayerController_FOV_Params params;
	params.NewFOV = NewFOV;

	UObject::ProcessEvent(_this, fn, &params);
}

struct AActor_K2_TeleportTo_Params
{
	Vector3										DestLocation;                                             // (Parm, ZeroConstructor, IsPlainOldData)
	Vector3										DestRotation;                                             // (Parm, ZeroConstructor, IsPlainOldData)
	bool										ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};

BOOL K2_TeleportTo(uintptr_t CurrentActor, const Vector3& DestLocation, const Vector3& DestRotation)
{
	static UObject* fn = NULL;
	if (!fn)
		fn = FindObject("K2_TeleportTo");
	AActor_K2_TeleportTo_Params params;
	params.DestLocation = DestLocation;
	params.DestRotation = DestRotation;

	UObject::ProcessEvent(CurrentActor, fn, &params);

	return params.ReturnValue;
}

void DrawCircle(UCanvas* canvas, int x, int y, int radius, int numsides, FLinearColor color)
{
	float Step = MP_I * 2.0 / numsides;
	int Count = 0;
	FVector2D V[128];
	for (float a = 0; a < MP_I * 2.0; a += Step)
	{
		float X1 = radius * cosf(a) + x;
		float Y1 = radius * sinf(a) + y;
		float X2 = radius * cosf(a + Step) + x;
		float Y2 = radius * sinf(a + Step) + y;
		V[Count].X = X1;
		V[Count].Y = Y1;
		V[Count + 1].X = X2;
		V[Count + 1].Y = Y2;
		canvas->K2_DrawLine(Vector2({ V[Count].X, V[Count].Y }), Vector2({ X2, Y2 }), color, 1.0f);
	}
}

bool AController_SetControlRotation(UCanvas* funccanvas, Vector3 rot, uint64_t controller, uintptr_t LocalPawn)
{
	auto VTable = *(uintptr_t*)(controller);
	if (!VTable)
		return false;

	auto func = (*(void(__fastcall**)(uint64_t, void*))(VTable + 0x6F8));
	func(controller, (void*)&rot);

	return true;
}

#define M_RADPI	57.295779513082f
__forceinline Vector3 calc_angle(Vector3& Src, Vector3& Dst)
{
	Vector3 Delta = Src - Dst;
	Vector3 AimAngles;
	float Hyp = sqrt(powf(Delta.x, 2.f) + powf(Delta.y, 2.f));
	AimAngles.y = atanf(Delta.y / Delta.x) * M_RADPI;
	AimAngles.x = (atanf(Delta.z / Hyp) * M_RADPI) * -1.f;
	if (Delta.x >= 0.f) AimAngles.y += 180.f;
	AimAngles.z = 0.f;
	return AimAngles;
}

void SetViewAngles(UCanvas* funccanvas, Vector3 ang, uintptr_t GController, uintptr_t LocalPawn)
{
	auto angls = Clamp(ang);
	angls.z = 0.0f;
	AController_SetControlRotation(funccanvas, ang, GController, LocalPawn);
}

#define M_PI 3.14159265358979323846264338327950288419716939937510582f

double GetCrossDistance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

std::string TextFormat(const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);

	char buffer[2048];
	vsprintf(buffer, format, argptr);

	va_end(argptr);

	return buffer;
}