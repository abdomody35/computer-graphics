#include "../Image.h"
#include "../Terrain.h"

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

    return 0;
}
