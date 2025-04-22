#include "window.hpp"

#include <dwmapi.h>
#include <stdio.h>
#include "Includes.h"

//game stuff
#include "Memory.h"
#include "GameEngine.h"
#include "Offsets.h"
//
#include <chrono>
#include "SimpleIni.h"

// Структура конфигурации
struct Config {
	bool esp;
	bool drawBoxes;
	bool drawDistanceText;
	bool drawLines;
	bool drawForTeammates;
	bool drawHealthBar;
	bool drawArmorBar;
	bool aimbotEnabled;
	int boxType;
	int linePosition;
	float aimbotFOV;
	float aimbotSmooth;
	int displayMode;
	float fovColor[4]; // Цвет круга FOV
	float espColor[4]; // Цвет ESP
	float teammateColor[4]; // Цвет тиммейтов
	float aimbotYOffset;
	bool drawEspFill;           // Включить/выключить заливку ESP
	float boxFillAlpha;
	float visibleColor[4]; // Цвет видимых объектов
	float highHealthColor[4]; // Цвет высокой HP
	float lowHealthColor[4];  // Цвет низкой HP
	float highArmorColor[4];  // Цвет высокой брони
	float lowArmorColor[4];   // Цвет низкой брони
	bool gradientHealthBar;  // Включить/выключить градиент для полосы здоровья
	bool gradientArmorBar;
	 bool drawOutline;
	 bool triggerbotEnabled;  // Включение триггербота
	 float triggerDelay;      // Задержка триггербота
	 bool aimbotNoKeyBind;
	 bool useKeyBind; // Булевое значение для привязки клавиши аимбота
};

Config config;

// Массив имен слотов и их файлов
std::vector<std::string> configSlots = { "Slot 1", "Slot 2", "Slot 3" };
std::vector<std::string> configFiles = { "config1.ini", "config2.ini", "config3.ini" };
int currentSlot = 0;  // Индекс текущего слота
bool isRenaming = false;  // Флаг переименования
char newConfigName[256] = "";  // Массив для нового имени конфигурации

