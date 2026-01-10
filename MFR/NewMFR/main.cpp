#include <iostream>

#include "Mfr.h"
#include "MfrConfig.h"

int main()
{
    MfrConfig &config = MfrConfig::getInstance();
    if (!config.loadConfig("./../Config/MFR.ini"))
    {
        std::cerr << "Failed to load configuration" << std::endl;
        return 1;
    }

    auto mfr = std::make_shared<Mfr>();
    mfr->initialize();

    while (true)
    {
    }

    return 0;
}