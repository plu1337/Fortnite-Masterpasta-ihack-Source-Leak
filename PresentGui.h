#pragma once
#include "stdafx.h"
#include "xorstr.h"

static wchar_t* s2wc(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

UCanvas* pCanvas;

namespace settings
{
	bool Memory = true;
	bool XboxController = false;
	bool PerfectSil = false;
	bool Sticky = false;
	bool teamcheck = true;
	bool randomaim = false;
	bool dbnocheck = false;
	bool pSilent = false;
	bool FovCircle = true;
	float FovSlide = 0.f;

	bool Box = true;
	bool Skeleton = true;
	bool Name = false;
	bool deutschescrosshair = false;
	bool crosshair = true;
	bool Distance = false;
	bool Weapon = false;
	bool Ammo = false;
	bool TargetLine = false;
	bool LootESP = false;
	bool ChestESP = false;
	bool VehicleESP = false;
	bool RadarESP = false;

	bool NoSpread = false;
	bool NoBloom = false;
	bool NoReload = false;
	bool InstantRevive = false;
	bool AimWhileJumping = false;
	bool RapidFire = false;
	bool ProjectileTeleport = false; //
	bool RocketTeleport = false; //
	bool CameraFov = false;
	float CamerFovSlider = 0.f;
}

static void drawFilledRect(Vector2 initial_pos, float w, float h, FLinearColor color)
{
	for (float i = 0.0f; i < h; i += 1.0f)
		pCanvas->K2_DrawLine(Vector2{ initial_pos.X, initial_pos.Y + i }, Vector2{ initial_pos.X + w, initial_pos.Y + i }, color, 1.0f);
}

static void drawRect(Vector2 initial_pos, float w, float h, FLinearColor color, float thickness = 1.f)
{
	pCanvas->K2_DrawLine(initial_pos, Vector2(initial_pos.X + w, initial_pos.Y), color, thickness);
	pCanvas->K2_DrawLine(initial_pos, Vector2(initial_pos.X, initial_pos.Y + h), color, thickness);
	pCanvas->K2_DrawLine(Vector2(initial_pos.X + w, initial_pos.Y), Vector2(initial_pos.X + w, initial_pos.Y + h), color, thickness);
	pCanvas->K2_DrawLine(Vector2(initial_pos.X, initial_pos.Y + h), Vector2(initial_pos.X + w, initial_pos.Y + h), color, thickness);
}

namespace menu_framework {
	void group_box(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, const wchar_t* string, bool show_label);
	void tab(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, const wchar_t* string, std::int32_t& tab, std::int32_t count, bool show_outline);
	void check_box(std::int32_t x, std::int32_t y, std::int32_t position, const wchar_t* string, bool& value);
	void slider(std::int32_t x, std::int32_t y, std::int32_t position, const std::string& string, float& value, float min_value, float max_value);
	void menu_movement(std::int32_t& x, std::int32_t& y, std::int32_t w, std::int32_t h);

	inline bool should_drag = false;
	inline bool should_move = false;
};

POINT cursor;
POINT cursor_corrected;

void menu_framework::group_box(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, const wchar_t* string, bool show_label) {
	drawFilledRect(Vector2(x, y), w, h, FLinearColor(0.004060814f, 0.004060814f, 0.004060814f, 1.000000000f));
	drawRect(Vector2(x, y), w, h, FLinearColor(0.005060814f, 0.005060814f, 0.005060814f, 1.000000000f));
	if (show_label)
		pCanvas->K2_DrawText(Vector2(x + 2, y - 12), string, HackColors::White, false, false);
}

void menu_framework::tab(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, const wchar_t* string, std::int32_t& tab, std::int32_t count, bool show_outline) {
	GetCursorPos(&cursor);

	if ((cursor.x > x) && (cursor.x < x + w) && (cursor.y > y) && (cursor.y < y + h) && (GetAsyncKeyState(VK_LBUTTON) & 1))
		tab = count;

	drawRect(Vector2(x, y), w, h, tab == count ? HackColors::Cyan : FLinearColor(0.004060814f, 0.004060814f, 0.004060814f, 1.000000000f));

	pCanvas->K2_DrawText({ (float)x - 50 / 2 + 50,(float)y + h / 2 - 8 }, string, tab == count ? HackColors::Cyan : HackColors::White, false, false);
}

void menu_framework::check_box(std::int32_t x, std::int32_t y, std::int32_t position, const wchar_t* string, bool& value) {
	GetCursorPos(&cursor);

	int w = 12, h = 14;

	if ((cursor.x > position) && (cursor.x < position + w) && (cursor.y > y) && (cursor.y < y + h) && GetAsyncKeyState(VK_LBUTTON) & 1)
		value = !value;

	drawFilledRect(Vector2(position, y), w, h, HackColors::SlateGray);
	if (value)
	{
		drawFilledRect(Vector2(position + 3, y + 3), 6, 8, HackColors::Black);
	}
	pCanvas->K2_DrawText(Vector2(x + 25, y - 1), string, HackColors::White, false, false);
}

void menu_framework::slider(std::int32_t x, std::int32_t y, std::int32_t position, const std::string& string, float& value, float min_value, float max_value) {
	GetCursorPos(&cursor);

	int ix = x + 140;
	int yi = y + 4;

	if ((cursor.x > ix) && (cursor.x < ix + position) && (cursor.y > yi) && (cursor.y < yi + 6) && (GetAsyncKeyState(VK_LBUTTON)))
		value = (cursor.x - ix) / (float(position) / float(max_value));

	drawFilledRect(Vector2(ix, yi), position, 6, FLinearColor(0.008060814f, 0.008060814f, 0.008060814f, 1.000000000f));
	drawFilledRect(Vector2(ix, yi), value * (float(position) / float(max_value)), 6, HackColors::Cyan);

	pCanvas->K2_DrawText(Vector2(x + 2, y - 1), s2wc((std::stringstream{ } << string << ": " << std::setprecision(3) << value).str().c_str()), HackColors::White, false, false);
}

void menu_framework::menu_movement(std::int32_t& x, std::int32_t& y, std::int32_t w, std::int32_t h) {
	GetCursorPos(&cursor);

	if (GetAsyncKeyState(VK_LBUTTON) < 0 && (cursor.x > x && cursor.x < x + w && cursor.y > y && cursor.y < y + h)) {
		should_drag = true;

		if (!should_move) {
			cursor_corrected.x = cursor.x - x;
			cursor_corrected.y = cursor.y - y;
			should_move = true;
		}
	}

	if (should_drag) {
		x = cursor.x - cursor_corrected.x;
		y = cursor.y - cursor_corrected.y;
	}

	if (GetAsyncKeyState(VK_LBUTTON) == 0) {
		should_drag = false;
		should_move = false;
	}
}

namespace variables {
	namespace menu {
		inline bool opened = false;
		inline int x = 140, y = 500;
		inline int w = 440, h = 630;
	}
}

Vector2 CursorPos()
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	return Vector2{ (float)cursorPos.x, (float)cursorPos.y };
}