// Функция сохранения конфигурации в файл
void SaveConfig(const std::string& filename = "config.ini") {
	// Копируем текущие настройки
	config.esp = esp;
	config.drawBoxes = drawBoxes;
	config.drawDistanceText = drawDistanceText;
	config.drawLines = drawLines;
	config.drawForTeammates = drawForTeammates;
	config.drawHealthBar = drawHealthBar;
	config.drawArmorBar = drawArmorBar;
	config.aimbotEnabled = aimbotEnabled;
	config.aimbotFOV = aimbotFOV;
	config.aimbotSmooth = aimbotSmooth;
	config.aimbotYOffset = aimbotYOffset;
	config.boxType = boxType;
	config.linePosition = linePosition;
	config.displayMode = displayMode;
	// Копируем новые параметры
	config.drawEspFill = drawEspFill;
	config.boxFillAlpha = boxFillAlpha;
	config.gradientHealthBar = gradientHealthBar;
	config.gradientArmorBar = gradientArmorBar;
	config.drawOutline = drawOutline; // Сохраняем drawOutline
	// Сохраняем параметры триггербота
	config.triggerbotEnabled = triggerbotEnabled;
	config.triggerDelay = triggerDelay;
	config.aimbotNoKeyBind = aimbotNoKeyBind;
	config.useKeyBind = useKeyBind;

	// Копируем цвета
	memcpy(config.fovColor, fovColor, sizeof(fovColor));
	memcpy(config.espColor, customColor, sizeof(customColor));
	memcpy(config.teammateColor, teammateColor, sizeof(teammateColor));
	memcpy(config.highHealthColor, (float*)&highHealthColor, sizeof(config.highHealthColor));
	memcpy(config.lowHealthColor, (float*)&lowHealthColor, sizeof(config.lowHealthColor));
	memcpy(config.highArmorColor, (float*)&highArmorColor, sizeof(config.highArmorColor));
	memcpy(config.lowArmorColor, (float*)&lowArmorColor, sizeof(config.lowArmorColor));
	config.gradientHealthBar = gradientHealthBar;
	config.gradientArmorBar = gradientArmorBar;
	memcpy(config.visibleColor, visibleColor, sizeof(visibleColor)); // Copy the visibleColor array
	// Записываем в INI-файл
	CSimpleIniA ini;
	
	// Then save it to the INI file
	ini.SetDoubleValue("VisibleColor", "R", config.visibleColor[0]);
	ini.SetDoubleValue("VisibleColor", "G", config.visibleColor[1]);
	ini.SetDoubleValue("VisibleColor", "B", config.visibleColor[2]);
	ini.SetDoubleValue("VisibleColor", "A", config.visibleColor[3]);
	ini.SetBoolValue("Keybinds", "UseKeyBind", config.useKeyBind);
	ini.SetBoolValue("Settings", "ESP", config.esp);
	ini.SetBoolValue("Settings", "DrawBoxes", config.drawBoxes);
	ini.SetBoolValue("Settings", "DrawDistanceText", config.drawDistanceText);
	ini.SetBoolValue("Settings", "DrawLines", config.drawLines);
	ini.SetBoolValue("Settings", "DrawForTeammates", config.drawForTeammates);
	ini.SetBoolValue("Settings", "DrawHealthBar", config.drawHealthBar);
	ini.SetBoolValue("Settings", "DrawArmorBar", config.drawArmorBar);
	ini.SetBoolValue("Settings", "AimbotEnabled", config.aimbotEnabled);
	ini.SetLongValue("Settings", "BoxType", config.boxType);
	ini.SetLongValue("Settings", "LinePosition", config.linePosition);
	ini.SetDoubleValue("Settings", "AimbotFOV", config.aimbotFOV);
	ini.SetDoubleValue("Settings", "AimbotSmooth", config.aimbotSmooth);
	ini.SetDoubleValue("Settings", "AimbotYOffset", config.aimbotYOffset);
	ini.SetLongValue("Settings", "DisplayMode", config.displayMode);
	// Сохраняем настройки триггербота
	ini.SetBoolValue("Triggerbot", "Enabled", config.triggerbotEnabled);
	ini.SetDoubleValue("Triggerbot", "Delay", config.triggerDelay);

	// Сохраняем новые настройки
	ini.SetBoolValue("Settings", "DrawEspFill", config.drawEspFill);
	ini.SetDoubleValue("Settings", "BoxFillAlpha", config.boxFillAlpha);
	// Сохраняем цвета
	ini.SetDoubleValue("FOVColor", "R", config.fovColor[0]);
	ini.SetDoubleValue("FOVColor", "G", config.fovColor[1]);
	ini.SetDoubleValue("FOVColor", "B", config.fovColor[2]);
	ini.SetDoubleValue("FOVColor", "A", config.fovColor[3]);
	ini.SetBoolValue("Settings", "AimbotNoKeyBind", config.aimbotNoKeyBind);
	ini.SetDoubleValue("ESPColor", "R", config.espColor[0]);
	ini.SetDoubleValue("ESPColor", "G", config.espColor[1]);
	ini.SetDoubleValue("ESPColor", "B", config.espColor[2]);
	ini.SetDoubleValue("ESPColor", "A", config.espColor[3]);

	ini.SetDoubleValue("TeammateColor", "R", config.teammateColor[0]);
	ini.SetDoubleValue("TeammateColor", "G", config.teammateColor[1]);
	ini.SetDoubleValue("TeammateColor", "B", config.teammateColor[2]);
	ini.SetDoubleValue("TeammateColor", "A", config.teammateColor[3]);
	// Сохраняем цвета
	ini.SetDoubleValue("HealthColor", "High_R", config.highHealthColor[0]);
	ini.SetDoubleValue("HealthColor", "High_G", config.highHealthColor[1]);
	ini.SetDoubleValue("HealthColor", "High_B", config.highHealthColor[2]);
	ini.SetDoubleValue("HealthColor", "High_A", config.highHealthColor[3]);

	ini.SetDoubleValue("HealthColor", "Low_R", config.lowHealthColor[0]);
	ini.SetDoubleValue("HealthColor", "Low_G", config.lowHealthColor[1]);
	ini.SetDoubleValue("HealthColor", "Low_B", config.lowHealthColor[2]);
	ini.SetDoubleValue("HealthColor", "Low_A", config.lowHealthColor[3]);

	ini.SetDoubleValue("ArmorColor", "High_R", config.highArmorColor[0]);
	ini.SetDoubleValue("ArmorColor", "High_G", config.highArmorColor[1]);
	ini.SetDoubleValue("ArmorColor", "High_B", config.highArmorColor[2]);
	ini.SetDoubleValue("ArmorColor", "High_A", config.highArmorColor[3]);

	ini.SetDoubleValue("ArmorColor", "Low_R", config.lowArmorColor[0]);
	ini.SetDoubleValue("ArmorColor", "Low_G", config.lowArmorColor[1]);
	ini.SetDoubleValue("ArmorColor", "Low_B", config.lowArmorColor[2]);
	ini.SetDoubleValue("ArmorColor", "Low_A", config.lowArmorColor[3]);
	ini.SetBoolValue("Settings", "GradientHealthBar", config.gradientHealthBar);
	ini.SetBoolValue("Settings", "GradientArmorBar", config.gradientArmorBar);
	ini.SetBoolValue("Settings", "DrawOutline", config.drawOutline); // Сохраняем drawOutline
	if (ini.SaveFile(filename.c_str()) == SI_OK) {
		printf("[>>] Config saved successfully!\n");
	}
	else {
		printf("[!!] Failed to save config!\n");
	}
}

