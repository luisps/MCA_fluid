# 3D Fluid Simulation with OpenMP

A high-performance 3D fluid simulation for smoke dynamics using C++ and OpenMP acceleration. The simulation implements the Navier-Stokes equations with temperature-driven buoyancy and obstacle handling.

> **⚠️ Disclaimer**: This is a **teaching and learning code** designed for educational purposes. While it implements physically-based fluid dynamics, it prioritizes clarity and ease of understanding over production-level optimization. The code is suitable for learning CFD concepts, prototyping, and visualization, but not intended for critical engineering applications or high-accuracy scientific simulations.

## Features

- **3D Navier-Stokes solver** with semi-Lagrangian advection
- **Temperature-based buoyancy** for realistic smoke behavior
- **Closed boundary conditions** - smoke stays contained in volume
- **Obstacle support** with arbitrary geometry
- **OpenMP acceleration** for parallel computation
- **VTK output** for visualization in ParaView
- **Command line control** for simulation parameters

## Requirements

- C++17 compatible compiler (GCC, Clang, or MSVC)
- CMake 3.15 or higher
- **VTK library** (required for output)
  - macOS: `brew install vtk`
  - Linux: `sudo apt-get install libvtk9-dev` or build from source
  - Windows: Download from vtk.org or use vcpkg
- OpenMP support (optional, but recommended for performance)
  - macOS: `brew install libomp`
  - Linux: Usually pre-installed with GCC
  - Windows: Included with MSVC
- ParaView (for visualization)

## Building

```bash
# Install dependencies (macOS)
brew install vtk libomp

# Install dependencies (Linux - Ubuntu/Debian)
# sudo apt-get install libvtk9-dev libomp-dev

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
make

# Or for a release build with optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

**Note for macOS users:** The CMakeLists.txt automatically detects Homebrew installations of both VTK and libomp. If OpenMP is not found, the simulation will still compile and run sequentially (without parallelization).

## Running

```bash
# Run with default parameters (64³ grid, 200 steps)
./fluid_sim

# Show help and available options
./fluid_sim --help

# Custom grid size and steps
./fluid_sim -n 128 -s 500

# Non-cubic grid
./fluid_sim --nx 128 --ny 64 --nz 64

# Adjust time step and output frequency
./fluid_sim --dt 0.05 -o 5 -s 1000

# Generate smoke for first 100 steps, then watch it dissipate
./fluid_sim -s 500 --smoke-steps 100

# Output files will be generated in the current directory (VTK XML format)
# Files: output_0000.vti, output_0010.vti, ..., etc.
```

### Command Line Options

- `-h, --help` - Show help message
- `-n, --grid SIZE` - Cubic grid size (default: 64)
- `--nx, --ny, --nz` - Grid dimensions (default: 64 for each)
- `-s, --steps NUM` - Number of simulation steps (default: 200)
- `-o, --output-interval N` - Output every N steps (default: 10)
- `--smoke-steps N` - Stop smoke generation after N steps (default: same as --steps)
- `--dt` - Time step size (default: 0.1)
- `--dx` - Grid spacing (default: 1.0)

## Simulation Parameters

Most parameters can be set via **command line arguments** (see Running section above).

Physical parameters can be modified in `src/FluidSolver.cpp` constructor:

- **Viscosity**: Controls fluid thickness (default: 0.0001)
- **Diffusion**: Controls density/temperature diffusion (default: 0.0001)
- **Buoyancy**: Temperature-driven upward force (default: 0.5)

## Visualization in ParaView

1. Open ParaView
2. File → Open → Select `output_*.vti` files (all at once)
3. Click "Apply" to load the data
4. In the toolbar, select which field to visualize:
   - **density**: Shows smoke concentration (default active scalar)
   - **temperature**: Shows heat distribution
   - **velocity_magnitude**: Shows flow speed
   - **velocity**: Vector field (default active vector - use Glyph or Stream Tracer filter)
   - **obstacle**: Shows obstacle geometry

### Recommended Filters

- **Volume rendering**: For density field
- **Contour**: For isosurfaces of density/temperature
- **Stream Tracer**: For velocity field visualization
- **Slice**: For cross-sectional views

## Algorithm Overview

The simulation uses a stable fluids approach:

1. **Advection**: Semi-Lagrangian backtrace
2. **Diffusion**: Jacobi iteration solver
3. **Projection**: Helmholtz-Hodge decomposition for incompressibility
4. **Buoyancy**: Temperature-driven vertical force
5. **Boundary conditions**: No-slip at walls and obstacles

## Performance

On a typical modern CPU with 8 cores:
- 64³ grid: ~10-20ms per timestep
- 128³ grid: ~80-150ms per timestep

OpenMP automatically uses all available CPU cores.

## File Structure

```
.
├── CMakeLists.txt          # Build configuration
├── README.md              # This file
└── src/
    ├── main.cpp           # Main simulation loop
    ├── FluidSolver.h      # Solver interface
    ├── FluidSolver.cpp    # Solver implementation
    ├── VTKWriter.h        # VTK output interface
    └── VTKWriter.cpp      # VTK output implementation
```

## License

Free to use and modify for any purpose.