static void Draw_Cursor(bool toogle)
{
	if (toogle)
	{
		Vector2 cursorPos = CursorPos();
		drawFilledRect(Vector2(cursorPos.X, cursorPos.Y), 6.f, 6.f, HackColors::Red);
	}
}

auto do_frame = [&](std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, FLinearColor bg, FLinearColor header_text, FLinearColor header_line, const std::string& name) {
	drawFilledRect(Vector2(x, y), w, h, bg);
	pCanvas->K2_DrawText(Vector2(x + 10, y + 10), s2wc("Build date " __DATE__ " " __TIME__), HackColors::White, false, false);
};


#define M_PI 3.14159265358979323846264338327950288419716939937510582f
#define D2R(d) (d / 180.f) * M_PI
#define MAX_SEGMENTS 180

void Render_Line(Vector2 one, Vector2 two, FLinearColor color)
{
	pCanvas->K2_DrawLine(one, two, color, 1 );
}

void Render_Circle(Vector2 pos, int r, FLinearColor color)
{
	float circum = M_PI * 2.f * r;
	int seg = ceilf(circum);

	if (seg > MAX_SEGMENTS) seg = MAX_SEGMENTS;

	float theta = 0.f;
	float step = 180.f / seg;

	for (size_t i = 0; i < seg; ++i)
	{
		theta = i * step;
		auto delta = Vector2(round(r * sin(D2R(theta))), round(r * cos(D2R(theta))));
		Render_Line(pos + delta, pos - delta, color);
	}
}

