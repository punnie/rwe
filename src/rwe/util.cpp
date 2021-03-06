#include "util.h"

namespace rwe
{
    std::optional<boost::filesystem::path> getLocalDataPath()
    {
        auto appData = std::getenv("APPDATA");
        if (appData == nullptr)
        {
            return std::nullopt;
        }

        boost::filesystem::path path(appData);
        path /= "RWE";

        return path;
    }

    std::optional<boost::filesystem::path> getSearchPath()
    {
        auto path = getLocalDataPath();
        if (!path)
        {
            return std::nullopt;
        }


        *path /= "Data";

        return *path;
    }

    float toRadians(float v)
    {
        return v * (Pif / 180.0f);
    }

    RadiansAngle toRadians(TaAngle angle)
    {
        return RadiansAngle(static_cast<float>(angle.value) * (Pif / 32768.0f));
    }

    float toleranceToRadians(unsigned int angle)
    {
        return static_cast<float>(angle) * (Pif / 32768.0f);
    }

    TaAngle toTaAngle(RadiansAngle angle)
    {
        return TaAngle(static_cast<int16_t>(angle.value * (32768.0f / Pif)));
    }
}
