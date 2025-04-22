#include "GameEngine.h"
#include "window.hpp"
#include "Includes.h"
#include "Memory.h"
#include "Offsets.h"
#include "Vector.h"
#include <thread>
#include "w2s.h"
#include "Watermark.h"
#include "Aimbot.h"
#include <algorithm> // Нужен для std::max() и std::min()
#include <cmath>
#include <unordered_map>
#include "TriggerBot.h"
#include "AimbotDirect.h"  
void DrawOutline(ImVec2 topLeft, ImVec2 bottomRight, ImU32 outlineColor) {
    if (drawOutline) {
        // Тонкая черная обводка
        float outlineThickness = 3.5f;
        ImGui::GetForegroundDrawList()->AddRect(
            topLeft, bottomRight, outlineColor, 0.0f, 0, outlineThickness);
    }
}
// Функция для отрисовки линии с использованием ImGui
void DrawLine(float x1, float y1, float x2, float y2, float color[4], float thickness) {
    ImU32 col = IM_COL32(
        static_cast<int>(color[0] * 255),  // Red
        static_cast<int>(color[1] * 255),  // Green
        static_cast<int>(color[2] * 255),  // Blue
        static_cast<int>(color[3] * 255)   // Alpha
    );

    // Добавляем линию на экран
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), col, thickness);
}