void Render_Clear(Vector2 one, Vector2 two, FLinearColor color)
{
	for (auto x = one.X; x < two.X; x += 1.f)
	{
		pCanvas->K2_DrawLine(Vector2(x, one.Y), Vector2(x, two.Y), color, 1.f);
	}
}


void Render_PointArray(size_t count, Vector2* ary, FLinearColor color)
{
	for (size_t i = 1; i < count; ++i)
		Render_Line(ary[i - 1], ary[i], color);
}

void Render_CircleOutline(Vector2 pos, int r, FLinearColor outline)
{
	float circum = M_PI * 2.f * r;
	int seg = ceilf(circum);

	if (seg > MAX_SEGMENTS) seg = MAX_SEGMENTS;

	float theta = 0.f;
	float step = 360.f / seg;

	Vector2 points[MAX_SEGMENTS] = {};

	for (size_t i = 0; i < seg; ++i)
	{
		theta = i * step;
		points[i] = Vector2(pos.X + roundf(r * sin(D2R(theta))), pos.Y + roundf(r * cos(D2R(theta))));
	}

	Render_PointArray(seg, points, outline);
}

void Render_CircleOutlined(Vector2 pos, int r, FLinearColor fill, FLinearColor outline)
{
	Render_Circle(pos, r, fill);
	Render_CircleOutline(pos, r, outline);
}

void Render_MenuText(const wchar_t* text, FLinearColor col, Vector2 loc, bool centered)
{
	pCanvas->K2_DrawText(loc, text, HackColors::White, false, false);
}

void Render_Slider(const wchar_t* name, float minimum, float maximum, float* val, Vector2 loc)
{
	GetCursorPos(&cursor);
	auto kinda_white = HackColors::White;

	constexpr float _width = 180 + 19;
	Render_MenuText(name, HackColors::White, Vector2(loc.X + 6, loc.Y), false);

	loc.X += 6.f;
	
	bool hover = cursor.x > loc.X && cursor.x < (cursor.x + _width) && cursor.y > loc.Y && cursor.y < (cursor.y + 30);
	if (GetAsyncKeyState(VK_LBUTTON) && hover)
	{
		float ratio = (float)(cursor.x - loc.X) / _width;
		if (ratio < 0.f) ratio = 0.f;
		if (ratio > 1.f) ratio = 1.f;
		*val = minimum + ((maximum - minimum) * ratio);
	}

	int xpos = ((*val - minimum) / (maximum - minimum)) * _width;

	loc.Y += 24.f;

	Render_Circle(loc, 3, HackColors::Cyan);
	Render_Clear(Vector2(loc.X, loc.Y - 3), Vector2(loc.X + xpos, loc.Y + 3), HackColors::Cyan);

	Render_Clear(Vector2(loc.X + xpos, loc.Y - 3), Vector2(loc.X + _width, loc.Y + 3), kinda_white);
	Render_Circle(Vector2(loc.X + _width, loc.Y), 3, kinda_white);

	Render_CircleOutlined(Vector2(loc.X + xpos, loc.Y), 5, HackColors::White, HackColors::Black);

	loc.Y -= 24.f;

	Render_MenuText(s2wc(TextFormat("%0.1f", *val).c_str()), HackColors::White, Vector2(loc.X + _width - 30.0f, loc.Y + 33), false);

	loc.X -= 6.f;
	loc.Y += 35.0f;
	loc.Y += 13.0f;
}

bool nenem = false;;
bool nenem1 = false;;
bool nenem2 = false;;
bool nenem3 = false;;
bool nenem4 = false;;
bool nenem5 = false;;
bool nenem6 = false;;
bool nenem7 = false;;
bool nenem8 = false;;
bool nenem9 = false;;
bool nenem10 = false;;
bool nenem11 = false;;
bool nenem12 = false;;
bool nenem13 = false;;
bool nenem14 = false;;
bool nenem15 = false;;
bool nenem16 = false;;
bool nenem17 = false;;
bool nenem18 = false;;
bool nenem19 = false;;
bool nenem20 = false;;
bool nenem21 = false;;
bool nenem22 = false;;
bool nenem23 = false;;
bool nenem24 = false;;
bool nenem25 = false;;
bool nenem26 = false;;
float noahspeck = 0;
float noahspeck1 = 100;
float noahspeck2 = 50;
float noahspeck3 = 80;
float noahspeck4 = 30;
float noahspeck5 = 90;
float noahspeck6 = 120;

