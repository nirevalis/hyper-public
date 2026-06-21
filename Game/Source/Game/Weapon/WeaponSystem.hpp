#pragma once

namespace Hyper
{
    class WeaponSystem
    {
    private:
        double m_TimeAccumulation = 0;

    public:
        WeaponSystem() = default;
        ~WeaponSystem() = default;

        void Update(double deltaTime);
    };
}