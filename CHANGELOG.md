# Changelog

## Version 1.1 - Educational Wind Tunnel Simulation (2025-10-14)

### Overview

Complete educational wind tunnel simulation system featuring enhanced vortex visualization, comprehensive documentation, and step-by-step ParaView tutorial. This version consolidates all features into a polished teaching tool for fluid mechanics and CFD visualization.

---

### Major Features

#### Wind Tunnel Mode
- Inlet/outlet boundary conditions for constant horizontal flow
- Configurable inlet velocity via `setInletVelocity(u, v, w)` (default: 5.0 m/s)
- Zero-gradient outlet for realistic flow exit
- Reynolds number ~533 (transitional regime, ideal for vortex visualization)
- Proper boundary conditions: inlet (left), outlet (right), no-slip walls (other faces)

#### Obstacles
- **Sphere**: 3D obstacle at domain center (radius 8)
  - Demonstrates 3D flow structures
  - Smooth flow separation
  - Complex wake patterns
- **Vertical Cylinder**: 2D-like obstacle (radius 5, full height)
  - Classic von Kármán vortex shedding
  - Regular alternating vortex pattern
  - Consistent flow along height
- Both obstacles positioned for optimal smoke stream interaction

#### Enhanced Vortex Shedding
- Added `applyObstacleDrag()` method with enhanced drag coefficient (2.5)
- Creates strong velocity gradients at obstacle boundaries
- Promotes shear layer formation and flow separation
- Results in pronounced, visible von Kármán vortex streets
- Drag applied only to fluid cells adjacent to obstacles
- Minimum 30% velocity retention to maintain stability

#### Targeted Smoke Streams
Three strategically positioned smoke streams for optimal flow visualization:
- **Stream 1**: Cylinder-targeting (density 0.06, strongest)
  - Position: aligned with cylinder obstacle
  - Shows sharp separation and regular vortex shedding
- **Stream 2**: Sphere-targeting (density 0.04, medium)
  - Position: domain center
  - Shows smooth 3D flow patterns
- **Stream 3**: Reference stream (density 0.03, light)
  - Position: upper region, no obstacles
  - Baseline for comparison
- Smoke injection controlled by `--smoke-steps` parameter

---

### Comprehensive Documentation

#### USER_GUIDE.md (Educational Focus)
**Content:**
- Quick start guide with example commands
- Simulation parameter explanations
- Wind tunnel concept and physics principles
- "What to Look For" section with 7 key phenomena:
  1. Cylinder obstacle smoke impact (most dramatic)
  2. Sphere obstacle smooth separation
  3. Vortex shedding cycle (von Kármán pattern)
  4. Boundary layer drag effects
  5. Wake turbulence evolution
  6. Comparison between obstacles
  7. Reference stream behavior
- Simplified physics explanations (no complex equations)
- Reynolds number concept with real-world examples
- Common observations explained (wake formation, vortex patterns)
- Tips for teaching fluid mechanics (4 progressive lessons)
- Troubleshooting guide
- File format explanations

#### PARAVIEW_TUTORIAL.md (Complete Visualization Guide)
**5 Progressive Tutorials:**
1. **Tutorial 1: Visualizing Smoke**
   - Loading data files
   - Coloring by density field
   - Choosing colormaps
   - Playing animations
   - Navigation controls

2. **Tutorial 2: Adding Obstacles**
   - Using Contour filter
   - Creating iso-surfaces
   - Adjusting appearance and opacity
   - Showing multiple datasets

3. **Tutorial 3: Creating Streamlines**
   - Stream Tracer filter
   - Configuring seed points
   - Integration parameters
   - Coloring by velocity
   - Multiple seed locations

4. **Tutorial 4: Combined Visualization**
   - Layering smoke, obstacles, and streamlines
   - Transparency management
   - Camera positioning
   - Animation playback

5. **Tutorial 5: Advanced Techniques**
   - Cross-section slices
   - Volume rendering
   - Velocity glyphs (arrows)
   - Vorticity calculation
   - Image/animation export
   - Split view comparisons