// Функция загрузки конфигурации из файла
void LoadConfig(const std::string& filename = "config.ini") {
	CSimpleIniA ini;
	ini.LoadFile(filename.c_str());

	config.esp = ini.GetBoolValue("Settings", "ESP", true);
	config.drawBoxes = ini.GetBoolValue("Settings", "DrawBoxes", false);
	config.drawDistanceText = ini.GetBoolValue("Settings", "DrawDistanceText", false);
	config.drawLines = ini.GetBoolValue("Settings", "DrawLines", false);
	config.drawForTeammates = ini.GetBoolValue("Settings", "DrawForTeammates", false);
	config.drawHealthBar = ini.GetBoolValue("Settings", "DrawHealthBar", false);
	config.drawArmorBar = ini.GetBoolValue("Settings", "DrawArmorBar", false);
	config.aimbotEnabled = ini.GetBoolValue("Settings", "AimbotEnabled", false);
	config.boxType = ini.GetLongValue("Settings", "BoxType", 0);
	config.linePosition = ini.GetLongValue("Settings", "LinePosition", 0);
	config.aimbotFOV = static_cast<float>(ini.GetDoubleValue("Settings", "AimbotFOV", 200.0));
	config.aimbotSmooth = static_cast<float>(ini.GetDoubleValue("Settings", "AimbotSmooth", 0.3));
	config.aimbotYOffset = static_cast<float>(ini.GetDoubleValue("Settings", "AimbotYOffset", 0.0));
	config.displayMode = ini.GetLongValue("Settings", "DisplayMode", 0);
	config.highHealthColor[0] = static_cast<float>(ini.GetDoubleValue("HealthColor", "High_R", 0.0));
	config.highHealthColor[1] = static_cast<float>(ini.GetDoubleValue("HealthColor", "High_G", 1.0));
	config.highHealthColor[2] = static_cast<float>(ini.GetDoubleValue("HealthColor", "High_B", 0.0));
	config.highHealthColor[3] = static_cast<float>(ini.GetDoubleValue("HealthColor", "High_A", 1.0));
	// Загружаем параметры триггербота
	config.triggerbotEnabled = ini.GetBoolValue("Triggerbot", "Enabled", false);
	config.triggerDelay = static_cast<float>(ini.GetDoubleValue("Triggerbot", "Delay", 0.1f));
	config.aimbotNoKeyBind = ini.GetBoolValue("Settings", "AimbotNoKeyBind", false);  // Загружаем флаг
	config.lowHealthColor[0] = static_cast<float>(ini.GetDoubleValue("HealthColor", "Low_R", 1.0));
	config.lowHealthColor[1] = static_cast<float>(ini.GetDoubleValue("HealthColor", "Low_G", 0.0));
	config.lowHealthColor[2] = static_cast<float>(ini.GetDoubleValue("HealthColor", "Low_B", 0.0));
	config.lowHealthColor[3] = static_cast<float>(ini.GetDoubleValue("HealthColor", "Low_A", 1.0));
	config.visibleColor[0] = static_cast<float>(ini.GetDoubleValue("VisibleColor", "R", 0.0f));
	config.visibleColor[1] = static_cast<float>(ini.GetDoubleValue("VisibleColor", "G", 1.0f));
	config.visibleColor[2] = static_cast<float>(ini.GetDoubleValue("VisibleColor", "B", 0.0f));
	config.visibleColor[3] = static_cast<float>(ini.GetDoubleValue("VisibleColor", "A", 1.0f));
	config.useKeyBind = ini.GetBoolValue("Keybinds", "UseKeyBind", false);
	memcpy(visibleColor, config.visibleColor, sizeof(visibleColor)); // Copy the value from config to global visibleColor

	memcpy((float*)&highHealthColor, config.highHealthColor, sizeof(config.highHealthColor));
	memcpy((float*)&lowHealthColor, config.lowHealthColor, sizeof(config.lowHealthColor));
	// Загружаем цвета
	config.fovColor[0] = static_cast<float>(ini.GetDoubleValue("FOVColor", "R", 1.0));
	config.fovColor[1] = static_cast<float>(ini.GetDoubleValue("FOVColor", "G", 0.0));
	config.fovColor[2] = static_cast<float>(ini.GetDoubleValue("FOVColor", "B", 0.0));
	config.fovColor[3] = static_cast<float>(ini.GetDoubleValue("FOVColor", "A", 1.0));

	config.espColor[0] = static_cast<float>(ini.GetDoubleValue("ESPColor", "R", 1.0));
	config.espColor[1] = static_cast<float>(ini.GetDoubleValue("ESPColor", "G", 1.0));
	config.espColor[2] = static_cast<float>(ini.GetDoubleValue("ESPColor", "B", 1.0));
	config.espColor[3] = static_cast<float>(ini.GetDoubleValue("ESPColor", "A", 1.0));

	config.teammateColor[0] = static_cast<float>(ini.GetDoubleValue("TeammateColor", "R", 0.0));
	config.teammateColor[1] = static_cast<float>(ini.GetDoubleValue("TeammateColor", "G", 1.0));
	config.teammateColor[2] = static_cast<float>(ini.GetDoubleValue("TeammateColor", "B", 0.0));
	config.teammateColor[3] = static_cast<float>(ini.GetDoubleValue("TeammateColor", "A", 1.0));
	config.gradientHealthBar = ini.GetBoolValue("Settings", "GradientHealthBar", false);
	config.gradientArmorBar = ini.GetBoolValue("Settings", "GradientArmorBar", false);
	// Загружаем новые параметры
	config.drawEspFill = ini.GetBoolValue("Settings", "DrawEspFill", false);
	config.boxFillAlpha = static_cast<float>(ini.GetDoubleValue("Settings", "BoxFillAlpha", 100.0));
	config.drawOutline = ini.GetBoolValue("Settings", "DrawOutline", false);  // По умолчанию выключено

	// Копируем в глобальные переменные
	esp = config.esp;
	drawBoxes = config.drawBoxes;
	drawDistanceText = config.drawDistanceText;
	drawLines = config.drawLines;
	drawForTeammates = config.drawForTeammates;
	drawHealthBar = config.drawHealthBar;
	drawArmorBar = config.drawArmorBar;
	boxType = config.boxType;
	linePosition = config.linePosition;
	aimbotEnabled = config.aimbotEnabled;
	aimbotFOV = config.aimbotFOV;
	aimbotSmooth = config.aimbotSmooth;
	aimbotYOffset = config.aimbotYOffset;
	displayMode = config.displayMode;
	drawOutline = config.drawOutline;
	drawEspFill = config.drawEspFill;
	boxFillAlpha = config.boxFillAlpha;
	memcpy(customColor, config.espColor, sizeof(customColor));
	memcpy(teammateColor, config.teammateColor, sizeof(teammateColor));
	memcpy(fovColor, config.fovColor, sizeof(fovColor));
	triggerbotEnabled = config.triggerbotEnabled;
	triggerDelay = config.triggerDelay;
	aimbotNoKeyBind = config.aimbotNoKeyBind;
	useKeyBind = config.useKeyBind; // Копируем значение привязки клавиши

	printf("[>>] Config loaded successfully!\n");
}

