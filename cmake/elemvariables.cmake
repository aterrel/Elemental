# To help simplify including Elemental in external projects

ELEM_INC = @CMAKE_INSTALL_PREFIX@/include
ELEM_LIB = @CMAKE_INSTALL_PREFIX@/lib

CC = @CMAKE_C_COMPILER@
CXX = @CMAKE_CXX_COMPILER@
CXX_FLAGS = @CXX_FLAGS@

MATH_LIBS = @MATH_LIBS_STRING@

MPI_CXX_INCLUDE_STRING = @MPI_CXX_INCLUDE_STRING@
MPI_CXX_COMPILE_FLAGS = @MPI_CXX_COMPILE_FLAGS@
MPI_CXX_LINK_FLAGS = @MPI_CXX_LINK_FLAGS@
MPI_CXX_LIBS = @MPI_CXX_LIBS@

ELEM_COMPILE_FLAGS = ${CXX_FLAGS} -I${ELEM_INC} ${MPI_CXX_INCLUDE_STRING}
ELEM_LINK_FLAGS = -L${ELEM_LIB} ${MPI_CXX_LINK_FLAGS}

WITHOUT_PMRRR = @WITHOUT_PMRRR@
ifeq (${WITHOUT_PMRRR},TRUE)
  PMRRR_LIBS = 
else
  MISSING_LAPACK = @MISSING_LAPACK@
  ifeq (${MISSING_LAPACK},TRUE)
    LAPACK_ADDONS = -llapack-addons @LAPACK_ADDONS_STRING@
  else
    LAPACK_ADDONS = 
  endif
  THREAD_INIT = @CMAKE_THREAD_LIBS_INIT@
  PMRRR_LIBS = -lpmrrr ${LAPACK_ADDONS} ${THREAD_INIT}
endif
ELEM_C = -lexperimental-c
ELEM_F90 = -lexperimental-f90
ELEM_LIBS = -lelemental -lplcg ${PMRRR_LIBS} -lelem-dummy-lib \
            ${MATH_LIBS} ${MPI_CXX_LIBS}