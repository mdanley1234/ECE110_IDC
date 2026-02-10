#ifndef CONFIG_H
#define CONFIG_H

namespace RobotConfig {
    
    // Pins for sensors
    namespace Pins {
        constexpr int QTI_LEFT   = 47;
        constexpr int QTI_MIDDLE = 48;
        constexpr int QTI_RIGHT  = 49;
    }

    namespace QTI {
        constexpr int QTI_PIN_0   = 47;
        constexpr int QTI_PIN_1 = 51;
        constexpr int QTI_PIN_2  = 52;
    }

}

#endif // CONFIG_H