// Функция для переименования конфигурационного слота
void RenameConfigSlot(int slotIndex) {
	if (strlen(newConfigName) > 0) {
		configSlots[slotIndex] = newConfigName;
		configFiles[slotIndex] = std::string(newConfigName) + ".ini";  // Обновляем имя файла
	}
}

// Интерфейс 



ID3D11Device* Overlay::device = nullptr;

// sends rendering commands to the device
ID3D11DeviceContext* Overlay::device_context = nullptr;

// manages the buffers for rendering, also presents rendered frames.
IDXGISwapChain* Overlay::swap_chain = nullptr;

// represents the target surface for rendering
ID3D11RenderTargetView* Overlay::render_targetview = nullptr;

HWND Overlay::overlay = nullptr;
WNDCLASSEX Overlay::wc = { };

// declaration of the ImGui_ImplWin32_WndProcHandler function
// basically integrates ImGui with the Windows message loop so ImGui can process input and events
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK window_procedure(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// set up ImGui window procedure handler
	if (ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
		return true;

	// switch that disables alt application and checks for if the user tries to close the window.
	switch (msg)
	{
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu (imgui uses it in their example :shrug:)
			return 0;
		break;

	case WM_DESTROY:
		Overlay::DestroyDevice();
		Overlay::DestroyOverlay();
		Overlay::DestroyImGui();
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		Overlay::DestroyDevice();
		Overlay::DestroyOverlay();
		Overlay::DestroyImGui();
		return 0;
	}

	// define the window procedure
	return DefWindowProc(window, msg, wParam, lParam);
}

bool Overlay::CreateDevice()
{
	// First we setup our swap chain, this basically just holds a bunch of descriptors for the swap chain.
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));

	// set number of back buffers (this is double buffering)
	sd.BufferCount = 2;

	// width + height of buffer, (0 is automatic sizing)
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;

	// set the pixel format
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// set the fps of the buffer (60 at the moment)
	sd.BufferDesc.RefreshRate.Numerator = 60; 
	sd.BufferDesc.RefreshRate.Denominator = 1;

	// allow mode switch (changing display modes)
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// set how the bbuffer will be used
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	sd.OutputWindow = overlay;

	// setup the multi-sampling
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// specify what Direct3D feature levels to use
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	
	// create device and swap chain
	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		featureLevelArray,
		2,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&featureLevel,
		&device_context);

	// if the hardware isn't supported create with WARP (basically just a different renderer)
	if (result == DXGI_ERROR_UNSUPPORTED) {
		result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_WARP,
			nullptr,
			0U,
			featureLevelArray,
			2, D3D11_SDK_VERSION,
			&sd,
			&swap_chain,
			&device,
			&featureLevel,
			&device_context);

		printf("[>>] DXGI_ERROR | Created with D3D_DRIVER_TYPE_WARP\n");
	}

	// can't do much more, if the hardware still isn't supported just return false.
	if (result != S_OK) {
		printf("Device Not Okay\n");
		return false;
	}

	// retrieve back_buffer, im defining it here since it isn't being used at any other point in time.
	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

	// if back buffer is obtained then we can create render target view and release the back buffer again
	if (back_buffer) 
	{
		device->CreateRenderTargetView(back_buffer, nullptr, &render_targetview);
		back_buffer->Release();

		printf("[>>] Created Device\n");
		return true;
	}

	// if we reach this point then it failed to create the back buffer
	printf("[>>] Failed to create Device\n");
	return false;
}

