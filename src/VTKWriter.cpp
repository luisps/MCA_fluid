#include "VTKWriter.h"
#include <vtkImageData.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkSmartPointer.h>
#include <cmath>

void VTKWriter::writeVTK(const std::string& filename,
                        int nx, int ny, int nz,
                        double dx,
                        const std::vector<double>& density,
                        const std::vector<double>& temperature,
                        const std::vector<double>& u,
                        const std::vector<double>& v,
                        const std::vector<double>& w,
                        const std::vector<bool>& obstacles) {
    
    // Create image data (structured grid)
    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    imageData->SetDimensions(nx, ny, nz);
    imageData->SetSpacing(dx, dx, dx);
    imageData->SetOrigin(0.0, 0.0, 0.0);
    
    int numPoints = nx * ny * nz;
    
    // Create density array
    vtkSmartPointer<vtkFloatArray> densityArray = vtkSmartPointer<vtkFloatArray>::New();
    densityArray->SetName("density");
    densityArray->SetNumberOfComponents(1);
    densityArray->SetNumberOfTuples(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        densityArray->SetValue(i, static_cast<float>(density[i]));
    }
    imageData->GetPointData()->AddArray(densityArray);
    
    // Create temperature array
    vtkSmartPointer<vtkFloatArray> temperatureArray = vtkSmartPointer<vtkFloatArray>::New();
    temperatureArray->SetName("temperature");
    temperatureArray->SetNumberOfComponents(1);
    temperatureArray->SetNumberOfTuples(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        temperatureArray->SetValue(i, static_cast<float>(temperature[i]));
    }
    imageData->GetPointData()->AddArray(temperatureArray);
    
    // Create obstacle array
    vtkSmartPointer<vtkFloatArray> obstacleArray = vtkSmartPointer<vtkFloatArray>::New();
    obstacleArray->SetName("obstacle");
    obstacleArray->SetNumberOfComponents(1);
    obstacleArray->SetNumberOfTuples(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        obstacleArray->SetValue(i, obstacles[i] ? 1.0f : 0.0f);
    }
    imageData->GetPointData()->AddArray(obstacleArray);
    
    // Create velocity magnitude array
    vtkSmartPointer<vtkFloatArray> velocityMagArray = vtkSmartPointer<vtkFloatArray>::New();
    velocityMagArray->SetName("velocity_magnitude");
    velocityMagArray->SetNumberOfComponents(1);
    velocityMagArray->SetNumberOfTuples(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        float mag = std::sqrt(u[i]*u[i] + v[i]*v[i] + w[i]*w[i]);
        velocityMagArray->SetValue(i, mag);
    }
    imageData->GetPointData()->AddArray(velocityMagArray);
    
    // Create velocity vector array
    vtkSmartPointer<vtkFloatArray> velocityArray = vtkSmartPointer<vtkFloatArray>::New();
    velocityArray->SetName("velocity");
    velocityArray->SetNumberOfComponents(3);
    velocityArray->SetNumberOfTuples(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        velocityArray->SetTuple3(i, 
                                static_cast<float>(u[i]), 
                                static_cast<float>(v[i]), 
                                static_cast<float>(w[i]));
    }
    imageData->GetPointData()->AddArray(velocityArray);
    
    // Set density as the default scalar for visualization
    imageData->GetPointData()->SetActiveScalars("density");
    imageData->GetPointData()->SetActiveVectors("velocity");
    
    // Write to XML format (.vti)
    vtkSmartPointer<vtkXMLImageDataWriter> writer = vtkSmartPointer<vtkXMLImageDataWriter>::New();
    writer->SetFileName(filename.c_str());
    writer->SetInputData(imageData);
    writer->SetDataModeToBinary();  // Use binary for smaller files
    writer->Write();
}
