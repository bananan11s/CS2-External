bool watermark = true;
std::string namestring = "EXTERNAL";

ImColor bordercolor = ImColor(62, 14, 185, 200);
ImColor bgcolor = ImColor(42, 6, 108, 100);

std::string GetSystemInfo() {
    // Получение текущего времени
    time_t now = time(nullptr);
    struct tm localTime;
    localtime_s(&localTime, &now); // Используем безопасный вариант localtime

    char timeBuffer[6]; // HH:mm + \0
    strftime(timeBuffer, sizeof(timeBuffer), "%H:%M", &localTime);

    // Получение имени пользователя
    char userName[256];
    DWORD userNameSize = sizeof(userName);
    if (!GetUserNameA(userName, &userNameSize)) {
        snprintf(userName, sizeof(userName), "UnknownUser");
    }

    // Формирование строки с временем и именем пользователя
    std::string systemInfo = "|" + std::string(timeBuffer) + "|" + std::string(userName) + "|";

    return systemInfo;
}
void DrawWatermark()
{
    if (watermark) {
        ImGuiIO& io = ImGui::GetIO();

        // Получаем текст с временем и именем ПК
        std::string watertext = GetSystemInfo();
        watertext += std::string(("t.me/LastNighSO2|"));
        watertext += namestring; // Имя пользователя или другой текст

        auto textsize = 15;
        auto height = 0;
        auto textSizeX = ((ImGui::CalcTextSize(watertext.c_str()).x) / ImGui::GetFontSize()) * textsize;

        // Поднимаем на пару пикселей вверх
        int offsetY = -2;

        ImVec2 rectMin(20, 30 + height + offsetY);
        ImVec2 rectMax(-40 + 80 + textSizeX, 65 - 10 + offsetY);

        // Рисуем фон водяного знака
        ImGui::GetBackgroundDrawList()->AddRectFilled(rectMin, rectMax, bgcolor, 0, 0);

        // Рисуем рамку
        ImGui::GetBackgroundDrawList()->AddLine(ImVec2(rectMin.x, rectMax.y), ImVec2(rectMax.x, rectMax.y), bordercolor, 2);

        // Рисуем текст водяного знака
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), textsize,
            ImVec2(30, (25 + 20 - (((ImGui::CalcTextSize(watertext.c_str()).y / 2) / ImGui::GetFontSize()) * textsize) + height + offsetY) - 3),
            ImColor(255, 255, 255), // Цвет текста всегда белый
            watertext.c_str());
    }
}
