#include "FluidSolver.h"
#include "VTKWriter.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <cstring>
#ifdef _OPENMP
#include <omp.h>
#endif

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -n, --grid SIZE         Grid size (cubic, default: 64)\n";
    std::cout << "  --nx NX                 Grid size in X direction (default: 64)\n";
    std::cout << "  --ny NY                 Grid size in Y direction (default: 64)\n";
    std::cout << "  --nz NZ                 Grid size in Z direction (default: 64)\n";
    std::cout << "  -s, --steps NUM         Number of simulation steps (default: 200)\n";
    std::cout << "  -o, --output-interval N Output every N steps (default: 10)\n";
    std::cout << "  --smoke-steps N         Stop smoke generation after N steps (default: same as --steps)\n";
    std::cout << "  --dt TIMESTEP           Time step size (default: 0.1)\n";
    std::cout << "  --dx SPACING            Grid spacing (default: 1.0)\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << progName << " -n 128 -s 500\n";
    std::cout << "  " << progName << " --nx 128 --ny 64 --nz 64 --steps 1000\n";
    std::cout << "  " << progName << " --dt 0.05 --output-interval 5\n";
    std::cout << "  " << progName << " -s 500 --smoke-steps 100  # Generate smoke for first 100 steps only\n";
}

int main(int argc, char* argv[]) {
    std::cout << "=== 3D Fluid Simulation ===" << std::endl;
#ifdef _OPENMP
    std::cout << "OpenMP threads: " << omp_get_max_threads() << std::endl;
#else
    std::cout << "Running sequentially (OpenMP not available)" << std::endl;
#endif
    
    // Default simulation parameters
    int nx = 64;
    int ny = 64;
    int nz = 64;
    double dx = 1.0;
    double dt = 0.1;
    int num_steps = 200;
    int output_interval = 10;
    int smoke_steps = -1;  // -1 means same as num_steps
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        else if ((arg == "-n" || arg == "--grid") && i + 1 < argc) {
            nx = ny = nz = std::atoi(argv[++i]);
        }
        else if (arg == "--nx" && i + 1 < argc) {
            nx = std::atoi(argv[++i]);
        }
        else if (arg == "--ny" && i + 1 < argc) {
            ny = std::atoi(argv[++i]);
        }
        else if (arg == "--nz" && i + 1 < argc) {
            nz = std::atoi(argv[++i]);
        }
        else if ((arg == "-s" || arg == "--steps") && i + 1 < argc) {
            num_steps = std::atoi(argv[++i]);
        }
        else if ((arg == "-o" || arg == "--output-interval") && i + 1 < argc) {
            output_interval = std::atoi(argv[++i]);
        }
        else if (arg == "--smoke-steps" && i + 1 < argc) {
            smoke_steps = std::atoi(argv[++i]);
        }
        else if (arg == "--dt" && i + 1 < argc) {
            dt = std::atof(argv[++i]);
        }
        else if (arg == "--dx" && i + 1 < argc) {
            dx = std::atof(argv[++i]);
        }
        else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Validate parameters
    if (nx < 8 || ny < 8 || nz < 8) {
        std::cerr << "Error: Grid size must be at least 8 in each direction\n";
        return 1;
    }
    if (num_steps < 1) {
        std::cerr << "Error: Number of steps must be positive\n";
        return 1;
    }
    if (output_interval < 1) {
        std::cerr << "Error: Output interval must be positive\n";
        return 1;
    }
    if (dt <= 0 || dx <= 0) {
        std::cerr << "Error: Time step and grid spacing must be positive\n";
        return 1;
    }
    
    // Set smoke_steps to num_steps if not specified
    if (smoke_steps == -1) {
        smoke_steps = num_steps;
    }
    if (smoke_steps < 0) {
        std::cerr << "Error: Smoke steps must be non-negative\n";
        return 1;
    }
    
    std::cout << "Grid size: " << nx << "x" << ny << "x" << nz << std::endl;
    std::cout << "Time step: " << dt << std::endl;
    std::cout << "Total steps: " << num_steps << std::endl;
    std::cout << "Smoke generation stops at step: " << smoke_steps << std::endl;
    
    // Create solver
    FluidSolver solver(nx, ny, nz, dx, dt);
    
    // Configure wind tunnel inlet velocity (flow from left to right)
    solver.setInletVelocity(5.0, 0.0, 0.0);  // 5.0 m/s in x-direction
    std::cout << "Wind tunnel mode: inlet velocity = 5.0 m/s (x-direction)" << std::endl;
    
    // Add obstacles - create a sphere obstacle in the middle
    std::cout << "Adding obstacles..." << std::endl;
    int cx = nx / 2;
    int cy = ny / 2;
    int cz = nz / 2;
    int radius = 8;
    
    for (int k = 0; k < nz; ++k) {
        for (int j = 0; j < ny; ++j) {
            for (int i = 0; i < nx; ++i) {
                int dx_obs = i - cx;
                int dy_obs = j - cy;
                int dz_obs = k - cz;
                double dist = std::sqrt(dx_obs*dx_obs + dy_obs*dy_obs + dz_obs*dz_obs);
                
                if (dist < radius) {
                    solver.setObstacle(i, j, k, true);
                }
            }
        }
    }
    
    // Add a cylinder obstacle (vertical, along y-axis)
    int cyl_cx = nx/4 + 4;  // Center x position
    int cyl_cz = nz/4 + 4;  // Center z position
    int cyl_radius = 5;     // Cylinder radius
    
    for (int j = 0; j < ny; ++j) {  // Full height (vertical cylinder)
        for (int k = 0; k < nz; ++k) {
            for (int i = 0; i < nx; ++i) {
                int dx_cyl = i - cyl_cx;
                int dz_cyl = k - cyl_cz;
                double dist_cyl = std::sqrt(dx_cyl*dx_cyl + dz_cyl*dz_cyl);
                
                if (dist_cyl < cyl_radius) {
                    solver.setObstacle(i, j, k, true);
                }
            }
        }
    }
    
    std::cout << "Starting simulation..." << std::endl;
    
    // Main simulation loop
    for (int step = 0; step < num_steps; ++step) {
        // Wind tunnel: inject smoke tracers at inlet to visualize flow
        if (step < smoke_steps) {
            // Stream 1: Aimed at BOX OBSTACLE (positioned to hit it directly)
            // Box is at (nx/4, ny/4, nz/4), so align smoke with it
            for (int k = nz/4; k < nz/4 + 8; ++k) {
                for (int j = ny/4; j < ny/4 + 8; ++j) {
                    for (int i = 3; i < 6; ++i) {
                        solver.addSource(i, j, k, 0.06, 0.0);  // STRONG stream hitting box
                    }
                }
            }
            
            // Stream 2: Center stream (hitting sphere obstacle)
            for (int k = nz/2 - 4; k < nz/2 + 4; ++k) {
                for (int j = ny/2 - 4; j < ny/2 + 4; ++j) {
                    for (int i = 3; i < 6; ++i) {
                        solver.addSource(i, j, k, 0.04, 0.0);  // Main stream hitting sphere
                    }
                }
            }
            
            // Stream 3: Upper stream for contrast
            for (int k = nz/2 - 2; k < nz/2 + 2; ++k) {
                for (int j = 3*ny/4; j < 3*ny/4 + 3; ++j) {
                    for (int i = 3; i < 6; ++i) {
                        solver.addSource(i, j, k, 0.03, 0.0);  // Upper stream
                    }
                }
            }
        }
        
        // Perform simulation step
        auto start_time = std::chrono::high_resolution_clock::now();
        solver.step();
        auto end_time = std::chrono::high_resolution_clock::now();
        
        // Output progress
        if (step % output_interval == 0) {
            double elapsed_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
            std::cout << "Step " << std::setw(4) << step 
                     << " / " << num_steps 
                     << " - Time: " << std::fixed << std::setprecision(3) 
                     << elapsed_ms << " ms" << std::endl;
            
            // Write VTK file (XML format)
            std::ostringstream filename;
            filename << "output_" << std::setw(4) << std::setfill('0') << step << ".vti";
            
            VTKWriter::writeVTK(filename.str(),
                              solver.getNx(), solver.getNy(), solver.getNz(),
                              solver.getDx(),
                              solver.getDensity(),
                              solver.getTemperature(),
                              solver.getVelocityU(),
                              solver.getVelocityV(),
                              solver.getVelocityW(),
                              solver.getObstacles());
        }
    }
    
    std::cout << "\nSimulation complete!" << std::endl;
    std::cout << "VTK files saved (XML format). Open in ParaView to visualize." << std::endl;
    std::cout << "\nParaView tips:" << std::endl;
    std::cout << "- Load output_*.vti files (File -> Open)" << std::endl;
    std::cout << "- Visualize 'density' scalar for smoke" << std::endl;
    std::cout << "- Visualize 'temperature' scalar for heat" << std::endl;
    std::cout << "- Visualize 'velocity' vector with glyphs or streamlines" << std::endl;
    std::cout << "- Visualize 'obstacle' scalar to see obstacles" << std::endl;
    
    return 0;
}
