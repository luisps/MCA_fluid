#pragma once

#include <vector>
#include <cmath>
#include <algorithm>

class FluidSolver {
public:
    FluidSolver(int nx, int ny, int nz, double dx, double dt);
    
    // Main simulation step
    void step();
    
    // Add smoke source
    void addSource(int x, int y, int z, double density, double temperature);
    
    // Set obstacle
    void setObstacle(int x, int y, int z, bool is_obstacle);
    
    // Set inlet velocity (for wind tunnel)
    void setInletVelocity(double inlet_u, double inlet_v, double inlet_w);
    
    // Getters for visualization
    const std::vector<double>& getDensity() const { return density; }
    const std::vector<double>& getTemperature() const { return temperature; }
    const std::vector<bool>& getObstacles() const { return obstacles; }
    const std::vector<double>& getVelocityU() const { return u; }
    const std::vector<double>& getVelocityV() const { return v; }
    const std::vector<double>& getVelocityW() const { return w; }
    
    int getNx() const { return nx; }
    int getNy() const { return ny; }
    int getNz() const { return nz; }
    double getDx() const { return dx; }
    
private:
    // Grid dimensions
    int nx, ny, nz;
    double dx, dt;
    
    // Physical parameters (dimensionless)
    double viscosity;           // Kinematic viscosity (momentum diffusivity)
    double thermal_diffusivity; // Heat diffusivity (separate from mass)
    double mass_diffusivity;    // Mass/smoke diffusivity
    double gravity;             // Gravitational acceleration
    double thermal_expansion;   // Thermal expansion coefficient β
    double ambient_temperature; // Reference temperature
    
    // Wind tunnel parameters
    double inlet_velocity_u;
    double inlet_velocity_v;
    double inlet_velocity_w;
    
    // Grid data
    std::vector<double> u, v, w;           // velocity components
    std::vector<double> u_prev, v_prev, w_prev;
    std::vector<double> density, density_prev;
    std::vector<double> temperature, temperature_prev;
    std::vector<double> pressure;
    std::vector<bool> obstacles;
    
    // Helper functions
    int idx(int i, int j, int k) const;
    bool isValid(int i, int j, int k) const;
    
    // Simulation steps
    void advect(std::vector<double>& field, const std::vector<double>& field_prev);
    void diffuse(std::vector<double>& field, const std::vector<double>& field_prev, double diff_coef);
    void project();
    void applyBuoyancy();
    void applyObstacleDrag();
    void applyBoundaryConditions();
    
    // Linear solver
    void jacobiIteration(std::vector<double>& x, const std::vector<double>& b, 
                        double alpha, double beta, int iterations);
};
