#ifndef CUSTOM_WIDGETS_H
#define CUSTOM_WIDGETS_H

#include "imgui/imgui.h"

// Функция для линейной интерполяции между двумя цветами
ImVec4 LerpColor(const ImVec4& colorA, const ImVec4& colorB, float t)
{
    return ImVec4(
        colorA.x + t * (colorB.x - colorA.x),
        colorA.y + t * (colorB.y - colorA.y),
        colorA.z + t * (colorB.z - colorA.z),
        colorA.w + t * (colorB.w - colorA.w)
    );
}

void CustomCheckbox(const char* label, bool* v, const ImVec4& accent_color)
{
    static float anim_t[3] = { 0.0f, 0.0f, 0.0f };  // Отдельные переменные для каждого чекбокса
    int index = label[strlen(label) - 1] - '6';    // Получаем индекс на основе последнего символа имени чекбокса

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImVec2(20, 20);

    ImGui::InvisibleButton(label, size);
    if (ImGui::IsItemClicked())
    {
        *v = !*v;
    }

    if (*v)
    {
        anim_t[index] += ImGui::GetIO().DeltaTime * 12.0f;
        if (anim_t[index] > 1.0f) anim_t[index] = 1.0f;
    }
    else
    {
        anim_t[index] -= ImGui::GetIO().DeltaTime * 12.0f;
        if (anim_t[index] < 0.0f) anim_t[index] = 0.0f;
    }

    draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(50, 50, 50, 255), 5.0f);

    ImVec4 inactive_color = ImVec4(0.39f, 0.39f, 0.39f, 1.0f);
    ImVec4 current_color = LerpColor(inactive_color, accent_color, anim_t[index]);
    draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::GetColorU32(current_color), 5.0f);

    if (anim_t[index] > 0.0f)
    {
        float scale = anim_t[index];
        ImVec2 check_start = ImVec2(pos.x + 5, pos.y + 10);
        ImVec2 check_mid = ImVec2(pos.x + 8, pos.y + 14);
        ImVec2 check_end = ImVec2(pos.x + 14, pos.y + 6);

        draw_list->PathLineTo(check_start);
        draw_list->PathLineTo(check_mid);
        draw_list->PathLineTo(check_end);
        draw_list->PathStroke(IM_COL32(0, 0, 0, 255), false, 2.0f * scale);
    }

    ImGui::SameLine();
    ImGui::Text(label);
}

void CustomCombo(const char* label, int* current_item, const char* const items[], int items_count, const ImVec4& accent_color)
{
    ImGui::PushID(label);

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImVec2(200, 30);  // Размер кнопки-комбо

    // Стиль кнопки для ComboBox с тонкой обводкой
    ImGui::InvisibleButton(label, size);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Цвет фона и тонкой обводки для комбо бокса
    ImVec4 button_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);  // Чёрный фон
    draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::GetColorU32(button_color), 5.0f);
    draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::GetColorU32(accent_color), 5.0f, 0, 1.0f); // Тонкая обводка

    // Проверяем, что индекс в пределах массива
    if (*current_item < 0) *current_item = 0;
    if (*current_item >= items_count) *current_item = items_count - 1;

    // Текущий выбранный элемент
    ImVec2 text_pos = ImVec2(pos.x + 10, pos.y + (size.y - ImGui::CalcTextSize(items[*current_item]).y) * 0.5f);
    draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), items[*current_item]);  // Белый текст выбранного элемента

    // Если комбо открыт
    static float anim_height = 0.0f;
    static bool is_open = false;

    if (ImGui::IsItemClicked())
    {
        is_open = !is_open;
        anim_height = 0.0f;  // Начальная высота для анимации
    }

    // Анимация скольжения вниз
    if (is_open)
    {
        ImVec2 dropdown_pos = ImVec2(pos.x, pos.y + size.y);
        ImGui::SetNextWindowPos(dropdown_pos);
        ImGui::SetNextWindowSize(ImVec2(size.x, anim_height));

        if (ImGui::Begin("##combo_dropdown", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar))
        {
            anim_height += ImGui::GetIO().DeltaTime * 200.0f;
            if (anim_height > items_count * size.y) anim_height = items_count * size.y;

            for (int i = 0; i < items_count; ++i)
            {
                bool is_selected = (i == *current_item);

                ImGui::PushID(i);

                if (ImGui::Selectable(items[i], is_selected))
                {
                    *current_item = i;
                    is_open = false;
                }

                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }

                ImGui::PopID();
            }

            draw_list->AddRect(dropdown_pos, ImVec2(dropdown_pos.x + size.x, dropdown_pos.y + anim_height), ImGui::GetColorU32(accent_color), 5.0f, 0, 1.5f);

            ImGui::End();
        }
    }

    ImGui::PopID();
}

