#pragma once

// Хранилище внешних шейдерных параметров, которые читаются в Blender_Recorder_StandartBinding.cpp
class ShadersExternalData //--#SM+#--
{
public:
    Fmatrix m_script_params; // Матрица, значения которой доступны из Lua
    Fvector4 hud_params;     // [zoom_rotate_factor, fov_secondVP, NULL, NULL] - Параметры худа оружия
    Fvector4 m_blender_mode; // x\y = [0 - default, 1 - night vision, 2 - thermo vision, ... см. common.h] - Режимы рендеринга
                             // x - основной вьюпорт, y - второй впьюпорт, z = [0.0f / 1.0f - выключен или включён двойной ренденр], w = [0.0f - идёт рендер обычного объекта, 1.0f - идёт рендер детальных объектов (трава, мусор)]

    ShadersExternalData()
    {
        m_script_params = Fmatrix();
        hud_params.set(0.f, 0.f, 0.f, 0.f);
        m_blender_mode.set(0.f, 0.f, 0.f, 0.f);
    }
};
