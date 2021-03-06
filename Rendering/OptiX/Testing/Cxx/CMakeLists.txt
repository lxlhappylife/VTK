
find_package( OptiX REQUIRED )
# TODO: Use CMake's built-in Cuda support instead.
find_package( CUDA 4.0 REQUIRED )

include_directories(
    "${OptiX_INCLUDE}"
    "${CUDA_INCLUDE_DIRS}"
    )

vtk_add_test_cxx(vtkRenderingOptiXCxxTests tests
  TestOptiXCompositePolyDataMapper2.cxx
  TestOptiXDynamicObject.cxx
  TestOptiXDynamicScene.cxx
  TestOptiXImplicits.cxx
  TestOptiXLayers.cxx
  TestOptiXLights.cxx
  TestOptiXMultiBlock.cx
  TestOptiXMultiBlockPartialArrayFieldData.cxx
  TestOptiXPass.cxx
  TestOptiXRenderMesh.cxx
  TestOptiXScalarBar.cx
  TestOptiXTime.cxx
  TestOptiXWindow.cxx
  )
vtk_test_cxx_executable(vtkRenderingOptiXCxxTests tests RENDERING_FACTORY
  vtkOptiXTestInteractor.cxx
  )

# Do we have MPI and parallel support
if (TARGET vtkRenderingParallel AND TARGET vtkParallelMPI)
  set(vtkRenderingOptiXCxx-MPI_NUMPROCS 2)
  vtk_add_test_mpi(vtkRenderingOptiXCxx-MPI tests
    TESTING_DATA
    TestOptiXComposite.cxx
    )
  vtk_test_cxx_executable(vtkRenderingOptiXCxx-MPI tests RENDERING_FACTORY
    vtkOptiXTestInteractor.cxx
    )

  target_link_libraries(vtkRenderingOptiXCxx-MPI
    PRIVATE
      vtkParallelMPI
      vtkRenderingParallel)
endif ()
