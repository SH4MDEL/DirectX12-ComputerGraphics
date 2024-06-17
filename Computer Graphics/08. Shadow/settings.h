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

    constexpr UINT MaxDirectionalLight = 5;
    constexpr UINT MaxPointLight = 10;
    constexpr UINT MaxSpotLight = 130;

    namespace Light
    {
        constexpr UINT MaxLight = 30;
        constexpr UINT Directional = 0;
        constexpr UINT Point = 1;
        constexpr UINT Spot = 2;
    }
}

namespace RootParameter
{
    constexpr UINT GameObject = 0;
    constexpr UINT Camera = 1;
    constexpr UINT Shadow = 2;
    constexpr UINT Material = 3;
    constexpr UINT Light = 4;
    constexpr UINT Instance = 5;
    constexpr UINT TextureCube = 6;
    constexpr UINT TextureShadow = 7;
    constexpr UINT Texture = 8;
}

namespace DescriptorRange
{
    constexpr UINT TextureCube = 0;
    constexpr UINT TextureShadow = 1;
    constexpr UINT Texture = 2;
}
