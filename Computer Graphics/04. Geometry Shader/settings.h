#pragma once

namespace Settings
{
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
    constexpr UINT TextureCube = 2;
    constexpr UINT Texture = 3;
}

namespace DescriptorRange
{
    constexpr UINT TextureCube = 0;
    constexpr UINT Texture = 1;
}
