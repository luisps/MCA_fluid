#pragma once

#include <string>
#include <vector>

class VTKWriter {
public:
    // Write VTK file using VTK library (outputs .vti XML format)
    static void writeVTK(const std::string& filename,
                        int nx, int ny, int nz,
                        double dx,
                        const std::vector<double>& density,
                        const std::vector<double>& temperature,
                        const std::vector<double>& u,
                        const std::vector<double>& v,
                        const std::vector<double>& w,
                        const std::vector<bool>& obstacles);
};
