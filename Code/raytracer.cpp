#include "tools.h"
#include "ppmWriter.h"
#include <iostream>

int main()
{
    Tools tools;
    tools.readConfig("../TestSuite/scene.json");
    int width = 1200;
    int height = 800;
    std::vector<unsigned char> backgrounddata = {64, 64, 64};
    PPMWriter ppmwriter(width, height,backgrounddata);
    tools.render(ppmwriter);
    ppmwriter.writePPM("output.ppm");
    return 0;
}