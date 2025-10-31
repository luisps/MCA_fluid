# Quick Start Guide

## Build and Run

```bash
# Install VTK (required)
brew install vtk

# Build the project
mkdir build && cd build
cmake ..
make

# Run with default settings
./fluid_sim

# Or customize parameters
./fluid_sim -n 128 -s 500 -o 20
```

The default simulation will:
- Generate a 64×64×64 grid
- Simulate 200 timesteps
- Output VTK XML files every 10 steps
- Create files: `output_0000.vti`, `output_0010.vti`, ..., `output_0190.vti`

### Quick Parameter Reference

```bash
./fluid_sim -n 32 -s 50           # Fast test: small grid, fewer steps
./fluid_sim -n 128 -s 1000        # High detail: large grid, many steps
./fluid_sim --dt 0.05 -o 5        # Smaller timestep, more frequent output
./fluid_sim -s 500 --smoke-steps 100  # Smoke for 100 steps, watch dissipate
./fluid_sim --help                # See all options
```

## Install OpenMP (Recommended for parallel acceleration)

```bash
# On macOS
brew install libomp

# Then rebuild
cd build
rm -rf *
cmake ..
make
```

**Good news for macOS users:** The CMake configuration automatically detects and uses Homebrew's libomp! Just install it and rebuild.

With OpenMP, the simulation will run **4-8x faster** on multi-core CPUs. The difference is especially noticeable on larger grids (128³ or bigger).

## Visualize in ParaView

1. Open ParaView
2. File → Open → Select all `output_*.vti` files
3. Click "Apply"
4. In the dropdown, select:
   - **density** - to see smoke (default)
   - **temperature** - to see heat
   - **velocity** - to see flow (use Stream Tracer filter)
   - **obstacle** - to see obstacles

**Note:** VTK XML format (`.vti`) is more efficient and feature-rich than legacy `.vtk` format.

### Quick Visualization Tips

**For smoke visualization:**
- Select "density" field
- Apply → Filters → Contour (for isosurfaces)
- Or: Representation → Volume (for volume rendering)

**For velocity visualization:**
- Select "velocity" field
- Filters → Stream Tracer with High Resolution Line Source
- Adjust "Maximum Streamline Length"

**For temperature:**
- Select "temperature" field
- Use color map to visualize heat distribution

## Customize Simulation

### Via Command Line (Easy)

```bash
# Change grid size
./fluid_sim -n 128              # 128³ grid
./fluid_sim --nx 128 --ny 64 --nz 64  # Non-cubic grid

# Change simulation length
./fluid_sim -s 500              # 500 timesteps
./fluid_sim -o 5                # Output every 5 steps

# Change physics
./fluid_sim --dt 0.05           # Smaller time step (more accurate)
./fluid_sim --dx 0.5            # Smaller grid spacing
```

### Via Source Code (Advanced)

Edit `src/FluidSolver.cpp` constructor:

```cpp
viscosity(0.0001),    // Higher = more viscous fluid
diffusion(0.0001),    // Higher = more smoke spreading
buoyancy(0.5),        // Higher = stronger upward force
```

## Troubleshooting

**Simulation is too slow:**
- Reduce grid size (e.g., 32×32×32)
- Reduce num_steps
- Install OpenMP for parallelization

**Smoke disappears quickly:**
- Reduce boundary dissipation in `FluidSolver.cpp` line 286: `density[index] *= 0.999;`
- Increase smoke source strength in `main.cpp` line 70: `solver.addSource(i, j, k, 1.0, 10.0);`

**Want more obstacles:**
Add in `main.cpp` after line 54:
```cpp
// Add a wall
for (int k = 0; k < nz; ++k) {
    for (int j = 0; j < ny/2; ++j) {
        solver.setObstacle(nx/2, j, k, true);
    }
}
```
