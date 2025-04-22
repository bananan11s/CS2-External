#pragma once
#include <d3d11.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

class Overlay {
public:
	// functions for the d3d11 device
	static bool CreateDevice();
	static void DestroyDevice();

	// functions for the window
	static void CreateOverlay();
	static void DestroyOverlay();

	// functions for ImGui
	static bool CreateImGui();
	static void DestroyImGui();

	// for use inside of your main loop to render.
	static void StartRender();
	static void EndRender();

	// i'm using this for the menu specifically, but you could render ESP and shit like that in here if you wanted to.
	// as long as you're calling ImGui::GetBackgroundDrawList() between Start and End render you're fine to do what you want :)
	static void Render();

	// winapi window requirements
	static HWND overlay;
	static WNDCLASSEX wc;

	// small helper functions for the SetForeground function
	static bool IsWindowInForeground(HWND window) { return GetForegroundWindow() == window; }
	static bool BringToForeground(HWND window) { return SetForegroundWindow(window); }

	// sets the window to the foreground
	static void SetForeground(HWND window);

	static ID3D11Device* device;
	static ID3D11DeviceContext* device_context;
	static IDXGISwapChain* swap_chain;
	static ID3D11RenderTargetView* render_targetview;

	// if we should render menu on top of the overlay.
	inline static bool RenderMenu;

	// if we should run the overlay at all
	inline static bool shouldRun;
};

inline Overlay overlay;

extern bool esp;
extern bool drawBoxes;
extern bool drawDistanceText;
extern bool drawLines;
extern int linePosition;
extern  float customColor[4];
extern float teammateColor[4];
extern bool drawForTeammates;
extern bool drawHealthBar;
extern bool drawArmorBar;
extern int displayMode;
extern int boxType;
extern bool drawBones;

extern bool esp;
extern bool drawBoxes;
extern bool drawDistanceText;
extern bool drawLines;
extern bool drawForTeammates;
extern bool drawHealthBar;
extern bool drawArmorBar;
extern int boxType;
extern int linePosition;
extern float aimbotFOV;
extern float aimbotSmooth;
extern int displayMode;
extern float fovColor[4]; // RGBA цвет FOV круга
extern bool aimbotEnabled;
extern float aimbotYOffset; // Смещение прицела по высоте (по Y)
extern bool drawEspFill;
extern  float boxFillAlpha;
extern ImVec4 highHealthColor;
extern ImVec4 lowHealthColor;
extern ImVec4 highArmorColor;  // Цвет для высокой брони
extern ImVec4 lowArmorColor;
extern float visibleColor[4];
extern bool gradientHealthBar;  // Включить/выключить градиент для полосы здоровья
extern bool gradientArmorBar;
extern bool drawOutline;
extern bool triggerbotEnabled; // Включение триггербота
extern float triggerDelay;     // Настройка задержки между выстрелами
extern bool drawNames;
extern bool newAimbotEnabled;
extern int selectedKey;
extern int defaultKey;
extern bool aimbotNoKeyBind;
extern bool useKeyBind;  // Новый флаг для управления привязкой клавиш