**Additional Features:**
- Detailed "What to Observe" section
- Troubleshooting with specific solutions
- Keyboard shortcuts and quick reference
- Common filters table
- Step-by-step with exact values

---

### Technical Implementation

#### Drag Mechanism
```cpp
void FluidSolver::applyObstacleDrag() {
    double drag_coefficient = 2.5;  // Enhanced drag
    
    // For each fluid cell adjacent to obstacles:
    drag_factor = 1.0 - drag_coefficient * dt * velocity_magnitude;
    drag_factor = max(0.3, drag_factor);  // Minimum 30% retention
    
    u[index] *= drag_factor;
    v[index] *= drag_factor;
    w[index] *= drag_factor;
}
```

#### Cylinder Geometry
```cpp
// Vertical cylinder along y-axis, circular cross-section in XZ plane
int cyl_cx = nx/4 + 4;  // Center x
int cyl_cz = nz/4 + 4;  // Center z
int cyl_radius = 5;

for (int j = 0; j < ny; ++j) {  // Full height
    for (int k = 0; k < nz; ++k) {
        for (int i = 0; i < nx; ++i) {
            double dist = sqrt((i-cyl_cx)² + (k-cyl_cz)²);
            if (dist < cyl_radius) {
                setObstacle(i, j, k, true);
            }
        }
    }
}
```

#### Smoke Stream Positioning
- **Cylinder stream**: (3-6, ny/4 to ny/4+8, nz/4 to nz/4+8) - density 0.06
- **Sphere stream**: (3-6, ny/2-4 to ny/2+4, nz/2-4 to nz/2+4) - density 0.04
- **Reference stream**: (3-6, 3*ny/4 to 3*ny/4+3, nz/2-2 to nz/2+2) - density 0.03

---

### Physics Accuracy

#### Proper Boussinesq Approximation
- Fixed buoyancy: `F = g × β × ΔT` (removed incorrect density multiplication)
- Physical constants: `g = 9.81 m/s²`, `β = 0.00333 K⁻¹`
- Buoyancy acts on vertical velocity component (y-direction)

#### Correct Diffusion Scaling
- Fixed from incorrect `dt × D × nx×ny×nz` to physical `dt × D / (dx²)`
- Implements proper discrete diffusion: `∂φ/∂t = D∇²φ`

#### Separate Physical Diffusivities
- Kinematic viscosity: `ν = 0.001 m²/s` (momentum)
- Thermal diffusivity: `α = 0.0015 m²/s` (heat)
- Mass diffusivity: `D = 0.0012 m²/s` (smoke)
- Prandtl number: `Pr ≈ 0.67` (matches air)

---

### Educational Value

**Physics Concepts Demonstrated:**
- Von Kármán vortex streets behind cylinder
- Flow separation and reattachment
- Boundary layer effects and enhanced drag
- Wake turbulence evolution
- Cylinder (2D-like) vs sphere (3D) flow comparison
- Reynolds number effects on flow regime
- Advection vs diffusion mechanisms

**Teaching Applications:**
- Undergraduate fluid mechanics courses
- Flow visualization demonstrations
- Aerodynamics and drag concepts
- Computational Fluid Dynamics (CFD) introduction
- ParaView visualization training
- Scientific visualization education

**Target Audience:**
- Students without physics background (simplified explanations)
- Fluid mechanics instructors
- CFD beginners
- Visualization software learners

---

### Command Line Interface

All simulation parameters configurable without recompilation:

```bash
# Options
-h, --help              Show help message
-n, --grid SIZE         Cubic grid size (default: 64)
--nx, --ny, --nz        Individual grid dimensions
-s, --steps NUM         Total simulation steps (default: 1000)
-o, --output-interval N Save output every N steps (default: 10)
--smoke-steps N         Stop smoke after N steps (default: all)
--dt TIMESTEP           Time step size (default: 0.1)
--dx SPACING            Grid spacing (default: 1.0)

# Examples
./fluid_sim -n 64 -s 1000 --smoke-steps 500 -o 10  # Default run
./fluid_sim -n 128 -s 2000 -o 20                   # High resolution
./fluid_sim -n 32 -s 500                           # Quick test
```

