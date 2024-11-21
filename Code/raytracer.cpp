#include "tools.h"
#include "ppmWriter.h"
#include <iostream>

int main()
{
    Tools tools;
    tools.readConfig("../TestSuite/binary_primitives.json");
    int width = 1200;
    int height = 800;
    PPMWriter ppmwriter(width, height, {64, 64, 64});
    tools.render(ppmwriter);
    ppmwriter.writePPM("output.ppm");
    return 0;
}