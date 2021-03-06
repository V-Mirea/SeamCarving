#include <iostream> // std::cout
#include <fstream> // std::fstream
#include <cmath> // std::abs
#include <algorithm> // std::min_element

using std::vector;

void printMatrix(std::vector<std::vector<int>> matrix, std::ostream& out = std::cout){
    vector<vector<int>>::iterator row;
    vector<int>::iterator col;
    for (row = matrix.begin(); row != matrix.end(); row++) {
        for (col = row->begin(); col != row->end(); col++) {
            out << *col << " ";
        }
        out << "\n";
    }
    out << "\n";
}

vector<vector<int>> rotateMatrix(vector<vector<int>> matrix) {
    int height = matrix.size();
    int width = matrix[0].size();

    vector<vector<int>> rotated(width, vector<int>(height));

    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            rotated[i][j] = matrix[height-1-j][i];
        }
    }

    return rotated;
}

void createPGM(vector<vector<int>> pixels, std::string fileName) {
    int width = pixels[0].size();
    int height = pixels.size();

    std::ofstream ostr(fileName + "_processed.pgm");
    ostr << "P2\n";
    ostr << "#Generated by removing least visible seams horizontally and vertically\n";
    ostr << width << " " << height << "\n";
    ostr << "255\n";
    printMatrix(pixels, ostr);

    //printMatrix(pixels);
}

std::vector<std::vector<int>> removeSeam(std::vector<std::vector<int>> pixels) {
    int height = pixels.size();
    int width = pixels[0].size();

    // Calculate energy value of each pixel
    vector<vector<int>> energies(height, vector<int>(width));
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

    // Calculate lowest cumulative path value for each pixel
    vector<vector<int>> accumCost(height, vector<int>(width));
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
                if (i == 0)
                    accumCost[i][j] = energies[i][j]; // Top row is equal to top energy row
                else {
                    int* b;
                    int* e;
                    if(j == 0){
                        b = &accumCost[i-1][j]; // Pixel straight above
                        e = &accumCost[i-1][j+1]; // Pixel above and to the right
                    } else if(j == width-1) {
                        b = &accumCost[i-1][j-1]; // left
                        e = &accumCost[i-1][j]; // above
                    } else {
                        b = &accumCost[i-1][j-1]; // left
                        e = &accumCost[i-1][j+1]; // right
                    }

                    int minimum = *(std::min_element(b, e+1));
                    accumCost[i][j] = energies[i][j] + minimum;
                }
        }
    }

    // Calculate path of seam
    int path[height]; // Will hold indexes of pixels to be removed in each row
    int* minCost = std::min_element(&accumCost[height-1][0], &accumCost[height-1][width-1]+1); // Pointer to index with lowest value in the last row
    path[height-1] = minCost - &accumCost[height-1][0]; // Subtract the two pointers to find the index

    for(int i = height - 2; i >= 0; i--) {
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

    // Remove seam from picture
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == path[i]) {
                pixels[i].erase(pixels[i].begin() + j);
                break;
            }
        }
    }

    return pixels;
}

int main(int argc, char* argv[]){
    if (argc != 3) {
        std::cout << "Invalid number of arguments";
        return 1;
    }

    std::string filename = argv[1];
    int numSeams = std::stoi(argv[2]);

    std::ifstream picStream(filename);
    int width, height, num;
    int dim[3];

    for(int i = 0; i < 3;) {
        if(picStream.peek() == '#' || picStream.peek() == 'P')
            picStream.ignore(10000, '\n');
        else
            picStream >> dim[i++];
    }

    width = dim[0]; // because im a hack and dont want to rename variables
    height = dim[1];// after making changes

    /*
    picStream.ignore(256, '\n'); // Ignore the first line ("P2")
    picStream.ignore(256, '\n'); // Ignore the second line (comment)
    picStream >> width;
    picStream >> height;
    picStream >> maxVal;
    */

    // Read in each pixel value and store in matrix
    vector<vector<int>> pixels(height, vector<int>(width));
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            picStream >> pixels[i][j];
        }
    }
    picStream.close();

    for (int i = 0; i < numSeams; i++)
        pixels = removeSeam(pixels);

    vector<vector<int>> rotated = rotateMatrix(pixels);

    for (int i = 0; i < numSeams; i++)
        rotated = removeSeam(rotated);

    for (int i = 0; i < 3; i++)
        rotated = rotateMatrix(rotated);

    createPGM(rotated, filename.substr(0, filename.size()-4)); // Pass in image matrix and filename without extension

    return 0;
}
