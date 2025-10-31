#include "FluidSolver.h"
#ifdef _OPENMP
#include <omp.h>
#endif
#include <cstring>

FluidSolver::FluidSolver(int nx, int ny, int nz, double dx, double dt)
    : nx(nx), ny(ny), nz(nz), dx(dx), dt(dt),
      viscosity(0.15),               // Kinematic viscosity (momentum diffusion)
      thermal_diffusivity(0.25),     // Thermal diffusivity (Pr = nu/alpha ~ 0.6 for air)
      mass_diffusivity(0.5),         // Mass diffusivity for smoke (high for fast spreading)
      gravity(9.81),                 // Gravitational acceleration (m/s²)
      thermal_expansion(0.00333),    // Thermal expansion coefficient for air (1/300K)
      ambient_temperature(0.0),
      inlet_velocity_u(5.0),         // Default inlet velocity in x-direction
      inlet_velocity_v(0.0),
      inlet_velocity_w(0.0) {
    
    int size = nx * ny * nz;
    
    u.resize(size, 0.0);
    v.resize(size, 0.0);
    w.resize(size, 0.0);
    u_prev.resize(size, 0.0);
    v_prev.resize(size, 0.0);
    w_prev.resize(size, 0.0);
    
    density.resize(size, 0.0);
    density_prev.resize(size, 0.0);
    
    temperature.resize(size, ambient_temperature);
    temperature_prev.resize(size, ambient_temperature);
    
    pressure.resize(size, 0.0);
    obstacles.resize(size, false);
}

int FluidSolver::idx(int i, int j, int k) const {
    return i + nx * (j + ny * k);
}

bool FluidSolver::isValid(int i, int j, int k) const {
    return i >= 0 && i < nx && j >= 0 && j < ny && k >= 0 && k < nz;
}

void FluidSolver::addSource(int x, int y, int z, double dens, double temp) {
    if (isValid(x, y, z)) {
        int index = idx(x, y, z);
        density[index] += dens;
        temperature[index] += temp;
    }
}

void FluidSolver::setObstacle(int x, int y, int z, bool is_obstacle) {
    if (isValid(x, y, z)) {
        obstacles[idx(x, y, z)] = is_obstacle;
    }
}

void FluidSolver::setInletVelocity(double inlet_u, double inlet_v, double inlet_w) {
    inlet_velocity_u = inlet_u;
    inlet_velocity_v = inlet_v;
    inlet_velocity_w = inlet_w;
}

void FluidSolver::step() {
    // Save previous state
    u_prev = u;
    v_prev = v;
    w_prev = w;
    density_prev = density;
    temperature_prev = temperature;
    
    // Apply buoyancy force from temperature
    applyBuoyancy();
    
    // Diffuse velocity
    diffuse(u, u_prev, viscosity);
    diffuse(v, v_prev, viscosity);
    diffuse(w, w_prev, viscosity);
    
    // Project to make velocity field divergence-free
    project();
    
    u_prev = u;
    v_prev = v;
    w_prev = w;
    
    // Advect velocity
    advect(u, u_prev);
    advect(v, v_prev);
    advect(w, w_prev);
    
    // Apply drag near obstacles to enhance vortex formation
    applyObstacleDrag();
    
    // Project again
    project();
    
    // Diffuse and advect density (with mass diffusivity)
    diffuse(density, density_prev, mass_diffusivity);
    density_prev = density;
    advect(density, density_prev);
    
    // Diffuse and advect temperature (with thermal diffusivity)
    diffuse(temperature, temperature_prev, thermal_diffusivity);
    temperature_prev = temperature;
    advect(temperature, temperature_prev);
    
    // Apply boundary conditions
    applyBoundaryConditions();
}

