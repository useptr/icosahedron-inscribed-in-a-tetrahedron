#include "stdafx.h"
#include "utilities.h"
#include <random>
AcCmColor getRandomColor()
{
    static std::default_random_engine engine;
    static std::uniform_int_distribution<Adesk::UInt8> dist(0, 255);
    AcCmColor color;
    Adesk::UInt8 blue{ dist(engine) }, green{ dist(engine) }, red{ dist(engine) };
    color.setRGB(blue, green, red);
    return color;
}
