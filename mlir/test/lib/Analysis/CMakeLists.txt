# Exclude tests from libMLIR.so
add_mlir_library(MLIRTestAnalysis
  TestAliasAnalysis.cpp
  TestCallGraph.cpp
  TestLiveness.cpp
  TestMatchReduction.cpp
  TestMemRefBoundCheck.cpp
  TestMemRefDependenceCheck.cpp
  TestMemRefStrideCalculation.cpp
  TestNumberOfExecutions.cpp


  EXCLUDE_FROM_LIBMLIR

  LINK_LIBS PUBLIC
  MLIRAffine
  MLIRAnalysis
  MLIRMemRef
  MLIRPass
  MLIRTestDialect
  )

target_include_directories(MLIRTestAnalysis
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../Dialect/Test
  ${CMAKE_CURRENT_BINARY_DIR}/../Dialect/Test
  )