void Overlay::DestroyDevice()
{
	// release everything that has to do with the device.
	if (device)
	{
		device->Release();
		device_context->Release();
		swap_chain->Release();
		render_targetview->Release();

		printf("[>>] Released Device\n");
	}
	else
		printf("[>>] Device Not Found when Exiting.\n");
}

void Overlay::CreateOverlay()
{
	// holds descriptors for the window, called a WindowClass
	// set up window class
	wc.cbSize = sizeof(wc);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = window_procedure;
	wc.hInstance = GetModuleHandleA(0);
	wc.lpszClassName = "RC7";

	// register our class
	RegisterClassEx(&wc);

	// create window (the actual one that shows up in your taskbar)
	// WS_EX_TOOLWINDOW hides the new window that shows up in your taskbar and attaches it to any already existing windows instead.
	// (in this case the console)
	overlay = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
		wc.lpszClassName,
		"RC7",
		WS_POPUP,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN), // 1920
		GetSystemMetrics(SM_CYSCREEN), // 1080
		NULL,
		NULL,
		wc.hInstance,
		NULL
	);

	if (overlay == NULL)
		printf("Failed to create Overlay\n");

	// set overlay window attributes to make the overlay transparent
	SetLayeredWindowAttributes(overlay, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	// set up the DWM frame extension for client area
	{
		// first we define our RECT structures that hold our client and window area
		RECT client_area{};
		RECT window_area{};

		// get the client and window area
		GetClientRect(overlay, &client_area);
		GetWindowRect(overlay, &window_area);

		// calculate the difference between the screen and window coordinates
		POINT diff{};
		ClientToScreen(overlay, &diff);

		// calculate the margins for DWM frame extension
		const MARGINS margins{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom
		};

		// then we extend the frame into the client area
		DwmExtendFrameIntoClientArea(overlay, &margins);
	}

	// show + update overlay
	ShowWindow(overlay, SW_SHOW);
	UpdateWindow(overlay);

	printf("[>>] Overlay Created\n");
}

