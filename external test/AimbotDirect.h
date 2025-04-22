#pragma once
#include "GameEngine.h"
#include "Vector.h"
#include "Memory.h"
#include "imgui/imgui.h"
#include "w2s.h"  // Подключаем w2s.h для преобразования мировых координат в экранные
#include <cmath>
#include <windows.h>
#include <iostream>
#include <vector>

extern bool aimbotEnabled; // Включение/выключение аимбота
extern float aimbotFOV;    // Настройка FOV через слайдер
extern float aimbotSmooth; // Настройка сглаживания через слайдер
extern bool drawForTeammates;  // Для фильтрации тиммейтов

#define MAX_PLAYERS 64  // Максимальное количество игроков

class AimbotDirect {
    CDispatcher* mem = CDispatcher::Get();
    CBasePlayer cplayer;  // Объект для работы с игроками
    float maxAngleDelta = 150.0f; // Максимальный угол поворота

public:
    void AimAtTargetNew(uintptr_t base, uintptr_t playerpawn, view_matrix_t& vm) {
        if (!aimbotEnabled) {
            std::cout << "[DEBUG] Aimbot is disabled\n";
            return;
        }

        if (!GetAsyncKeyState(VK_XBUTTON2)) {
            return;  // Если кнопка не нажата, выходим
        }
        std::cout << "[DEBUG] Key pressed, continuing AimAtTargetNew...\n";  // Логируем, что кнопка нажата

        uintptr_t aimAngleAddr = base + dwViewAngles;
        uintptr_t punchAngleAddr = playerpawn + m_aimPunchAngle; // Адрес отдачи
        if (!playerpawn) return;

        int localTeam = cplayer.getLocalTeam(base); // Получаем команду локального игрока
        std::cout << "[AIMBOT] Local Team: " << localTeam << "\n";

        bool foundTarget = false;

        // Цикл по всем сущностям
        for (int i = 0; i < 64; i++) {
            uintptr_t targetEntity = cplayer.GetEntity(base, i);
            if (!targetEntity) continue;

            int team = cplayer.GetTeam(targetEntity);  // Получаем команду цели

            // Проверка на тиммейтов
            std::cout << "[DEBUG] Target " << i << " team: " << team << " (local team: " << localTeam << ")\n";
            if (localTeam == team && !drawForTeammates) continue;  // Пропускаем тиммейтов, если флаг не выставлен

            uintptr_t targetPawn = cplayer.GetPlayerPawn(base, i);
            if (!targetPawn) continue;

            CVector targetPosition = cplayer.GetOrigin(targetEntity);  // Получаем позицию цели
            CVector screenPos;

            if (!w2s(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), targetPosition, screenPos, vm)) {
                std::cout << "[DEBUG] w2s failed for target " << i << ", skipping...\n";
                continue;  // Пропускаем, если не удалось преобразовать в экранные координаты
            }

            // Вычисление FOV
            ImVec2 screenCenter = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y);
            float fov = GetFOV(screenCenter, ImVec2(screenPos.x, screenPos.y));
            std::cout << "[DEBUG] Target " << i << " FOV: " << fov << "\n";  // Логируем FOV
            if (fov > aimbotFOV) continue;  // Пропускаем, если цель за пределами FOV

            // Логируем, что нашли цель
            foundTarget = true;
            std::cout << "[AIMBOT] Target found: " << i << " | Team: " << team << " | FOV: " << fov << "\n";

            // Находим угол между целью и текущим положением мыши
            float deltaX = abs(screenPos.x - screenCenter.x);
            float deltaY = abs(screenPos.y - screenCenter.y);

            float score = (deltaX + deltaY) + (fov * 0.7f);  // Простой расчет приоритета цели

            // Если цель подходит, двигаем мышь на нее
            MoveMouseToTarget(base, ImVec2(screenPos.x, screenPos.y), punchAngleAddr);
        }

        if (!foundTarget) {
            std::cout << "[DEBUG] No targets found\n";  // Логируем, если не нашли цели
        }

        // Отрисовка FOV
        DrawFOV();
    }

private:
    float GetFOV(ImVec2 from, ImVec2 to) {
        float dx = to.x - from.x;
        float dy = to.y - from.y;
        return sqrt(dx * dx + dy * dy);
    }

    void MoveMouseToTarget(uintptr_t base, ImVec2 target, uintptr_t punchAngleAddr) {
        ImVec2 screenCenter = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);

        float deltaX = target.x - screenCenter.x;
        float deltaY = target.y - screenCenter.y;

        // Читаем отдачу
        CVector punchAngle = mem->ReadMemory<CVector>(punchAngleAddr);

        // Компенсация отдачи
        float recoilScale = 2.0f;
        float recoilFactor = 1.0f;
        float distanceToCenter = sqrt(deltaX * deltaX + deltaY * deltaY);
        if (distanceToCenter > 100.0f) recoilFactor = 1.5f;
        if (distanceToCenter > 200.0f) recoilFactor = 2.0f;

        float recoilCompX = punchAngle.y * recoilScale * recoilFactor;
        float recoilCompY = punchAngle.x * recoilScale * recoilFactor;

        deltaX -= recoilCompX;
        deltaY -= recoilCompY;

        // Ограничение угла поворота
        float angleDiff = sqrt(deltaX * deltaX + deltaY * deltaY);
        if (angleDiff > maxAngleDelta) return;

        // Сглаживание движения мыши
        deltaX *= aimbotSmooth;
        deltaY *= aimbotSmooth;

        // Двигаем мышь
        mouse_event(MOUSEEVENTF_MOVE, static_cast<int>(deltaX), static_cast<int>(deltaY), 0, 0);
        std::cout << "[AIMBOT] Moving mouse to (" << target.x << ", " << target.y << ") | ΔX=" << deltaX << ", ΔY=" << deltaY << " | Recoil X=" << recoilCompX << " Y=" << recoilCompY << "\n";
    }

    void DrawFOV() {
        // Отрисовываем круг для FOV
        ImVec2 screenCenter = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddCircle(screenCenter, aimbotFOV, IM_COL32(255, 0, 0, 255), 60, 2.0f); // Красный круг
    }
};