void FluidSolver::applyBuoyancy() {
    // Boussinesq approximation: F_buoyancy = g * β * (T - T₀)
    // Where: g = gravity, β = thermal expansion coefficient
    // This assumes density variations are small except in buoyancy term
    #pragma omp parallel for collapse(3)
    for (int k = 1; k < nz - 1; ++k) {
        for (int j = 1; j < ny - 1; ++j) {
            for (int i = 1; i < nx - 1; ++i) {
                int index = idx(i, j, k);
                if (!obstacles[index]) {
                    // Buoyancy force acts upward (in j direction - y-axis is vertical)
                    // Note: j-direction (y-axis) is vertical in this simulation
                    double temp_diff = temperature[index] - ambient_temperature;
                    v[index] += dt * gravity * thermal_expansion * temp_diff;
                }
            }
        }
    }
}

void FluidSolver::applyObstacleDrag() {
    // Apply enhanced drag near obstacle boundaries to promote vortex shedding
    // This creates stronger velocity gradients and shear layers
    double drag_coefficient = 2.5;  // Enhanced drag factor
    
    #pragma omp parallel for collapse(3)
    for (int k = 1; k < nz - 1; ++k) {
        for (int j = 1; j < ny - 1; ++j) {
            for (int i = 1; i < nx - 1; ++i) {
                int index = idx(i, j, k);
                
                // Only apply to fluid cells adjacent to obstacles
                if (!obstacles[index]) {
                    // Check if any neighbor is an obstacle
                    bool near_obstacle = false;
                    near_obstacle |= obstacles[idx(i-1, j, k)];
                    near_obstacle |= obstacles[idx(i+1, j, k)];
                    near_obstacle |= obstacles[idx(i, j-1, k)];
                    near_obstacle |= obstacles[idx(i, j+1, k)];
                    near_obstacle |= obstacles[idx(i, j, k-1)];
                    near_obstacle |= obstacles[idx(i, j, k+1)];
                    
                    if (near_obstacle) {
                        // Apply drag force proportional to velocity
                        // This simulates enhanced friction at obstacle boundaries
                        double vel_mag = std::sqrt(u[index]*u[index] + 
                                                   v[index]*v[index] + 
                                                   w[index]*w[index]);
                        
                        if (vel_mag > 0.01) {
                            double drag_factor = 1.0 - drag_coefficient * dt * vel_mag;
                            drag_factor = std::max(0.3, drag_factor);  // Don't reduce below 30%
                            
                            u[index] *= drag_factor;
                            v[index] *= drag_factor;
                            w[index] *= drag_factor;
                        }
                    }
                }
            }
        }
    }
}

void FluidSolver::advect(std::vector<double>& field, const std::vector<double>& field_prev) {
    double dt0 = dt / dx;
    
    #pragma omp parallel for collapse(3)
    for (int k = 1; k < nz - 1; ++k) {
        for (int j = 1; j < ny - 1; ++j) {
            for (int i = 1; i < nx - 1; ++i) {
                int index = idx(i, j, k);
                
                if (obstacles[index]) {
                    field[index] = 0.0;
                    continue;
                }
                
                // Backtrace
                double x = i - dt0 * u_prev[index];
                double y = j - dt0 * v_prev[index];
                double z = k - dt0 * w_prev[index];
                
                // Clamp to grid
                x = std::max(0.5, std::min(x, nx - 1.5));
                y = std::max(0.5, std::min(y, ny - 1.5));
                z = std::max(0.5, std::min(z, nz - 1.5));
                
                // Trilinear interpolation
                int i0 = (int)x, i1 = i0 + 1;
                int j0 = (int)y, j1 = j0 + 1;
                int k0 = (int)z, k1 = k0 + 1;
                
                double sx1 = x - i0, sx0 = 1.0 - sx1;
                double sy1 = y - j0, sy0 = 1.0 - sy1;
                double sz1 = z - k0, sz0 = 1.0 - sz1;
                
                field[index] = 
                    sz0 * (sy0 * (sx0 * field_prev[idx(i0, j0, k0)] + 
                                  sx1 * field_prev[idx(i1, j0, k0)]) +
                           sy1 * (sx0 * field_prev[idx(i0, j1, k0)] + 
                                  sx1 * field_prev[idx(i1, j1, k0)])) +
                    sz1 * (sy0 * (sx0 * field_prev[idx(i0, j0, k1)] + 
                                  sx1 * field_prev[idx(i1, j0, k1)]) +
                           sy1 * (sx0 * field_prev[idx(i0, j1, k1)] + 
                                  sx1 * field_prev[idx(i1, j1, k1)]));
            }
        }
    }
}

