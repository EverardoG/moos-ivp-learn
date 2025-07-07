#include "general_utils.h"

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << " <input_csv> <output_csv> [verbose]" << std::endl;
        return 1;
    }

    std::string input_csv = argv[1];
    std::string output_csv = argv[2];
    int verbose = 0;

    if (argc == 4) {
        try {
            verbose = std::stoi(argv[3]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid verbosity level (This argument must be an integer): " << argv[3] << std::endl;
            return 1;
        }
    }

    if (csvFilterDuplicateRows(input_csv, output_csv, verbose))
        return 0;
    else
        return 2;
}
