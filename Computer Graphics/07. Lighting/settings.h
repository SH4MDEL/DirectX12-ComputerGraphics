#pragma once

namespace Settings
{
    constexpr wstring_view TitleName = TEXT("07. Lighting");
    constexpr UINT DefaultWindowWidth = 1920;
    constexpr UINT DefaultWindowHeight = 1080;

    constexpr FLOAT DefaultCameraPitch = XM_PIDIV2 - 0.3f;
    constexpr FLOAT DefaultCameraYaw = 0.f;
    constexpr FLOAT DefaultCameraRadius = 10.f;
    constexpr FLOAT CameraMinPitch = XM_PIDIV2 - 0.6f;
    constexpr FLOAT CameraMaxPitch = XM_PIDIV2 + 0.2f;

    constexpr FLOAT SunRadius = 80.f;

    constexpr FLOAT PlayerSpeed = 10.f;

    namespace Light
    {
        constexpr UINT MaxLight = 100;
        constexpr UINT Directional = 0;
        constexpr UINT Point = 1;
        constexpr UINT Spot = 2;
        constexpr UINT Last = 3;
    }
}

namespace RootParameter
{
    constexpr UINT GameObject = 0;
    constexpr UINT Camera = 1;
    constexpr UINT Material = 2;
    constexpr UINT Light = 3;
    constexpr UINT Instance = 4;
    constexpr UINT TextureCube = 5;
    constexpr UINT Texture = 6;
}

namespace DescriptorRange
{
    constexpr UINT TextureCube = 0;
    constexpr UINT Texture = 1;
}
