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
        constexpr int QTI_PIN_0 = 47; // Left QTI
        constexpr int QTI_PIN_1 = 51;   // Middle QTI
        constexpr int QTI_PIN_2 = 52;  // Right QTI

        constexpr int QTI_THRESH_0 = 600; // Left QTI
        constexpr int QTI_THRESH_1 = 300; // Left QTI
        constexpr int QTI_THRESH_2 = 400; // Left QTI
    }

}

#endif // CONFIG_H