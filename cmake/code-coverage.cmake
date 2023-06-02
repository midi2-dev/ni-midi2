if( CODE_COVERAGE OR (CMAKE_BUILD_TYPE MATCHES Coverage) )
  if (NOT CODE_COVERAGE)
      set( CODE_COVERAGE ON FORCE)
  endif()
  if (CMAKE_BUILD_TYPE MATCHES Coverage)
      set( CMAKE_BUILD_TYPE Debug FORCE )
  endif()

  if( CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
      message(STATUS "Building with llvm Code Coverage Tools")

      add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
      add_link_options(-fprofile-instr-generate -fcoverage-mapping)

  elseif(CMAKE_COMPILER_IS_GNUCXX)
      message(STATUS "Building with lcov Code Coverage Tools")

      add_compile_options( --coverage -fprofile-arcs -ftest-coverage )
      add_link_options( --coverage -fprofile-arcs -ftest-coverage)
  else()
      message(FATAL_ERROR "Code coverage requires Clang or GCC. Aborting.")
  endif()
endif()