void FluidSolver::diffuse(std::vector<double>& field, const std::vector<double>& field_prev, 
                         double diff_coef) {
    // Physically correct diffusion: coefficient scaled by dt/(dx²)
    // For 3D heat equation: ∂T/∂t = α∇²T
    double a = dt * diff_coef / (dx * dx);
    jacobiIteration(field, field_prev, a, 1.0 + 6.0 * a, 20);
}

void FluidSolver::jacobiIteration(std::vector<double>& x, const std::vector<double>& b,
                                 double alpha, double beta, int iterations) {
    std::vector<double> x_new = x;
    
    for (int iter = 0; iter < iterations; ++iter) {
        #pragma omp parallel for collapse(3)
        for (int k = 1; k < nz - 1; ++k) {
            for (int j = 1; j < ny - 1; ++j) {
                for (int i = 1; i < nx - 1; ++i) {
                    int index = idx(i, j, k);
                    
                    if (obstacles[index]) {
                        x_new[index] = 0.0;
                        continue;
                    }
                    
                    double sum = x[idx(i-1, j, k)] + x[idx(i+1, j, k)] +
                                x[idx(i, j-1, k)] + x[idx(i, j+1, k)] +
                                x[idx(i, j, k-1)] + x[idx(i, j, k+1)];
                    
                    x_new[index] = (b[index] + alpha * sum) / beta;
                }
            }
        }
        x.swap(x_new);
    }
}

void FluidSolver::project() {
    std::vector<double> div(nx * ny * nz, 0.0);
    
    // Compute divergence
    #pragma omp parallel for collapse(3)
    for (int k = 1; k < nz - 1; ++k) {
        for (int j = 1; j < ny - 1; ++j) {
            for (int i = 1; i < nx - 1; ++i) {
                int index = idx(i, j, k);
                
                if (obstacles[index]) continue;
                
                div[index] = -0.5 * dx * (
                    u[idx(i+1, j, k)] - u[idx(i-1, j, k)] +
                    v[idx(i, j+1, k)] - v[idx(i, j-1, k)] +
                    w[idx(i, j, k+1)] - w[idx(i, j, k-1)]
                );
            }
        }
    }
    
    // Solve for pressure
    std::fill(pressure.begin(), pressure.end(), 0.0);
    jacobiIteration(pressure, div, 1.0, 6.0, 40);
    
    // Subtract pressure gradient
    #pragma omp parallel for collapse(3)
    for (int k = 1; k < nz - 1; ++k) {
        for (int j = 1; j < ny - 1; ++j) {
            for (int i = 1; i < nx - 1; ++i) {
                int index = idx(i, j, k);
                
                if (obstacles[index]) {
                    u[index] = 0.0;
                    v[index] = 0.0;
                    w[index] = 0.0;
                    continue;
                }
                
                u[index] -= 0.5 * (pressure[idx(i+1, j, k)] - pressure[idx(i-1, j, k)]) / dx;
                v[index] -= 0.5 * (pressure[idx(i, j+1, k)] - pressure[idx(i, j-1, k)]) / dx;
                w[index] -= 0.5 * (pressure[idx(i, j, k+1)] - pressure[idx(i, j, k-1)]) / dx;
            }
        }
    }
}