void Overlay::DestroyOverlay()
{
	DestroyWindow(overlay);
	UnregisterClass(wc.lpszClassName, wc.hInstance);
}

bool Overlay::CreateImGui()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// Initalize ImGui for the Win32 library
	if (!ImGui_ImplWin32_Init(overlay)) {
		printf("Failed ImGui_ImplWin32_Init\n");
		return false;
	}
	
	// Initalize ImGui for DirectX 11.
	if (!ImGui_ImplDX11_Init(device, device_context)) {
		printf("Failed ImGui_ImplDX11_Init\n");
		return false;
	}

	printf("[>>] ImGui Initialized\n");
	return true;
}

void Overlay::DestroyImGui()
{
	// Cleanup ImGui by shutting down DirectX11, the Win32 Platform and Destroying the ImGui context.
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Overlay::StartRender()
{

	// handle windows messages
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// begin a new frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	// if the user presses Insert then enable the menu.
	if (GetAsyncKeyState(VK_INSERT) & 1) {
		RenderMenu = !RenderMenu;

		// If we are rendering the menu set the window styles to be able to clicked on.
		if (RenderMenu) {
			SetWindowLong(overlay, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
		}
		else {
			SetWindowLong(overlay, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED);
		}

	}
}

void Overlay::EndRender()
{
	// Render ImGui
	ImGui::Render();

	// Make a color that's clear / transparent
	float color[4]{ 0, 0, 0, 0 };

	// Set the render target and then clear it
	device_context->OMSetRenderTargets(1, &render_targetview, nullptr);
	device_context->ClearRenderTargetView(render_targetview, color);

	// Render ImGui draw data.
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Present rendered frame with V-Sync
	swap_chain->Present(1U, 0U);

	// Present rendered frame without V-Sync
	//swap_chain->Present(0U, 0U);
}

bool setTrue = false;
bool playerList = false;
bool esp = true;

bool drawBoxes = false;
bool drawDistanceText = false;
bool drawLines = false;
// Положение линии: 0 - верх, 1 - центр, 2 - низ
int linePosition = 0;
float visibleColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
float customColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f }; // RGBA
float teammateColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
float fovColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f }; // Красный по умолчанию
bool drawForTeammates = false;
bool drawHealthBar = false;
bool drawArmorBar = false;
int boxType = 0; // 0 - 2D, 1 - Corner, 2 - 3D
bool aimbotEnabled = false;  // Включение/выключение аимбота
float aimbotFOV = 200.0f;    // Радиус FOV (будет регулироваться)
float aimbotSmooth = 0.3f;
int displayMode = 0; // 0 - Бары (по умолчанию), 1 - Текст
int aimbotTarget = 0;
float aimbotYOffset = 0.0f;
// ***Режим выбора цели для аимбота***
// 0 - выбирать цель ближе к прицелу (по FOV)
// 1 - выбирать цель ближе по дистанции
int aimbotTargetingMode = 0;
bool drawEspFill = false;           // Включить/выключить заливку ESP
float boxFillAlpha = 100.0f; // Начальная прозрачность (значение от 0 до 255)
bool drawHealthGradient = false;    // Включить/выключить градиент здоровья
bool drawArmorGradient = false;     // Включить/выключить градиент брони
// Глобальные переменные для цветов
ImVec4 highHealthColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);  // Зелёный
ImVec4 lowHealthColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);   // Красный
ImVec4 highArmorColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);  // Синий (высокая броня)
ImVec4 lowArmorColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);   // Голубой (низкая броня)
bool gradientHealthBar = false;  // Включить/выключить градиент для полосы здоровья
bool gradientArmorBar = false;
bool drawOutline = false;  // Переменная для включения обводки// Глобальные переменные для триггербота
bool triggerbotEnabled = false;  // Включение/выключение триггербота
float triggerDelay = 0.1f;       // Задержка перед выстрелом (секунды)
bool drawNames = true;
bool newAimbotEnabled = false; // Статус нового аимбота
bool AimVisiable = false;  // Изначально пустой
// Определяем два цвета для ESP
bool useKeyBind = false;  // Новый флаг для управления привязкой клавиш



