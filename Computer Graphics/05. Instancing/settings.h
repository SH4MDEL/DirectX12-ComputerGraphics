#pragma once

namespace Settings
{
    constexpr wstring_view TitleName = TEXT("05. Instancing");
    constexpr UINT DefaultWindowWidth = 1920;
    constexpr UINT DefaultWindowHeight = 1080;

    constexpr FLOAT DefaultCameraPitch = XM_PIDIV2 - 0.3f;
    constexpr FLOAT DefaultCameraYaw = 0.f;
    constexpr FLOAT DefaultCameraRadius = 10.f;
    constexpr FLOAT CameraMinPitch = XM_PIDIV2 - 0.6f;
    constexpr FLOAT CameraMaxPitch = XM_PIDIV2 + 0.2f;

    constexpr FLOAT PlayerSpeed = 10.f;
}

namespace RootParameter
{
    constexpr UINT GameObject = 0;
    constexpr UINT Camera = 1;
    constexpr UINT Instance = 2;
    constexpr UINT Texture = 3;
    constexpr UINT TextureCube = 4;
}

namespace DescriptorRange
{
    constexpr UINT Texture = 0;
    constexpr UINT TextureCube = 1;
}
