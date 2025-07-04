#include "general_utils.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <shoreside_log_dir> <out_dir>" << std::endl;
        return 1;
    }

    std::string shoreside_log_dir = argv[1];
    std::string out_dir = argv[2];

    if (processNodeReports(shoreside_log_dir, out_dir))
        return 0;
    else
        return 2;
}