void FluidSolver::applyBoundaryConditions() {
    // Apply boundary conditions for velocity and obstacles
    #pragma omp parallel for collapse(3)
    for (int k = 0; k < nz; ++k) {
        for (int j = 0; j < ny; ++j) {
            for (int i = 0; i < nx; ++i) {
                int index = idx(i, j, k);
                
                // Zero velocity at obstacles
                if (obstacles[index]) {
                    u[index] = 0.0;
                    v[index] = 0.0;
                    w[index] = 0.0;
                    density[index] = 0.0;  // No smoke inside obstacles
                    temperature[index] = 0.0;
                }
            }
        }
    }
    
    // Apply reflective boundary conditions (Neumann - zero gradient)
    // This keeps smoke contained within the volume
    
    // X boundaries - Wind Tunnel: Inlet (left) and Outlet (right)
    #pragma omp parallel for collapse(2)
    for (int k = 0; k < nz; ++k) {
        for (int j = 0; j < ny; ++j) {
            // INLET at left boundary (i=0) - constant velocity inflow
            u[idx(0, j, k)] = inlet_velocity_u;
            v[idx(0, j, k)] = inlet_velocity_v;
            w[idx(0, j, k)] = inlet_velocity_w;
            density[idx(0, j, k)] = 0.0;  // No smoke at inlet initially
            temperature[idx(0, j, k)] = ambient_temperature;
            
            // OUTLET at right boundary (i=nx-1) - zero gradient outflow
            u[idx(nx-1, j, k)] = u[idx(nx-2, j, k)];
            v[idx(nx-1, j, k)] = v[idx(nx-2, j, k)];
            w[idx(nx-1, j, k)] = w[idx(nx-2, j, k)];
            density[idx(nx-1, j, k)] = density[idx(nx-2, j, k)];  // Smoke flows out
            temperature[idx(nx-1, j, k)] = temperature[idx(nx-2, j, k)];
        }
    }
    
    // Y boundaries
    #pragma omp parallel for collapse(2)
    for (int k = 0; k < nz; ++k) {
        for (int i = 0; i < nx; ++i) {
            // Bottom boundary (j=0)
            u[idx(i, 0, k)] = u[idx(i, 1, k)];
            v[idx(i, 0, k)] = 0.0;
            w[idx(i, 0, k)] = w[idx(i, 1, k)];
            density[idx(i, 0, k)] = density[idx(i, 1, k)];
            temperature[idx(i, 0, k)] = temperature[idx(i, 1, k)];
            
            // Top boundary (j=ny-1)
            u[idx(i, ny-1, k)] = u[idx(i, ny-2, k)];
            v[idx(i, ny-1, k)] = 0.0;
            w[idx(i, ny-1, k)] = w[idx(i, ny-2, k)];
            density[idx(i, ny-1, k)] = density[idx(i, ny-2, k)];
            temperature[idx(i, ny-1, k)] = temperature[idx(i, ny-2, k)];
        }
    }
    
    // Z boundaries
    #pragma omp parallel for collapse(2)
    for (int j = 0; j < ny; ++j) {
        for (int i = 0; i < nx; ++i) {
            // Front boundary (k=0)
            u[idx(i, j, 0)] = u[idx(i, j, 1)];
            v[idx(i, j, 0)] = v[idx(i, j, 1)];
            w[idx(i, j, 0)] = 0.0;
            density[idx(i, j, 0)] = density[idx(i, j, 1)];
            temperature[idx(i, j, 0)] = temperature[idx(i, j, 1)];
            
            // Back boundary (k=nz-1)
            u[idx(i, j, nz-1)] = u[idx(i, j, nz-2)];
            v[idx(i, j, nz-1)] = v[idx(i, j, nz-2)];
            w[idx(i, j, nz-1)] = 0.0;
            density[idx(i, j, nz-1)] = density[idx(i, j, nz-2)];
            temperature[idx(i, j, nz-1)] = temperature[idx(i, j, nz-2)];
        }
    }
}
