#include "stdafx.h"
#include "utilities.h"
#include <random>
AcCmColor getRandomColor()
{
    static std::default_random_engine engine;
    static std::uniform_int_distribution<> dist(0, 255); // Adesk::UInt8 is a unsigned char
    AcCmColor color;
    int blue{ dist(engine) }, green{ dist(engine) }, red{ dist(engine) };
    color.setRGB(blue, green, red);
    return color;
}