int main()
{
    // thread your cheat here 
    ImDrawList* draw;
    CDispatcher* mem = CDispatcher::Get();
    mem->Attach("cs2.exe");
    uintptr_t base = mem->GetModuleBase("client.dll");
    AimbotDirect newAimbot;
    CBaseEntity centity;
    CBasePlayer cplayer;
    Aimbot aimbot;
    std::vector<ImVec2> espTargets;
    std::vector<float> espDistances;

    overlay.shouldRun = true;
    overlay.RenderMenu = false;

    overlay.CreateOverlay();
    overlay.CreateDevice();
    overlay.CreateImGui();

    printf("[>>] Hit insert to show the menu in this overlay!\n");

    overlay.SetForeground(GetConsoleWindow());

    while (overlay.shouldRun)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        overlay.StartRender();

        if (overlay.RenderMenu) {
            overlay.Render();
        }
        else {
            DrawWatermark();
        }
        espTargets.clear();
        espDistances.clear();
   
        if (esp) {
            for (int i = 0; i < 64; i++) {
                uintptr_t entitylist = centity.GetEntityList(base);
                view_matrix_t vm = mem->ReadMemory<view_matrix_t>(base + dwViewMatrix);
                uintptr_t playerpawn = cplayer.getPCSPlayerPawn(base, i);
                int health = cplayer.getHealth(playerpawn);
                int maxHealth = mem->ReadMemory<int>(playerpawn + m_iMaxHealth); // Читаем максимальное здоровье
                int team = cplayer.GetTeam(playerpawn);
                int localTeam = cplayer.getLocalTeam(base);
                CVector vecOrig = cplayer.GetOrigin(playerpawn);
                CVector localOrig = cplayer.GetLocalOrigin(base);
              
                static std::unordered_map<uintptr_t, float> animatedHealthMap;
                static std::unordered_map<uintptr_t, float> oldHealthMap;
                static std::unordered_map<uintptr_t, float> damageFadeMap;
                static std::unordered_map<uintptr_t, float> animatedArmorMap;
                static std::unordered_map<uintptr_t, float> oldArmorMap;
                static std::unordered_map<uintptr_t, float> damageFadeArmorMap;
                if (!health || health <= 0)

                    continue;

                CVector screenTop, screenBottom;
                ImVec2 screensize = ImGui::GetIO().DisplaySize;

                // Вычисляем дистанцию до цели (используем реальные координаты)
                float dx = vecOrig.x - localOrig.x;
                float dy = vecOrig.y - localOrig.y;
                float dz = vecOrig.z - localOrig.z;
                float distance = sqrt(dx * dx + dy * dy + dz * dz);

                // Добавляем расстояние в вектор дистанций
                espDistances.push_back(distance);


                if (!w2s(screensize, vecOrig, screenBottom, vm))
                    continue;

                vecOrig.z += 75.0f; // Высота бокса
                if (!w2s(screensize, vecOrig, screenTop, vm))
                    continue;

                float boxHeight = abs(screenTop.y - screenBottom.y); // Высота бокса на экране
                float boxWidth = boxHeight / 2.0f;                   // Ширина бокса пропорциональна высоте

                float boxLeft = screenBottom.x - boxWidth / 2.0f;
                float boxRight = screenBottom.x + boxWidth / 2.0f;
                float boxTop = screenTop.y;
                float boxBottom = screenBottom.y;

            
              
                bool isVisible = cplayer.isSpotted(playerpawn);

                // Если isVisible истинно, меняем AimVisiable на true, если false, то на false
                AimVisiable = isVisible ? true : false;

              
                // Для врагов и тиммейтов
                if (localTeam != team || drawForTeammates) {
                    ImU32 boxColor = (localTeam != team) ? IM_COL32(
                        static_cast<int>(customColor[0] * 255),
                        static_cast<int>(customColor[1] * 255),
                        static_cast<int>(customColor[2] * 255),
                        static_cast<int>(customColor[3] * 255)
                    ) : IM_COL32(
                        static_cast<int>(teammateColor[0] * 255),
                        static_cast<int>(teammateColor[1] * 255),
                        static_cast<int>(teammateColor[2] * 255),
                        static_cast<int>(teammateColor[3] * 255)
                    );

                    boxColor = (isVisible) ? IM_COL32(
                        static_cast<int>(visibleColor[0] * 255),
                        static_cast<int>(visibleColor[1] * 255),
                        static_cast<int>(visibleColor[2] * 255),
                        static_cast<int>(visibleColor[3] * 255)
                    ) : boxColor;
              


                    if (drawBoxes) {
                        if (boxType == 0) { // 2D
                            // Получаем тот же цвет, что и для бокса, но добавляем регулировку альфа-канала через слайдер
                            ImU32 fillColor = IM_COL32(
                                (boxColor >> IM_COL32_R_SHIFT) & 0xFF,   // Красный
                                (boxColor >> IM_COL32_G_SHIFT) & 0xFF,   // Зеленый
                                (boxColor >> IM_COL32_B_SHIFT) & 0xFF,   // Синий
                                static_cast<int>(boxFillAlpha)           // Прозрачность, регулируемая слайдером
                            );
                            // Добавляем черную обводку для ESP
                            ImU32 outlineColor = IM_COL32(0, 0, 0, 255); // Черная обводка
                            DrawOutline(ImVec2(boxLeft, boxTop), ImVec2(boxRight, boxBottom), outlineColor);
                            // Добавление заливки с прозрачностью
                            if (drawEspFill) {
                                ImGui::GetForegroundDrawList()->AddRectFilled(
                                    ImVec2(boxLeft, boxTop),
                                    ImVec2(boxRight, boxBottom),
                                    fillColor
                                );
                            }

                            // Отрисовка контура бокса
                            ImGui::GetForegroundDrawList()->AddRect(
                                ImVec2(boxLeft, boxTop),
                                ImVec2(boxRight, boxBottom),
                                boxColor, 0.0f, 0, 1.5f
                            );

                            // Добавление точки цели
                            ImVec2 targetPos(screenBottom.x, (screenTop.y + screenBottom.y) / 2);
                            espTargets.push_back(targetPos);
                            espDistances.push_back(distance);
                        }


                        else if (boxType == 1) { // Corner
                            float cornerSize = boxHeight / 5.0f;  // Размер уголков
                            float squareSize = cornerSize * 2.0f; // Размер большого квадрата в углу

                            // Получаем тот же цвет, что и для бокса, но с добавлением регулировки альфа-канала
                            ImU32 fillColor = IM_COL32(
                                (boxColor >> IM_COL32_R_SHIFT) & 0xFF,   // Красный
                                (boxColor >> IM_COL32_G_SHIFT) & 0xFF,   // Зеленый
                                (boxColor >> IM_COL32_B_SHIFT) & 0xFF,   // Синий
                                static_cast<int>(boxFillAlpha)           // Прозрачность
                            );

                            // Заливка уголков (большие квадраты внутри углов)
                            if (drawEspFill) {
                                ImGui::GetForegroundDrawList()->AddRectFilled(
                                    ImVec2(boxLeft, boxTop),
                                    ImVec2(boxRight, boxBottom),
                                    fillColor
                                );
                            }

                            // Рисуем линии для уголков (внутри бокса)
                            ImGui::GetForegroundDrawList()->AddLine(
                                ImVec2(boxLeft, boxTop), ImVec2(boxLeft + cornerSize, boxTop), boxColor, 1.5f); // Верхний левый угол
                            ImGui::GetForegroundDrawList()->AddLine(
                                ImVec2(boxLeft, boxTop), ImVec2(boxLeft, boxTop + cornerSize), boxColor, 1.5f); // Верхний левый угол

                            ImGui::GetForegroundDrawList()->AddLine(
                                ImVec2(boxRight, boxTop), ImVec2(boxRight - cornerSize, boxTop), boxColor, 1.5f); // Верхний правый угол
                            ImGui::GetForegroundDrawList()->AddLine(
                                ImVec2(boxRight, boxTop), ImVec2(boxRight, boxTop + cornerSize), boxColor, 1.5f); // Верхний правый угол

                            ImGui::GetForegroundDrawList()->AddLine(
                                ImVec2(boxLeft, boxBottom), ImVec2(boxLeft + cornerSize, boxBottom), boxColor, 1.5f); // Нижний левый угол
                            ImGui::GetForegroundDrawList()->AddLine(
                                ImVec2(boxLeft, boxBottom), ImVec2(boxLeft, boxBottom - cornerSize), boxColor, 1.5f); // Нижний левый угол

                            ImGui::GetForegroundDrawList()->AddLine(
                                ImVec2(boxRight, boxBottom), ImVec2(boxRight - cornerSize, boxBottom), boxColor, 1.5f); // Нижний правый угол
                            ImGui::GetForegroundDrawList()->AddLine(
                                ImVec2(boxRight, boxBottom), ImVec2(boxRight, boxBottom - cornerSize), boxColor, 1.5f); // Нижний правый угол

                            if (drawOutline) {
                                ImU32 outlineColor = IM_COL32(0, 0, 0, 255); // Черная обводка

                                // Рисуем обводку на внешней стороне углов с более тонкими линиями
                                ImGui::GetForegroundDrawList()->AddLine(
                                    ImVec2(boxLeft - 1.0f, boxTop - 1.0f), ImVec2(boxLeft + cornerSize + 1.0f, boxTop - 1.0f), outlineColor, 1.0f); // Верхний левый угол
                                ImGui::GetForegroundDrawList()->AddLine(
                                    ImVec2(boxLeft - 1.0f, boxTop - 1.0f), ImVec2(boxLeft - 1.0f, boxTop + cornerSize + 1.0f), outlineColor, 1.0f); // Верхний левый угол

                                ImGui::GetForegroundDrawList()->AddLine(
                                    ImVec2(boxRight + 1.0f, boxTop - 1.0f), ImVec2(boxRight - cornerSize - 1.0f, boxTop - 1.0f), outlineColor, 1.0f); // Верхний правый угол
                                ImGui::GetForegroundDrawList()->AddLine(
                                    ImVec2(boxRight + 1.0f, boxTop - 1.0f), ImVec2(boxRight + 1.0f, boxTop + cornerSize + 1.0f), outlineColor, 1.0f); // Верхний правый угол

                                ImGui::GetForegroundDrawList()->AddLine(
                                    ImVec2(boxLeft - 1.0f, boxBottom + 1.0f), ImVec2(boxLeft + cornerSize + 1.0f, boxBottom + 1.0f), outlineColor, 1.0f); // Нижний левый угол
                                ImGui::GetForegroundDrawList()->AddLine(
                                    ImVec2(boxLeft - 1.0f, boxBottom + 1.0f), ImVec2(boxLeft - 1.0f, boxBottom - cornerSize - 1.0f), outlineColor, 1.0f); // Нижний левый угол

                                ImGui::GetForegroundDrawList()->AddLine(
                                    ImVec2(boxRight + 1.0f, boxBottom + 1.0f), ImVec2(boxRight - cornerSize - 1.0f, boxBottom + 1.0f), outlineColor, 1.0f); // Нижний правый угол
                                ImGui::GetForegroundDrawList()->AddLine(
                                    ImVec2(boxRight + 1.0f, boxBottom + 1.0f), ImVec2(boxRight + 1.0f, boxBottom - cornerSize - 1.0f), outlineColor, 1.0f); // Нижний правый угол
                            }


                            // Добавление точки цели
                            ImVec2 targetPos(screenBottom.x, (screenTop.y + screenBottom.y) / 2);
                            espTargets.push_back(targetPos);
                            espDistances.push_back(distance);
                        }


                        if (boxType == 2) { // 3D
                            // Настраиваем размеры бокса
                            float boxWidth3D = 40.0f;
                            float boxHeight3D = 75.0f;
                            float boxDepth3D = 40.0f;

                            // Дополнительное смещение вниз
                            float adjustmentDown = 35.0f;

                            // Корректируем нижнюю точку модели (ноги)
                            CVector feetOrigin = vecOrig;
                            feetOrigin.z -= (boxHeight3D / 2.0f) + adjustmentDown;

                            // Задаём координаты 8 углов куба относительно ног игрока
                            CVector topLeftFront = feetOrigin, topRightFront = feetOrigin, bottomLeftFront = feetOrigin, bottomRightFront = feetOrigin;
                            CVector topLeftBack = feetOrigin, topRightBack = feetOrigin, bottomLeftBack = feetOrigin, bottomRightBack = feetOrigin;

                            // Передняя часть
                            topLeftFront.x -= boxWidth3D / 2;
                            topLeftFront.y += boxDepth3D / 2;
                            topLeftFront.z += boxHeight3D;

                            topRightFront.x += boxWidth3D / 2;
                            topRightFront.y += boxDepth3D / 2;
                            topRightFront.z += boxHeight3D;

                            bottomLeftFront.x -= boxWidth3D / 2;
                            bottomLeftFront.y += boxDepth3D / 2;

                            bottomRightFront.x += boxWidth3D / 2;
                            bottomRightFront.y += boxDepth3D / 2;

                            // Задняя часть
                            topLeftBack.x -= boxWidth3D / 2;
                            topLeftBack.y -= boxDepth3D / 2;
                            topLeftBack.z += boxHeight3D;

                            topRightBack.x += boxWidth3D / 2;
                            topRightBack.y -= boxDepth3D / 2;
                            topRightBack.z += boxHeight3D;

                            bottomLeftBack.x -= boxWidth3D / 2;
                            bottomLeftBack.y -= boxDepth3D / 2;

                            bottomRightBack.x += boxWidth3D / 2;
                            bottomRightBack.y -= boxDepth3D / 2;

                            // Проецируем все углы в экранные координаты
                            CVector screenTopLeftFront, screenTopRightFront, screenBottomLeftFront, screenBottomRightFront;
                            CVector screenTopLeftBack, screenTopRightBack, screenBottomLeftBack, screenBottomRightBack;

                            if (w2s(screensize, topLeftFront, screenTopLeftFront, vm) &&
                                w2s(screensize, topRightFront, screenTopRightFront, vm) &&
                                w2s(screensize, bottomLeftFront, screenBottomLeftFront, vm) &&
                                w2s(screensize, bottomRightFront, screenBottomRightFront, vm) &&
                                w2s(screensize, topLeftBack, screenTopLeftBack, vm) &&
                                w2s(screensize, topRightBack, screenTopRightBack, vm) &&
                                w2s(screensize, bottomLeftBack, screenBottomLeftBack, vm) &&
                                w2s(screensize, bottomRightBack, screenBottomRightBack, vm)) {

                                if (drawOutline) {
                                    ImU32 outlineColor = IM_COL32(0, 0, 0, 255); // Черная обводка
                                    float outlineThickness = 3.0f;

                                    // Обводка передней части
                                    ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenTopLeftFront.x, screenTopLeftFront.y), ImVec2(screenTopRightFront.x, screenTopRightFront.y), outlineColor, outlineThickness);
                                    ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenTopRightFront.x, screenTopRightFront.y), ImVec2(screenBottomRightFront.x, screenBottomRightFront.y), outlineColor, outlineThickness);
                                    ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenBottomRightFront.x, screenBottomRightFront.y), ImVec2(screenBottomLeftFront.x, screenBottomLeftFront.y), outlineColor, outlineThickness);
                                    ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenBottomLeftFront.x, screenBottomLeftFront.y), ImVec2(screenTopLeftFront.x, screenTopLeftFront.y), outlineColor, outlineThickness);

                                    // Обводка задней части
                                    ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenTopLeftBack.x, screenTopLeftBack.y), ImVec2(screenTopRightBack.x, screenTopRightBack.y), outlineColor, outlineThickness);
                                    ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenTopRightBack.x, screenTopRightBack.y), ImVec2(screenBottomRightBack.x, screenBottomRightBack.y), outlineColor, outlineThickness);
                                    ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenBottomRightBack.x, screenBottomRightBack.y), ImVec2(screenBottomLeftBack.x, screenBottomLeftBack.y), outlineColor, outlineThickness);
                                    ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenBottomLeftBack.x, screenBottomLeftBack.y), ImVec2(screenTopLeftBack.x, screenTopLeftBack.y), outlineColor, outlineThickness);

                                    // Обводка соединяющих линий
                                    ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenTopLeftFront.x, screenTopLeftFront.y), ImVec2(screenTopLeftBack.x, screenTopLeftBack.y), outlineColor, outlineThickness);
                                    ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenTopRightFront.x, screenTopRightFront.y), ImVec2(screenTopRightBack.x, screenTopRightBack.y), outlineColor, outlineThickness);
                                    ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenBottomLeftFront.x, screenBottomLeftFront.y), ImVec2(screenBottomLeftBack.x, screenBottomLeftBack.y), outlineColor, outlineThickness);
                                    ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenBottomRightFront.x, screenBottomRightFront.y), ImVec2(screenBottomRightBack.x, screenBottomRightBack.y), outlineColor, outlineThickness);
                                }

                                // Основные линии
                                ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenTopLeftFront.x, screenTopLeftFront.y), ImVec2(screenTopRightFront.x, screenTopRightFront.y), boxColor, 1.5f);
                                ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenTopRightFront.x, screenTopRightFront.y), ImVec2(screenBottomRightFront.x, screenBottomRightFront.y), boxColor, 1.5f);
                                ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenBottomRightFront.x, screenBottomRightFront.y), ImVec2(screenBottomLeftFront.x, screenBottomLeftFront.y), boxColor, 1.5f);
                                ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenBottomLeftFront.x, screenBottomLeftFront.y), ImVec2(screenTopLeftFront.x, screenTopLeftFront.y), boxColor, 1.5f);

                                ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenTopLeftBack.x, screenTopLeftBack.y), ImVec2(screenTopRightBack.x, screenTopRightBack.y), boxColor, 1.5f);
                                ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenTopRightBack.x, screenTopRightBack.y), ImVec2(screenBottomRightBack.x, screenBottomRightBack.y), boxColor, 1.5f);
                                ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenBottomRightBack.x, screenBottomRightBack.y), ImVec2(screenBottomLeftBack.x, screenBottomLeftBack.y), boxColor, 1.5f);
                                ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenBottomLeftBack.x, screenBottomLeftBack.y), ImVec2(screenTopLeftBack.x, screenTopLeftBack.y), boxColor, 1.5f);

                                ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenTopLeftFront.x, screenTopLeftFront.y), ImVec2(screenTopLeftBack.x, screenTopLeftBack.y), boxColor, 1.5f);
                                ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenTopRightFront.x, screenTopRightFront.y), ImVec2(screenTopRightBack.x, screenTopRightBack.y), boxColor, 1.5f);
                                ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenBottomLeftFront.x, screenBottomLeftFront.y), ImVec2(screenBottomLeftBack.x, screenBottomLeftBack.y), boxColor, 1.5f);
                                ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenBottomRightFront.x, screenBottomRightFront.y), ImVec2(screenBottomRightBack.x, screenBottomRightBack.y), boxColor, 1.5f);
                            }
                        }



                    }
                    // Отрисовка других элементов
                    if (drawDistanceText) {
                        char textBuffer[64];
                        sprintf_s(textBuffer, "%.1fM", distance / 10.0f);
                        ImGui::GetForegroundDrawList()->AddText(ImVec2(screenTop.x, screenTop.y - 15.0f), IM_COL32(255, 255, 255, 255), textBuffer);
                    }
                    if (drawHealthBar) {
                        if (displayMode == 0 || displayMode == 2) {
                            // Берём старые значения или устанавливаем новые
                            float& animatedHealth = animatedHealthMap[playerpawn];
                            float& oldHealth = oldHealthMap[playerpawn];
                            float& damageFade = damageFadeMap[playerpawn];

                            if (animatedHealth == 0.0f) animatedHealth = health; // Инициализируем, если нет
                            if (oldHealth == 0.0f) oldHealth = health;
                            if (damageFade == 0.0f) damageFade = 1.0f;

                            float healthBarHeight = boxHeight * (animatedHealth / maxHealth);
                            float healthBarTop = boxTop + (boxHeight - healthBarHeight);
                            float healthPercent = animatedHealth / maxHealth;

                            // Если хп снизилось - запоминаем старое значение
                            if (health < oldHealth) {
                                damageFade = 1.0f; // Сброс прозрачности урона
                                oldHealth = health;
                            }

                            // Плавная анимация изменения хп
                            animatedHealth += (health - animatedHealth) * 0.1f;

                            // Рисуем фон для полосы здоровья
                            ImGui::GetForegroundDrawList()->AddRectFilled(
                                ImVec2(boxLeft - 8.0f, boxTop),
                                ImVec2(boxLeft - 3.0f, boxBottom),
                                IM_COL32(0, 0, 0, 255) // Чёрный фон
                            );

                            // Если включён градиент, рисуем вертикальный градиент (снизу вверх)
                            if (gradientHealthBar) {
                                for (float y = boxBottom; y > healthBarTop; --y) {
                                    float blend = (y - healthBarTop) / healthBarHeight; // Пропорциональное значение для плавного перехода

                                    // Интерполяция цвета от верхнего (низкое здоровье) к нижнему (высокое здоровье)
                                    ImVec4 color = ImVec4(
                                        highHealthColor.x * (1.0f - blend) + lowHealthColor.x * blend,
                                        highHealthColor.y * (1.0f - blend) + lowHealthColor.y * blend,
                                        highHealthColor.z * (1.0f - blend) + lowHealthColor.z * blend,
                                        1.0f // Полная непрозрачность
                                    );

                                    // Рисуем маленький горизонтальный отрезок на каждой строке (пикселе)
                                    ImGui::GetForegroundDrawList()->AddLine(
                                        ImVec2(boxLeft - 7.0f, y),
                                        ImVec2(boxLeft - 4.0f, y),
                                        IM_COL32(
                                            static_cast<int>(color.x * 255),
                                            static_cast<int>(color.y * 255),
                                            static_cast<int>(color.z * 255),
                                            255
                                        ),
                                        1.0f // Ширина линии 1 пиксель
                                    );
                                }
                            }
                            else {
                                // Без градиента: обычная полоска здоровья
                                ImVec4 color = ImVec4(
                                    lowHealthColor.x * (1.0f - healthPercent) + highHealthColor.x * healthPercent,
                                    lowHealthColor.y * (1.0f - healthPercent) + highHealthColor.y * healthPercent,
                                    lowHealthColor.z * (1.0f - healthPercent) + highHealthColor.z * healthPercent,
                                    1.0f
                                );

                                ImGui::GetForegroundDrawList()->AddRectFilled(
                                    ImVec2(boxLeft - 7.0f, healthBarTop),
                                    ImVec2(boxLeft - 4.0f, boxBottom),
                                    IM_COL32(
                                        static_cast<int>(color.x * 255),
                                        static_cast<int>(color.y * 255),
                                        static_cast<int>(color.z * 255),
                                        255
                                    )
                                );
                            }

                            // **ЭФФЕКТ "ПАДАЮЩЕГО" УРОНА**
                            if (animatedHealth < oldHealth) {
                                float oldHealthBarHeight = boxHeight * (oldHealth / maxHealth);
                                float oldHealthBarTop = boxTop + (boxHeight - oldHealthBarHeight);

                                ImGui::GetForegroundDrawList()->AddRectFilled(
                                    ImVec2(boxLeft - 7.0f, oldHealthBarTop),
                                    ImVec2(boxLeft - 4.0f, healthBarTop),
                                    IM_COL32(255, 0, 0, static_cast<int>(damageFade * 255)) // Полоска урона (красная, исчезает)
                                );

                                damageFade = fmax(damageFade - 0.05f, 0.0f); // Плавное исчезновение урона
                            }

                            // Рисуем цифры (в комбинированном режиме)
                            if (displayMode == 2) {
                                char healthText[16];
                                sprintf_s(healthText, "%d", health);
                                ImGui::GetForegroundDrawList()->AddText(
                                    ImVec2(boxLeft - 20.0f, healthBarTop - 10.0f),
                                    IM_COL32(255, 255, 255, 255),
                                    healthText
                                );
                            }
                        }
                    }

                    if (drawArmorBar) {
                        if (displayMode == 0 || displayMode == 2) {
                            int armor = mem->ReadMemory<int>(playerpawn + m_ArmorValue);
                            if (armor > 0) {
                                // Берём старые значения или устанавливаем новые
                                float& animatedArmor = animatedArmorMap[playerpawn];
                                float& oldArmor = oldArmorMap[playerpawn];
                                float& damageFadeArmor = damageFadeArmorMap[playerpawn];

                                if (animatedArmor == 0.0f) animatedArmor = armor;
                                if (oldArmor == 0.0f) oldArmor = armor;
                                if (damageFadeArmor == 0.0f) damageFadeArmor = 1.0f;

                                float armorBarHeight = boxHeight * (animatedArmor / 100.0f);
                                float armorBarTop = boxTop + (boxHeight - armorBarHeight);
                                float armorPercent = animatedArmor / 100.0f;

                                // Если броня снизилась - запоминаем старое значение
                                if (armor < oldArmor) {
                                    damageFadeArmor = 1.0f; // Сброс прозрачности урона
                                    oldArmor = armor;
                                }

                                // Плавная анимация изменения брони
                                animatedArmor += (armor - animatedArmor) * 0.1f;

                                // Рисуем фон для полосы брони
                                ImGui::GetForegroundDrawList()->AddRectFilled(
                                    ImVec2(boxRight + 3.0f, boxTop),
                                    ImVec2(boxRight + 8.0f, boxBottom),
                                    IM_COL32(0, 0, 0, 255) // Чёрный фон
                                );

                                // Если включён градиент, рисуем вертикальный градиент для брони (снизу вверх)
                                if (gradientArmorBar) {
                                    for (float y = boxBottom; y > armorBarTop; --y) {
                                        float blend = (y - armorBarTop) / armorBarHeight; // Пропорциональное значение для плавного перехода

                                        // Интерполяция цвета от верхнего (низкая броня) к нижнему (высокая броня)
                                        ImVec4 armorColor = ImVec4(
                                            highArmorColor.x * (1.0f - blend) + lowArmorColor.x * blend,
                                            highArmorColor.y * (1.0f - blend) + lowArmorColor.y * blend,
                                            highArmorColor.z * (1.0f - blend) + lowArmorColor.z * blend,
                                            1.0f // Полная непрозрачность
                                        );

                                        // Рисуем маленький горизонтальный отрезок на каждой строке (пикселе)
                                        ImGui::GetForegroundDrawList()->AddLine(
                                            ImVec2(boxRight + 4.0f, y),
                                            ImVec2(boxRight + 7.0f, y),
                                            IM_COL32(
                                                static_cast<int>(armorColor.x * 255),
                                                static_cast<int>(armorColor.y * 255),
                                                static_cast<int>(armorColor.z * 255),
                                                255
                                            ),
                                            1.0f // Ширина линии 1 пиксель
                                        );
                                    }
                                }
                                else {
                                    // Без градиента для брони
                                    ImVec4 armorColor = ImVec4(
                                        lowArmorColor.x * (1.0f - armorPercent) + highArmorColor.x * armorPercent,
                                        lowArmorColor.y * (1.0f - armorPercent) + highArmorColor.y * armorPercent,
                                        lowArmorColor.z * (1.0f - armorPercent) + highArmorColor.z * armorPercent,
                                        1.0f
                                    );

                                    ImGui::GetForegroundDrawList()->AddRectFilled(
                                        ImVec2(boxRight + 4.0f, armorBarTop),
                                        ImVec2(boxRight + 7.0f, boxBottom),
                                        IM_COL32(
                                            static_cast<int>(armorColor.x * 255),
                                            static_cast<int>(armorColor.y * 255),
                                            static_cast<int>(armorColor.z * 255),
                                            255
                                        )
                                    );
                                }

                                // **ЭФФЕКТ "ПАДАЮЩЕЙ" БРОНИ**
                                if (animatedArmor < oldArmor) {
                                    float oldArmorBarHeight = boxHeight * (oldArmor / 100.0f);
                                    float oldArmorBarTop = boxTop + (boxHeight - oldArmorBarHeight);

                                    ImGui::GetForegroundDrawList()->AddRectFilled(
                                        ImVec2(boxRight + 4.0f, oldArmorBarTop),
                                        ImVec2(boxRight + 7.0f, armorBarTop),
                                        IM_COL32(0, 0, 255, static_cast<int>(damageFadeArmor * 255)) // Полоска урона (синяя, исчезает)
                                    );

                                    damageFadeArmor = fmax(damageFadeArmor - 0.05f, 0.0f); // Плавное исчезновение урона
                                }

                                // Рисуем цифры (в комбинированном режиме)
                                if (displayMode == 2) {
                                    char armorText[16];
                                    sprintf_s(armorText, "%d", armor);
                                    ImGui::GetForegroundDrawList()->AddText(
                                        ImVec2(boxRight + 10.0f, armorBarTop - 10.0f),
                                        IM_COL32(255, 255, 255, 255),
                                        armorText
                                    );
                                }
                            }
                        }
                    }

                    TriggerBot triggerbot;
                    std::vector<uintptr_t> targetEntities;
                    for (int i = 0; i < espTargets.size(); i++) {
                        uintptr_t targetEntity = cplayer.getPCSPlayerPawn(base, i);
                        if (targetEntity) {
                            targetEntities.push_back(targetEntity);
                        }
                    }
                    aimbot.AimAtTarget(base, playerpawn, vm, espTargets, espDistances);

                    aimbot.DrawFOVCircle(); // Отрисовка круга

                    triggerbot.RunTriggerbot(base, playerpawn, vm, espTargets, espDistances);

                    //////////GOVNO
                 
                    if (newAimbotEnabled) {
                    
                        newAimbot.AimAtTargetNew(base, playerpawn, vm);  // Новый аимбот, вызываем его метод
                    }
                   
                    

                    //////////GOVNO


                    // Отрисовка линий
                    if (drawLines) {
                        ImVec2 lineStart;

                        if (linePosition == 0) { // Верх экрана
                            lineStart = { screensize.x / 2.0f, 0.0f }; // Центр верхнего края экрана
                            // Рисуем черную обводку для верхней линии
                            if (drawOutline) {
                                ImGui::GetForegroundDrawList()->AddLine(lineStart, ImVec2(screenTop.x, screenTop.y), IM_COL32(0, 0, 0, 255), 3.0f); // Черная обводка
                            }
                            // Рисуем основную линию
                            ImGui::GetForegroundDrawList()->AddLine(lineStart, ImVec2(screenTop.x, screenTop.y), boxColor, 1.5f); // Основная линия
                        }
                        else if (linePosition == 1) { // Центр экрана
                            lineStart = { screensize.x / 2.0f, screensize.y / 2.0f }; // Центр экрана
                            // Рисуем черную обводку для центральной линии
                            if (drawOutline) {
                                ImGui::GetForegroundDrawList()->AddLine(lineStart, ImVec2(screenBottom.x, screenBottom.y), IM_COL32(0, 0, 0, 255), 3.0f); // Черная обводка
                            }
                            // Рисуем основную линию
                            ImGui::GetForegroundDrawList()->AddLine(lineStart, ImVec2(screenBottom.x, screenBottom.y), boxColor, 1.5f); // Основная линия
                        }
                        else if (linePosition == 2) { // Низ экрана
                            lineStart = { screensize.x / 2.0f, screensize.y }; // Центр нижнего края экрана
                            // Рисуем черную обводку для нижней линии
                            if (drawOutline) {
                                ImGui::GetForegroundDrawList()->AddLine(lineStart, ImVec2(screenBottom.x, screenBottom.y), IM_COL32(0, 0, 0, 255), 3.0f); // Черная обводка
                            }
                            // Рисуем основную линию
                            ImGui::GetForegroundDrawList()->AddLine(lineStart, ImVec2(screenBottom.x, screenBottom.y), boxColor, 1.5f); // Основная линия
                        }
                    }
                }
                }
            }
        
            espTargets.clear();
            espDistances.clear();

            overlay.EndRender();
    }

    overlay.DestroyImGui();
    overlay.DestroyDevice();
    overlay.DestroyOverlay();
}
