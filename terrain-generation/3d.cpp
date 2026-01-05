#include "../Image.h"
#include "../Terrain.h"
#include "../Terrain3DVisualization.h"
#include <time.h>

int main()
{
    GrayscaleImage elevation;
    ColorImage biomes;

    // using default seed
    // terrain::TerrainGenerator generator;

    // random seed based on current time
    terrain::TerrainGenerator generator(time(NULL));

    generator.generateTerrain(elevation, biomes, 512, 512);

    elevation.Save("elevation.png");
    biomes.Save("biomes.png");

    // Generate hillshade (shaded relief) - looks 3D!
    ColorImage hillshade = terrain::generateHillshade(
        elevation, biomes,
        315.0f, // azimuth (sun angle: 315 = northwest)
        45.0f,  // altitude (sun height: 45 degrees)
        2.0f    // z-factor (height exaggeration)
    );

    hillshade.Save("hillshade.png");

    return 0;
}