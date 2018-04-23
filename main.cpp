#include <iostream> // std::cout
#include <fstream> // std::ifstream
#include <cmath> // std::abs
#include <algorithm> // std::min_element

int main(int argc, char* argv[]){
    if (argc != 3) {
        std::cout << "Invalid number of arguments";
        return 1;
    }

    std::string filename = argv[1];
    int numSeams = std::stoi(argv[2]);

    std::ifstream picStream(filename);
    int width, height, maxVal, num;

    picStream.ignore(256, '\n'); // Ignore the first line ("P2")
    picStream.ignore(256, '\n'); // Ignore the second line (comment)
    picStream >> width;
    picStream >> height;
    picStream >> maxVal;

    // Read in each pixel value and store in matrix
    int pixels[height][width];
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            picStream >> pixels[i][j];
        }
    }
    picStream.close();

    for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                std::cout << pixels[i][j] << ' ';
            }
            std::cout << std::endl;
        }

    for (int iterations = 0; iterations < int(numSeams); iterations++) {

        // Calculate energy value of each pixel
        int energies[height][width];
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                int energy = 0;

                if (i != 0) { // Top edge of picture
                    energy += std::abs(pixels[i-1][j] - pixels[i][j]);
                } if (i != height-1) { // Bottom edge of picture
                    energy += std::abs(pixels[i+1][j] - pixels[i][j]);
                } if (j != 0) { // Left edge
                    energy += std::abs(pixels[i][j-1] - pixels[i][j]);
                } if (j != width-1) { // Right edge
                    energy += std::abs(pixels[i][j+1] - pixels[i][j]);
                }

                energies[i][j] = energy;
            }
        }

        int accumCost[height][width];
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                    if (i == 0)
                        accumCost[i][j] = energies[i][j]; // Top row is equal to top energy row
                    else {
                        int candidates[3] = {99999, 99999, 99999};
                        candidates[1] = accumCost[i-1][j]; // Pixel straight above
                        if(j != 0)
                            candidates[0] = accumCost[i-1][j-1]; // Pixel above and to the left
                        if(j != width-1)
                            candidates[2] = accumCost[i-1][j+1]; // Pixel above and to the right

                        int minimum = *(std::min_element(std::begin(candidates), std::end(candidates)));
                        accumCost[i][j] = energies[i][j] + minimum;
                    }
            }
        }

        int path[height]; // Will hold indexes of pixels to be removed in each row
        int* minCost = std::min_element(&accumCost[height-1][0], &accumCost[height-1][width-1]+1); // Pointer to index with lowest value in the last row
        path[height-1] = minCost - &accumCost[height-1][0]; // Subtract the two pointers to find the index

        for(int i = height - 2; i >= 0; i--) {
            /*
            int candidates[3] = {99999, 99999, 99999};
            candidates[1] = accumCost[i][path[i+1]]; // Pixel straight above
            if(path[i+1] != 0)
                candidates[0] = accumCost[i][path[i+1]-1]; // Pixel above and to the left
            if(path[i+1] != width-1)
                candidates[2] = accumCost[i][path[i+1]+1]; // Pixel above and to the right

            int minimum = *(std::min_element(std::begin(candidates), std::end(candidates)));
            */

            int* b;
            int* e;
            if(path[i+1] == 0) {
                b = &accumCost[i][path[i+1]];
                e = &accumCost[i][path[i+1]+1];
            } else if(path[i+1] == width-1) {
                b = &accumCost[i][path[i+1]-1];
                e = &accumCost[i][path[i+1]];
            } else {
                b = &accumCost[i][path[i+1]-1];
                e = &accumCost[i][path[i+1]+1];
            }
            int* minimum = std::min_element(b, e+1); // e+1 because the end limit is non-inclusive
            path[i] = minimum - &accumCost[i][0];
        }

        std::cout << "Path: ";
        for (int i : path) {
            std::cout << i << " ";
        }
        std::cout << std::endl;

        int oldImage[height][width];
        std::copy(&pixels[0][0], &pixels[height-1][width-1]+1, &oldImage[0][0]);

        int pixels[height][width-1];
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (j < path[i])
                    pixels[i][j] = oldImage[i][j];
                else if(j > path[i])
                    pixels[i][j-1] = oldImage[i][j];
            }
        }

        width--;

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                std::cout << pixels[i][j] << ' ';
            }
            std::cout << std::endl;
        }

        /*
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width-1; ++j)
            {
                std::cout << newImage[i][j] << ' ';
            }
            std::cout << std::endl;
        }
        */
    }

    return 0;
}
