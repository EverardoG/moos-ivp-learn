#include "general_utils.h"
#include <vector>
#include <string>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <directory> [-e <vehicle>] [--exclude <vehicle>] ..." << std::endl;
        return 1;
    }

    std::string directory = argv[1];
    std::vector<std::string> exclude_vehicles;

    // Parse exclude flags only if they are at the end
    int i = 2;
    while (i < argc) {
        std::string arg = argv[i];
        if ((arg == "-e" || arg == "--exclude") && (i + 1 < argc)) {
            exclude_vehicles.push_back(argv[i + 1]);
            i += 2;
        } else {
            std::cerr << "Usage: " << argv[0] << " <directory> [-e <vehicle>] [--exclude <vehicle>] ..." << std::endl;
            return 1;
        }
    }

    if (csvMergeFiles(directory, exclude_vehicles))
        return 0;
    else
        return 2;
}
