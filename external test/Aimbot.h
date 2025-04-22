#pragma once
#include "GameEngine.h"
#include "Vector.h"
#include "Memory.h"
#include "imgui/imgui.h"
#include "w2s.h"
#include <cmath>
#include <windows.h>
#include <iostream>
#include <vector>

// Экстерн переменные из меню
extern bool aimbotEnabled; // Включение/выключение аимбота
extern float aimbotFOV;    // Настройка FOV через слайдер
extern float aimbotSmooth; // Настройка сглаживания через слайдер
extern bool drawDistanceText; // Для отображения дистанции в ESP
extern float aimbotYOffset;  // Для Y Offset
extern bool drawForTeammates;  // Для фильтрации тиммейтов
extern float fovColor[4];  // Цвет FOV
extern float espColor[4];  // Цвет ESP
extern bool esp;  // Включение ESP
extern bool AimVisiable;
CBasePlayer cplayer;  // Определение переменной
extern bool aimbotNoKeyBind; // Новый флаг для отключения клавишного бинда
extern bool useKeyBind; // Новый флаг для использования кейбинда

class Aimbot {
    CDispatcher* mem = CDispatcher::Get();
    float maxAngleDelta = 150.0f; // Максимальный угол поворота (фикс бага)

public:
    void AimAtTarget(uintptr_t base, uintptr_t playerpawn, view_matrix_t& vm, std::vector<ImVec2>& espTargets, std::vector<float>& espDistances) {
        if (!aimbotEnabled) return; // Если аимбот выключен, ничего не делаем

        // Если флаг useKeyBind активен, проверяем только клавишу мыши
        if (useKeyBind) {
            if (!(GetAsyncKeyState(VK_XBUTTON2) & 0x8000)) return; // Проверяем, зажата ли кнопка мыши 5
        }
        else {
            if (!aimbotNoKeyBind && !(GetAsyncKeyState(VK_XBUTTON2) & 0x8000)) return; // Если useKeyBind выключен, проверяем AimVisiable
        }

        if (espTargets.empty() || espDistances.empty()) {
            std::cout << "[AIMBOT] No valid ESP targets!\n";
            return;
        }

        std::cout << "[AIMBOT] Total targets in ESP: " << espTargets.size() << "\n";
        if (espTargets.empty() || espDistances.empty()) {
            espTargets.clear();
            espDistances.clear();
            return;
        }

        uintptr_t aimAngleAddr = base + dwViewAngles;
        uintptr_t punchAngleAddr = playerpawn + m_aimPunchAngle; // Адрес отдачи
        if (!playerpawn) return;

        ImVec2 screenCenter = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);

        int bestTarget;
        float bestScore = FLT_MAX;
        ImVec2 bestScreenTarget;
        float bestDistance = FLT_MAX;

        int localTeam = cplayer.getLocalTeam(base); // Получаем команду локального игрока

        std::cout << "[AIMBOT] Local Team: " << localTeam << "\n";

        // Цикл по всем целям
        for (int i = 0; i < espTargets.size(); i++) {
            ImVec2 screenPos = espTargets[i];
            screenPos.y += aimbotYOffset;

            // Получаем дистанцию до цели
            float distance = espDistances[i];

            // Получаем информацию о цели
            uintptr_t targetPawn = cplayer.getPCSPlayerPawn(base, i);
            int team = cplayer.GetTeam(targetPawn); // Команда цели

            std::cout << "[AIMBOT] Target " << i << " Team: " << team << "\n";

            // Пропускаем тиммейтов (если выбрана фильтрация по команде)
            if (localTeam == team && !drawForTeammates) {
                std::cout << "[AIMBOT] Skipping teammate: " << i << "\n";
                continue; // Это тиммейт, пропускаем
            }

            float fov = GetFOV(screenCenter, screenPos);
            std::cout << "[AIMBOT] Target " << i << " FOV: " << fov << "\n";

            if (fov > aimbotFOV) {
                continue; // Пропускаем цели, если они вне FOV
            }

            // Добавляем приоритет по дистанции (более близкие цели важнее)
            float deltaX = abs(screenPos.x - screenCenter.x);
            float deltaY = abs(screenPos.y - screenCenter.y);

            float score = (deltaX + deltaY) + (fov * 0.7f) + (distance * 0.5f);

            if (score < bestScore) {
                bestScore = score;
                bestTarget = i;
                bestScreenTarget = screenPos;
                bestDistance = distance;
            }
        }

        if (bestTarget != -1) {
            std::cout << "[AIMBOT] Target found: " << bestTarget
                << " | Distance: " << bestDistance / 10.0f << "m\n"; // Логируем дистанцию

            MoveMouseToTarget(base, bestScreenTarget, punchAngleAddr);
        }
        else {
            std::cout << "[AIMBOT] No valid ESP target found!\n";
        }
    }

    void DrawFOVCircle() {
        if (!aimbotEnabled) return; // Не рисовать, если аимбот выключен

        ImVec2 screenCenter = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);

        // Преобразуем цвет из RGBA (0-1) в формат ImGui (0-255)
        ImU32 circleColor = IM_COL32(
            static_cast<int>(fovColor[0] * 255),
            static_cast<int>(fovColor[1] * 255),
            static_cast<int>(fovColor[2] * 255),
            static_cast<int>(fovColor[3] * 255)
        );

        // Рисуем круг с заданным цветом
        ImGui::GetForegroundDrawList()->AddCircle(screenCenter, aimbotFOV, circleColor, 64, 1.5f);
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

        // **Читаем отдачу**
        CVector punchAngle = mem->ReadMemory<CVector>(punchAngleAddr);

        // **Настраиваем коэффициенты компенсации**
        float recoilScale = 2.0f;  // Общий множитель компенсации
        float recoilFactor = 1.0f; // Для динамической подстройки

        float distanceToCenter = sqrt(deltaX * deltaX + deltaY * deltaY);
        if (distanceToCenter > 100.0f) recoilFactor = 1.5f; // Увеличиваем, если цель далеко
        if (distanceToCenter > 200.0f) recoilFactor = 2.0f; // Еще больше, если очень далеко

        // **Корректируем горизонтальную и вертикальную отдачу**
        float recoilCompX = punchAngle.y * recoilScale * recoilFactor; // Горизонтальная компенсация
        float recoilCompY = punchAngle.x * recoilScale * recoilFactor; // Вертикальная компенсация

        deltaX -= recoilCompX;  // **Компенсация смещения вправо**
        deltaY -= recoilCompY;  // **Компенсация смещения вверх**

        // **Ограничение угла поворота**
        float angleDiff = sqrt(deltaX * deltaX + deltaY * deltaY);
        if (angleDiff > maxAngleDelta) {
            std::cout << "[AIMBOT] Angle too large, skipping.\n";
            return;
        }

        // **Сглаживаем движение мыши**
        deltaX *= aimbotSmooth;
        deltaY *= aimbotSmooth;

        // **Двигаем мышь**
        mouse_event(MOUSEEVENTF_MOVE, static_cast<int>(deltaX), static_cast<int>(deltaY), 0, 0);

        std::cout << "[AIMBOT] Moving mouse to (" << target.x << ", " << target.y << ") | ΔX=" << deltaX << ", ΔY=" << deltaY << " | Recoil X=" << recoilCompX << " Y=" << recoilCompY << "\n";
    }
};