void CustomSlider(const char* label, float* v, float v_min, float v_max, const ImVec4& accent_color)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImVec2(150, 10);  // Размер слайдера

    static float displayed_value = *v;
    float sliderWidth = (displayed_value - v_min) / (v_max - v_min) * size.x;

    draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(50, 50, 50, 255), 4.0f);

    draw_list->AddRectFilled(pos, ImVec2(pos.x + sliderWidth, pos.y + size.y), ImGui::GetColorU32(accent_color), 4.0f);

    ImVec2 circle_center = ImVec2(pos.x + sliderWidth, pos.y + size.y / 2);
    draw_list->AddCircleFilled(circle_center, 6.0f, IM_COL32(255, 255, 255, 255));  // Отрисовка белого кружочка

    ImGui::InvisibleButton(label, size);
    if (ImGui::IsItemActive())
    {
        *v = v_min + (ImGui::GetMousePos().x - pos.x) / size.x * (v_max - v_min);

        if (*v < v_min) *v = v_min;
        if (*v > v_max) *v = v_max;
    }

    displayed_value += (ImGui::GetIO().DeltaTime * 8.0f) * (*v - displayed_value);

    ImGui::SameLine();
    ImGui::Text("%s: %.2f", label, *v);
}


void CustomInputText(const char* label, char* buffer, size_t buffer_size, const char* placeholder)
{
    // Проверяем, передано ли пустое название для элемента, если пустое — используем дефолтное
    if (label == nullptr || strlen(label) == 0)
    {
        label = "##hidden";  // "##" используется для скрытия лейбла в ImGui
    }

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImVec2(196, 30);  // Немного увеличиваем высоту для устранения черной полосы

    ImVec4 dark_gray_color = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);  // Очень темно-серый цвет
    ImVec4 border_color = ImGui::GetStyle().Colors[ImGuiCol_Border];  // Цвет рамки

    // Настраиваем стили для InputText, чтобы он выглядел плоским и очень темным
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);          // Закругленные углы
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 7));   // Внутренние отступы
    ImGui::PushStyleColor(ImGuiCol_FrameBg, dark_gray_color);        // Темно-серый цвет фона
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255)); // Белый текст

    // Устанавливаем точную ширину элемента, чтобы он соответствовал кастомному размеру
    ImGui::SetNextItemWidth(size.x);

    // Само текстовое поле
    if (ImGui::InputText(label, buffer, buffer_size))
    {
        // Действия при изменении текста
    }

    // Восстанавливаем стили
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Отрисовываем рамку
    draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::ColorConvertFloat4ToU32(border_color), 5.0f);

    // Если текстовое поле пустое и оно не активно (не в фокусе), показываем placeholder
    if (strlen(buffer) == 0 && !ImGui::IsItemActive())
    {
        ImVec2 placeholder_pos = ImVec2(pos.x + 10, pos.y + 7);  // Позиция текста placeholder
        draw_list->AddText(placeholder_pos, IM_COL32(150, 150, 150, 255), placeholder);  // Placeholder текст
    }
}



// Обычная кнопка
void CustomButton(const char* label, bool* clicked, const ImVec4& accent_color)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImVec2(100, 30);

    ImGui::InvisibleButton(label, size);
    *clicked = ImGui::IsItemActive();

    static float anim_t = 0.0f;
    static bool pressed = false;

    if (*clicked)
    {
        anim_t += ImGui::GetIO().DeltaTime * 12.0f;
        if (anim_t > 1.0f) anim_t = 1.0f;
        pressed = true;
    }
    else
    {
        if (pressed)
        {
            anim_t -= ImGui::GetIO().DeltaTime * 12.0f;
            if (anim_t <= 0.0f)
            {
                anim_t = 0.0f;
                pressed = false;
            }
        }
    }

    float scale_factor = 1.0f - (0.1f * anim_t);
    ImVec2 button_pos = ImVec2(pos.x + size.x * (1.0f - scale_factor) / 2, pos.y + size.y * (1.0f - scale_factor) / 2);
    ImVec2 button_size = ImVec2(size.x * scale_factor, size.y * scale_factor);

    draw_list->AddRectFilled(button_pos, ImVec2(button_pos.x + button_size.x, button_pos.y + button_size.y), ImGui::GetColorU32(accent_color), 5.0f);

    ImVec2 text_size = ImGui::CalcTextSize(label);
    ImVec2 text_pos = ImVec2(button_pos.x + (button_size.x - text_size.x) * 0.5f, button_pos.y + (button_size.y - text_size.y) * 0.5f);
    draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), label);
}


//STYLES///




void ApplyDarkThemeWithRoundedCorners(const ImVec4& accent_color)
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Устанавливаем общие цвета
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);  // Темный фон окна
    colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);  // Темный фон чилдов
    colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);  // Бордеры чуть светлее черного
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);  // Фон для фреймов
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);  // Фон при наведении
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);  // Фон при активном элементе
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);  // Убираем тени

    // Устанавливаем закругление углов
    style.WindowRounding = 10.0f;  // Закругляем углы окна
    style.ChildRounding = 10.0f;   // Закругляем углы чилдов
    style.FrameRounding = 5.0f;    // Легкое закругление углов для фреймов и полей ввода

    // Устанавливаем толщину бордеров
    style.FrameBorderSize = 1.0f;  // Толщина бордера для фреймов
    style.WindowBorderSize = 1.0f; // Толщина бордера для окна
    style.ChildBorderSize = 1.0f;  // Толщина бордера для чилдов

    // Пример стиля для других элементов (кнопки, заголовки и т.д.)
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);  // Темный фон заголовка
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);  // При активном заголовке
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);  // Когда заголовок свернут
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);  // Кнопки
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);  // При наведении на кнопку
    colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);  // При нажатии кнопки

    // Убираем тени
    style.WindowRounding = 10.0f;
    style.PopupRounding = 5.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);  // Центрирование заголовков окон
}


#endif // CUSTOM_WIDGETS_H