---

### Files Added/Modified

**New Files:**
- `PARAVIEW_TUTORIAL.md` - Complete step-by-step ParaView guide (850+ lines)
- `USER_GUIDE.md` - Educational user guide with simplified physics (600+ lines)
- `CHANGELOG.md` - This comprehensive changelog

**Modified Files:**
- `src/main.cpp`:
  - Replaced box with vertical cylinder obstacle
  - Added three targeted smoke streams
  - Positioned streams to hit obstacles directly
- `src/FluidSolver.h`:
  - Added `applyObstacleDrag()` method declaration
  - Added inlet velocity member variables
- `src/FluidSolver.cpp`:
  - Implemented obstacle drag mechanism
  - Added wind tunnel boundary conditions
  - Fixed Boussinesq buoyancy physics
  - Corrected diffusion scaling
  - Added separate diffusivities for heat/mass/momentum

**Documentation Structure:**
```
README.md              - Installation and quick start
USER_GUIDE.md          - Educational guide and physics explanations
PARAVIEW_TUTORIAL.md   - Complete visualization tutorial
CHANGELOG.md           - Version history
VTK_INTEGRATION.md     - Technical VTK details
```

---

### Performance

**Typical Runtime (64³ grid, 1000 steps):**
- Wall time: ~50 seconds
- Performance: ~47-60 ms per timestep
- OpenMP threads: 8 (automatic)
- Output files: ~100 VTI files
- Total output size: ~200-300 MB

**Scaling:**
- 32³ grid: ~8× faster (6-8 seconds)
- 128³ grid: ~8× slower (6-8 minutes)

---

### Use Cases

1. **Fluid Mechanics Education**
   - Demonstrate vortex shedding visually
   - Teach flow separation concepts
   - Explain Reynolds number effects
   - Show difference between 2D and 3D flows

2. **CFD Training**
   - Introduction to flow solvers
   - Boundary condition concepts
   - Grid resolution effects
   - Numerical stability issues

3. **Visualization Training**
   - ParaView tutorial for beginners
   - Scientific data visualization
   - Multi-field dataset handling
   - Animation creation

4. **Research Starting Point**
   - Aerodynamic testing framework
   - Custom obstacle geometries
   - Parameter studies
   - Flow control experiments

---

### Breaking Changes from 1.0

- Boundary conditions changed from closed box to wind tunnel (inlet/outlet)
- Buoyancy strength differs (now physically based)
- Diffusion rates corrected (may affect appearance)
- Smoke behavior changed (now exits through outlet instead of accumulating)

---

### Known Limitations

- Teaching code - not production CFD solver
- Low Reynolds number regime only (Re < 1000)
- Fixed rectangular domain
- Simple obstacle shapes (sphere, cylinder)
- No turbulence modeling (direct simulation only)

---

### Future Enhancements (Potential)

- Interactive obstacle placement
- Real-time visualization
- Additional obstacle shapes (airfoil, multiple objects)
- Parameter optimization tools
- Quantitative drag/lift calculations
- Time-averaged flow statistics

---

## Version 1.0 - Initial Release (2025-10-14)

### Features
- 3D Navier-Stokes solver with semi-Lagrangian advection
- Temperature-based buoyancy for smoke behavior
- Obstacle support (sphere obstacle)
- OpenMP parallelization
- VTK library integration for output
- Binary XML format (`.vti`) for efficient storage
- ParaView-ready output

### Technical Details
- Grid: 64³ default
- Time step: 0.1
- Output: Every 10 steps
- File format: VTK XML ImageData (`.vti`)
- Parallelization: OpenMP with automatic thread detection
- Dependencies: VTK, OpenMP

### Physical Parameters
- Viscosity: 0.0001
- Diffusion: 0.0001
- Buoyancy: 0.5 (arbitrary)
- Ambient temperature: 0.0

### Output Fields
1. **density** - Smoke concentration
2. **temperature** - Heat distribution
3. **obstacle** - Obstacle mask
4. **velocity_magnitude** - Flow speed
5. **velocity** - Velocity vector field