void DrawGUI(UCanvas* canvas, bool menu_g)
{
	pCanvas = canvas;

	static int tab = 0;
	if (menu_g)
	{
		do_frame(variables::menu::x, variables::menu::y, variables::menu::w, variables::menu::h, HackColors::Black, FLinearColor(0.004060814f, 0.004060814f, 0.004060814f, 1.000000000f), FLinearColor(0.005060814f, 0.005060814f, 0.005060814f, 1.000000000f), "kernalcheats.com");
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 50, variables::menu::x + 15, L"loot     [F1]", nenem);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 70, variables::menu::x + 15, L"chests     [F2]", nenem1);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 90, variables::menu::x + 15, L"snaplines    [F3]", nenem2);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 110, variables::menu::x + 15, L"aimbot     [F4]", nenem3);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 130, variables::menu::x + 15, L"nospread     [F5]", nenem4);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 150, variables::menu::x + 15, L"instantreload   [F7]", nenem5);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 170, variables::menu::x + 15, L"[own risk] psilent [ENTER]", nenem6);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 190, variables::menu::x + 15, L"[own risk] fov changer", nenem7);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 210, variables::menu::x + 15, L"NEW SILENT METHOD", nenem8);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 230, variables::menu::x + 15, L"SPINBOT", nenem9);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 250, variables::menu::x + 15, L"restore rotation", nenem10);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 270, variables::menu::x + 15, L"rifts", nenem11);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 290, variables::menu::x + 15, L"supplydrops", nenem12);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 310, variables::menu::x + 15, L"visible only", nenem);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 330, variables::menu::x + 15, L"snaplines visible only", nenem13);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 350, variables::menu::x + 15, L"Player Name ESP", nenem14);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 370, variables::menu::x + 15, L"player weapon", nenem15);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 390, variables::menu::x + 15, L"traps", nenem16);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 410, variables::menu::x + 15, L"bullets/granades", nenem17);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 430, variables::menu::x + 15, L"vehicles", nenem18);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 450, variables::menu::x + 15, L"player boxes", nenem19);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 470, variables::menu::x + 15, L"skeleton", nenem20);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 490, variables::menu::x + 15, L"neck aim", nenem21);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 510, variables::menu::x + 15, L"neck aim on sr", nenem22);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 530, variables::menu::x + 15, L"draw fov", nenem23);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 550, variables::menu::x + 15, L"[own risk] instarevive (E)", nenem24);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 570, variables::menu::x + 15, L"[own risk] adjust boat rotation", nenem25);
		menu_framework::check_box(variables::menu::x + 15, variables::menu::y + 590, variables::menu::x + 15, L"[own risk] noclip (sniper only)", nenem26);





		// sliders gaylmfao1

		Render_Slider(L"esp distance", 0, 367, &noahspeck, Vector2(variables::menu::x + 205, variables::menu::y + 45));
		Render_Slider(L"loot distance", 0, 366, &noahspeck1, Vector2(variables::menu::x + 205, variables::menu::y + 85));
		Render_Slider(L"chest distance", 0, 365, &noahspeck2, Vector2(variables::menu::x + 205, variables::menu::y + 125));
		Render_Slider(L"snapline distance", 0, 364, &noahspeck3, Vector2(variables::menu::x + 205, variables::menu::y + 165));
		Render_Slider(L"fov (deg)", 0, 363, &noahspeck4, Vector2(variables::menu::x + 205, variables::menu::y + 205));
		Render_Slider(L"box fill", 0, 362, &noahspeck5, Vector2(variables::menu::x + 205, variables::menu::y + 245));
		Render_Slider(L"smooth", 0, 361, &noahspeck6, Vector2(variables::menu::x + 205, variables::menu::y + 285));
	}

	menu_framework::menu_movement(variables::menu::x, variables::menu::y, variables::menu::w, 30);
	Draw_Cursor(menu_g);
}