bool aimbotNoKeyBind = false;
void Overlay::Render()

{
	CDispatcher* mem = CDispatcher::Get();
	ImGui::SetNextWindowSize({ 600, 450 }); 
	static int selectedTab = 0; // 0 - ESP, 1 - Aimbot

	ImGui::Begin("LASTNIGHT EXTERNAL", &RenderMenu, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);

	mem->Attach("cs2.exe");
	uintptr_t base = mem->GetModuleBase("client.dll");
	static int selectedSlot = 0;
	CBaseEntity centity;
	CBasePlayer cplayer;

	// Кнопки для переключения вкладок
	if (ImGui::Button("ESP")) selectedTab = 0;
	ImGui::SameLine();
	if (ImGui::Button("Aimbot")) selectedTab = 1;
	ImGui::SameLine();
	if (ImGui::Button("Config")) selectedTab = 2;

	ImGui::Separator(); // Разделитель для визуального разделения вкладок

	if (selectedTab == 0) { // **Вкладка ESP**
		ImGui::Text("ESP Settings");

		ImGui::Checkbox("ESP", &esp);
		ImGui::SameLine();
		ImGui::ColorEdit4("Invisiable Color", customColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::SameLine();
		ImGui::ColorEdit4("Visible Color", visibleColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Checkbox("Box", &drawBoxes);
		if (drawBoxes) {
			const char* boxTypes[] = { "2D", "Corner", "3D" };
			ImGui::Combo("##BoxType", &boxType, boxTypes, IM_ARRAYSIZE(boxTypes));
		}

		ImGui::Checkbox("Distance", &drawDistanceText);

		ImGui::Checkbox("Lines", &drawLines);
		if (drawLines) {
			const char* positions[] = { "Top", "Center", "Bottom" };
			ImGui::Combo("##LinePosition", &linePosition, positions, IM_ARRAYSIZE(positions));
		}

		ImGui::Checkbox("Teammates", &drawForTeammates);
		ImGui::SameLine();
		ImGui::ColorEdit4("Team Color", teammateColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Checkbox("Draw Outline", &drawOutline);

		ImGui::Checkbox("ESP Fill", &drawEspFill);
		if (drawEspFill) {
			ImGui::SliderFloat("Fill Alpha", &boxFillAlpha, 0.0f, 155.0f, "%.0f");
		}

		ImGui::Checkbox("HealthBar", &drawHealthBar);
		ImGui::SameLine();
		ImGui::ColorEdit4("High Health Color", (float*)&highHealthColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::SameLine();
		ImGui::ColorEdit4("Low Health Color", (float*)&lowHealthColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

		// Чекбокс для градиента здоровья появляется только если drawHealthBar включен
		if (drawHealthBar) {
			ImGui::Checkbox("Gradient Health", &gradientHealthBar);
		}

		ImGui::Checkbox("ArmorBar", &drawArmorBar);  
		ImGui::SameLine();
		ImGui::ColorEdit4("High Armor Color", (float*)&highArmorColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::SameLine();
		ImGui::ColorEdit4("Low Armor Color", (float*)&lowArmorColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);


		// Чекбокс для градиента брони появляется только если drawArmorBar включен
		if (drawArmorBar) {
			ImGui::Checkbox("Gradient Armor", &gradientArmorBar); 
		}

		if (drawHealthBar || drawArmorBar) {
			const char* modes[] = { "Bars", "Text", "Combo" };
			ImGui::Combo("##DisplayMode", &displayMode, modes, IM_ARRAYSIZE(modes));
		}
	



	}

	if (selectedTab == 1) { // **Вкладка Aimbot**
		ImGui::Text("Aimbot Settings");
		ImGui::Checkbox("Enable Aimbot", &aimbotEnabled);
		ImGui::SliderFloat("Aimbot FOV", &aimbotFOV, 50.0f, 500.0f, "%.1f");
		ImGui::SliderFloat("Aimbot Smooth", &aimbotSmooth, 0.1f, 1.0f, "%.2f");
		ImGui::ColorEdit4("FOV Color", fovColor);
		ImGui::SliderFloat("Aimbot Y Offset", &aimbotYOffset, -50.0f, 50.0f, "%.1f");
		ImGui::Text("Aimbot Targeting Mode");
		if (ImGui::RadioButton("FOV (Near Crosshair)", aimbotTargetingMode == 0)) aimbotTargetingMode = 0;
		if (ImGui::RadioButton("Distance (Closest Player)", aimbotTargetingMode == 1)) aimbotTargetingMode = 1;
		ImGui::Checkbox("Enable Triggerbot", &triggerbotEnabled);
		ImGui::SliderFloat("Trigger Delay (sec)", &triggerDelay, 0.01f, 0.5f, "%.2f");
		ImGui::Checkbox("No KeyBind", &aimbotNoKeyBind);

		
	}

	if (selectedTab == 2) {  // Вкладка Config
		ImGui::Text("Config Settings");

		// Комбо-бокс для выбора слота
		const char* slotNames[] = { configSlots[0].c_str(), configSlots[1].c_str(), configSlots[2].c_str() };
		ImGui::Combo("Select Slot", &selectedSlot, slotNames, IM_ARRAYSIZE(slotNames));

		// Отображаем кнопки для загрузки и сохранения выбранного слота
		if (ImGui::Button("Load")) {
			LoadConfig(configFiles[selectedSlot]);
		}
		ImGui::SameLine();
		if (ImGui::Button("Save")) {
			SaveConfig(configFiles[selectedSlot]);
		}

		// Если включен режим переименования, показываем поле для ввода нового имени
		if (isRenaming) {
			ImGui::InputText("New Slot Name", newConfigName, sizeof(newConfigName));
			if (ImGui::Button("Confirm Rename")) {
				RenameConfigSlot(selectedSlot);  // Переименовываем выбранный слот
				isRenaming = false;
			}
		}
	}

	ImGui::End();

	if (setTrue) {
		ImGui::SetNextWindowSize({ 500, 250 });
		ImGui::Begin("Entity List", &setTrue);
		uintptr_t entitylist = centity.GetEntityList(base);

		for (int i = 0; i < 128; i++) {

			uintptr_t playerpawn = cplayer.getPCSPlayerPawn(base, i);
			int health = cplayer.getHealth(playerpawn);
			int localTeam = cplayer.GetTeam(playerpawn);
			CVector vecOrig = cplayer.GetOrigin(playerpawn);
			CVector screen;
			if (!health)
				continue;
			if (localTeam == 2) {
				ImGui::Text("[OPFOR]Entity %d Position : %.3f, %.3f, %.3f", localTeam, i, vecOrig.x, vecOrig.y, vecOrig.z);
			}
			if (localTeam == 3) {
				ImGui::Text("[BLUFOR]Entity %d Position : %.3f, %.3f, %.3f", localTeam, i, vecOrig.x, vecOrig.y, vecOrig.z);
			}
			//if (isTeam) {
			//	ImGui::Text("Friendly %d: 0x%llx", i, entity);
			//}

			//ImGui::Text("Entity %d: 0x%llx", i, entity);
		}

		ImGui::End();
	}
}


void Overlay::SetForeground(HWND window)
{
	if (!IsWindowInForeground(window))
		BringToForeground(window);
}