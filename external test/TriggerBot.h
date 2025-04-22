#pragma once
#include "GameEngine.h"
#include "Vector.h"
#include "Memory.h"
#include "imgui/imgui.h"
#include "Offsets.h"
#include <cmath>
#include <windows.h>
#include <iostream>
#include <vector>
#include <chrono>

// External variables from menu
extern bool triggerbotEnabled;   // Включение/выключение тригербота
extern float triggerDelay;       // Задержка между выстрелами
extern float aimbotYOffset;      // Y Offset для аимбота
extern bool useKeyBind;          // Флаг для использования кейбинда (тот же, что и для аимбота)

class TriggerBot {
    CDispatcher* mem = CDispatcher::Get();
    std::chrono::steady_clock::time_point lastShotTime;

public:
    void RunTriggerbot(uintptr_t base, uintptr_t playerpawn, view_matrix_t& vm, std::vector<ImVec2>& espTargets, std::vector<float>& espDistances) {
        if (!triggerbotEnabled) return;  // Если тригербот выключен, ничего не делаем

        // Проверка на использование кейбинда, если useKeyBind включен
        if (useKeyBind) {
            if (!(GetAsyncKeyState(VK_XBUTTON2) & 0x8000)) return; // Проверяем, зажата ли кнопка мыши 5
        }

        // Если цели или дистанции пустые, выходим
        if (espTargets.empty() || espDistances.empty()) return;

        // Проверяем время с последнего выстрела, чтобы не стрельнуть слишком быстро
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastShotTime).count() < triggerDelay * 1000) {
            return;
        }

        // Центр экрана
        ImVec2 screenCenter = { ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2 };

        int bestTarget = -1;
        float bestScore = FLT_MAX;
        ImVec2 bestScreenTarget;

        // Проходим по всем целям
        for (int i = 0; i < espTargets.size(); i++) {
            ImVec2 screenPos = espTargets[i];
            screenPos.y += aimbotYOffset;

            float distance = espDistances[i];
            uintptr_t targetPawn = cplayer.getPCSPlayerPawn(base, i);
            int team = cplayer.GetTeam(targetPawn);

            int localTeam = cplayer.getLocalTeam(base);
            if (localTeam == team) continue;  // Пропускаем тиммейтов

            float fov = GetFOV(screenCenter, screenPos);
            if (fov > 10.0f) continue;  // Пропускаем цели с большим FOV

            // Вычисление приоритета цели
            float deltaX = abs(screenPos.x - screenCenter.x);
            float deltaY = abs(screenPos.y - screenCenter.y);

            float score = (deltaX + deltaY) + (fov * 0.7f) + (distance * 0.5f);
            if (score < bestScore) {
                bestScore = score;
                bestTarget = i;
                bestScreenTarget = screenPos;
            }
        }

        // Если нашли подходящую цель, стреляем
        if (bestTarget != -1) {
            Shoot();
            lastShotTime = std::chrono::steady_clock::now();
        }
    }

private:
    float GetFOV(ImVec2 from, ImVec2 to) {
        float dx = to.x - from.x;
        float dy = to.y - from.y;
        return sqrt(dx * dx + dy * dy);
    }

    void Shoot() {
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        Sleep(10);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    }
};
