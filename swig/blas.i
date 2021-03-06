/*
   Copyright (c) 2009-2013, Jack Poulson
                      2013, Michael C. Grant
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/

%module elem_blas

%include "common.swg"
%import "elem.i"

/*
 * BLAS MISCELLANEOUS
 */
 
NO_OVERLOAD(SetLocalSymvBlocksize,int);
NO_OVERLOAD(LocalSymvBlocksize);
NO_OVERLOAD(SetLocalTrrkBlocksize,int);
NO_OVERLOAD(LocalTrrkBlocksize);
NO_OVERLOAD(SetLocalTrr2kBlocksize,int);
NO_OVERLOAD(LocalTrr2kBlocksize);
%include "elemental/blas-like_decl.hpp"

/*
 * BLAS LEVEL 1
 */

%include "elemental/blas-like/level1/Adjoint.hpp"
%include "elemental/blas-like/level1/Axpy.hpp"
%include "elemental/blas-like/level1/AxpyTriangle.hpp"
%include "elemental/blas-like/level1/Conjugate.hpp"
%include "elemental/blas-like/level1/Copy.hpp"
%include "elemental/blas-like/level1/DiagonalScale.hpp"
%include "elemental/blas-like/level1/DiagonalSolve.hpp"
%include "elemental/blas-like/level1/Dot.hpp"
%include "elemental/blas-like/level1/Dotu.hpp"
%include "elemental/blas-like/level1/MakeHermitian.hpp"
%include "elemental/blas-like/level1/MakeReal.hpp"
%include "elemental/blas-like/level1/MakeSymmetric.hpp"
%include "elemental/blas-like/level1/MakeTrapezoidal.hpp"
%include "elemental/blas-like/level1/MakeTriangular.hpp"
%include "elemental/blas-like/level1/Nrm2.hpp"
%include "elemental/blas-like/level1/Scale.hpp"
%include "elemental/blas-like/level1/ScaleTrapezoid.hpp"
%include "elemental/blas-like/level1/SetDiagonal.hpp"
%include "elemental/blas-like/level1/Transpose.hpp"
%include "elemental/blas-like/level1/Zero.hpp"
 
namespace elem {
OVERLOAD02_int(Adjoint)
OVERLOAD01_int(Axpy)
OVERLOAD01_int(AxpyTriangle)
OVERLOAD012_int(Conjugate)
OVERLOAD0_int(Copy)
OVERLOAD_COPY(Copy,CIRC,CIRC,CIRC,CIRC)
OVERLOAD_COPY(Copy,CIRC,CIRC,MC,MR)
OVERLOAD_COPY(Copy,CIRC,CIRC,MC,STAR)
OVERLOAD_COPY(Copy,CIRC,CIRC,MD,STAR)
OVERLOAD_COPY(Copy,CIRC,CIRC,MR,MC)
OVERLOAD_COPY(Copy,CIRC,CIRC,MR,STAR)
OVERLOAD_COPY(Copy,CIRC,CIRC,STAR,MC)
OVERLOAD_COPY(Copy,CIRC,CIRC,STAR,MD)
OVERLOAD_COPY(Copy,CIRC,CIRC,STAR,MR)
OVERLOAD_COPY(Copy,CIRC,CIRC,STAR,STAR)
OVERLOAD_COPY(Copy,CIRC,CIRC,STAR,VC)
OVERLOAD_COPY(Copy,CIRC,CIRC,STAR,VR)
OVERLOAD_COPY(Copy,CIRC,CIRC,VC,STAR)
OVERLOAD_COPY(Copy,CIRC,CIRC,VR,STAR)
OVERLOAD_COPY(Copy,MC,MR,CIRC,CIRC)
OVERLOAD_COPY(Copy,MC,MR,MC,MR)
OVERLOAD_COPY(Copy,MC,MR,MC,STAR)
OVERLOAD_COPY(Copy,MC,MR,MD,STAR)
OVERLOAD_COPY(Copy,MC,MR,MR,MC)
OVERLOAD_COPY(Copy,MC,MR,MR,STAR)
OVERLOAD_COPY(Copy,MC,MR,STAR,MC)
OVERLOAD_COPY(Copy,MC,MR,STAR,MD)
OVERLOAD_COPY(Copy,MC,MR,STAR,MR)
OVERLOAD_COPY(Copy,MC,MR,STAR,STAR)
OVERLOAD_COPY(Copy,MC,MR,STAR,VC)
OVERLOAD_COPY(Copy,MC,MR,STAR,VR)
OVERLOAD_COPY(Copy,MC,MR,VC,STAR)
OVERLOAD_COPY(Copy,MC,MR,VR,STAR)
OVERLOAD_COPY(Copy,MC,STAR,CIRC,CIRC)
OVERLOAD_COPY(Copy,MC,STAR,MC,MR)
OVERLOAD_COPY(Copy,MC,STAR,MC,STAR)
OVERLOAD_COPY(Copy,MC,STAR,MD,STAR)
OVERLOAD_COPY(Copy,MC,STAR,MR,MC)
OVERLOAD_COPY(Copy,MC,STAR,MR,STAR)
OVERLOAD_COPY(Copy,MC,STAR,STAR,MC)
OVERLOAD_COPY(Copy,MC,STAR,STAR,MD)
OVERLOAD_COPY(Copy,MC,STAR,STAR,MR)
OVERLOAD_COPY(Copy,MC,STAR,STAR,STAR)
OVERLOAD_COPY(Copy,MC,STAR,STAR,VC)
OVERLOAD_COPY(Copy,MC,STAR,STAR,VR)
OVERLOAD_COPY(Copy,MC,STAR,VC,STAR)
OVERLOAD_COPY(Copy,MC,STAR,VR,STAR)
OVERLOAD_COPY(Copy,MD,STAR,CIRC,CIRC)
OVERLOAD_COPY(Copy,MD,STAR,MC,MR)
OVERLOAD_COPY(Copy,MD,STAR,MC,STAR)
OVERLOAD_COPY(Copy,MD,STAR,MD,STAR)
OVERLOAD_COPY(Copy,MD,STAR,MR,MC)
OVERLOAD_COPY(Copy,MD,STAR,MR,STAR)
OVERLOAD_COPY(Copy,MD,STAR,STAR,MC)
OVERLOAD_COPY(Copy,MD,STAR,STAR,MD)
OVERLOAD_COPY(Copy,MD,STAR,STAR,MR)
OVERLOAD_COPY(Copy,MD,STAR,STAR,STAR)
OVERLOAD_COPY(Copy,MD,STAR,STAR,VC)
OVERLOAD_COPY(Copy,MD,STAR,STAR,VR)
OVERLOAD_COPY(Copy,MD,STAR,VC,STAR)
OVERLOAD_COPY(Copy,MD,STAR,VR,STAR)
OVERLOAD_COPY(Copy,MR,MC,CIRC,CIRC)
OVERLOAD_COPY(Copy,MR,MC,MC,MR)
OVERLOAD_COPY(Copy,MR,MC,MC,STAR)
OVERLOAD_COPY(Copy,MR,MC,MD,STAR)
OVERLOAD_COPY(Copy,MR,MC,MR,MC)
OVERLOAD_COPY(Copy,MR,MC,MR,STAR)
OVERLOAD_COPY(Copy,MR,MC,STAR,MC)
OVERLOAD_COPY(Copy,MR,MC,STAR,MD)
OVERLOAD_COPY(Copy,MR,MC,STAR,MR)
OVERLOAD_COPY(Copy,MR,MC,STAR,STAR)
OVERLOAD_COPY(Copy,MR,MC,STAR,VC)
OVERLOAD_COPY(Copy,MR,MC,STAR,VR)
OVERLOAD_COPY(Copy,MR,MC,VC,STAR)
OVERLOAD_COPY(Copy,MR,MC,VR,STAR)
OVERLOAD_COPY(Copy,MR,STAR,CIRC,CIRC)
OVERLOAD_COPY(Copy,MR,STAR,MC,MR)
OVERLOAD_COPY(Copy,MR,STAR,MC,STAR)
OVERLOAD_COPY(Copy,MR,STAR,MD,STAR)
OVERLOAD_COPY(Copy,MR,STAR,MR,MC)
OVERLOAD_COPY(Copy,MR,STAR,MR,STAR)
OVERLOAD_COPY(Copy,MR,STAR,STAR,MC)
OVERLOAD_COPY(Copy,MR,STAR,STAR,MD)
OVERLOAD_COPY(Copy,MR,STAR,STAR,MR)
OVERLOAD_COPY(Copy,MR,STAR,STAR,STAR)
OVERLOAD_COPY(Copy,MR,STAR,STAR,VC)
OVERLOAD_COPY(Copy,MR,STAR,STAR,VR)
OVERLOAD_COPY(Copy,MR,STAR,VC,STAR)
OVERLOAD_COPY(Copy,MR,STAR,VR,STAR)
OVERLOAD_COPY(Copy,STAR,MC,CIRC,CIRC)
OVERLOAD_COPY(Copy,STAR,MC,MC,MR)
OVERLOAD_COPY(Copy,STAR,MC,MC,STAR)
OVERLOAD_COPY(Copy,STAR,MC,MD,STAR)
OVERLOAD_COPY(Copy,STAR,MC,MR,MC)
OVERLOAD_COPY(Copy,STAR,MC,MR,STAR)
OVERLOAD_COPY(Copy,STAR,MC,STAR,MC)
OVERLOAD_COPY(Copy,STAR,MC,STAR,MD)
OVERLOAD_COPY(Copy,STAR,MC,STAR,MR)
OVERLOAD_COPY(Copy,STAR,MC,STAR,STAR)
OVERLOAD_COPY(Copy,STAR,MC,STAR,VC)
OVERLOAD_COPY(Copy,STAR,MC,STAR,VR)
OVERLOAD_COPY(Copy,STAR,MC,VC,STAR)
OVERLOAD_COPY(Copy,STAR,MC,VR,STAR)
OVERLOAD_COPY(Copy,STAR,MD,CIRC,CIRC)
OVERLOAD_COPY(Copy,STAR,MD,MC,MR)
OVERLOAD_COPY(Copy,STAR,MD,MC,STAR)
OVERLOAD_COPY(Copy,STAR,MD,MD,STAR)
OVERLOAD_COPY(Copy,STAR,MD,MR,MC)
OVERLOAD_COPY(Copy,STAR,MD,MR,STAR)
OVERLOAD_COPY(Copy,STAR,MD,STAR,MC)
OVERLOAD_COPY(Copy,STAR,MD,STAR,MD)
OVERLOAD_COPY(Copy,STAR,MD,STAR,MR)
OVERLOAD_COPY(Copy,STAR,MD,STAR,STAR)
OVERLOAD_COPY(Copy,STAR,MD,STAR,VC)
OVERLOAD_COPY(Copy,STAR,MD,STAR,VR)
OVERLOAD_COPY(Copy,STAR,MD,VC,STAR)
OVERLOAD_COPY(Copy,STAR,MD,VR,STAR)
OVERLOAD_COPY(Copy,STAR,MR,CIRC,CIRC)
OVERLOAD_COPY(Copy,STAR,MR,MC,MR)
OVERLOAD_COPY(Copy,STAR,MR,MC,STAR)
OVERLOAD_COPY(Copy,STAR,MR,MD,STAR)
OVERLOAD_COPY(Copy,STAR,MR,MR,MC)
OVERLOAD_COPY(Copy,STAR,MR,MR,STAR)
OVERLOAD_COPY(Copy,STAR,MR,STAR,MC)
OVERLOAD_COPY(Copy,STAR,MR,STAR,MD)
OVERLOAD_COPY(Copy,STAR,MR,STAR,MR)
OVERLOAD_COPY(Copy,STAR,MR,STAR,STAR)
OVERLOAD_COPY(Copy,STAR,MR,STAR,VC)
OVERLOAD_COPY(Copy,STAR,MR,STAR,VR)
OVERLOAD_COPY(Copy,STAR,MR,VC,STAR)
OVERLOAD_COPY(Copy,STAR,MR,VR,STAR)
OVERLOAD_COPY(Copy,STAR,STAR,CIRC,CIRC)
OVERLOAD_COPY(Copy,STAR,STAR,MC,MR)
OVERLOAD_COPY(Copy,STAR,STAR,MC,STAR)
OVERLOAD_COPY(Copy,STAR,STAR,MD,STAR)
OVERLOAD_COPY(Copy,STAR,STAR,MR,MC)
OVERLOAD_COPY(Copy,STAR,STAR,MR,STAR)
OVERLOAD_COPY(Copy,STAR,STAR,STAR,MC)
OVERLOAD_COPY(Copy,STAR,STAR,STAR,MD)
OVERLOAD_COPY(Copy,STAR,STAR,STAR,MR)
OVERLOAD_COPY(Copy,STAR,STAR,STAR,STAR)
OVERLOAD_COPY(Copy,STAR,STAR,STAR,VC)
OVERLOAD_COPY(Copy,STAR,STAR,STAR,VR)
OVERLOAD_COPY(Copy,STAR,STAR,VC,STAR)
OVERLOAD_COPY(Copy,STAR,STAR,VR,STAR)
OVERLOAD_COPY(Copy,STAR,VC,CIRC,CIRC)
OVERLOAD_COPY(Copy,STAR,VC,MC,MR)
OVERLOAD_COPY(Copy,STAR,VC,MC,STAR)
OVERLOAD_COPY(Copy,STAR,VC,MD,STAR)
OVERLOAD_COPY(Copy,STAR,VC,MR,MC)
OVERLOAD_COPY(Copy,STAR,VC,MR,STAR)
OVERLOAD_COPY(Copy,STAR,VC,STAR,MC)
OVERLOAD_COPY(Copy,STAR,VC,STAR,MD)
OVERLOAD_COPY(Copy,STAR,VC,STAR,MR)
OVERLOAD_COPY(Copy,STAR,VC,STAR,STAR)
OVERLOAD_COPY(Copy,STAR,VC,STAR,VC)
OVERLOAD_COPY(Copy,STAR,VC,STAR,VR)
OVERLOAD_COPY(Copy,STAR,VC,VC,STAR)
OVERLOAD_COPY(Copy,STAR,VC,VR,STAR)
OVERLOAD_COPY(Copy,STAR,VR,CIRC,CIRC)
OVERLOAD_COPY(Copy,STAR,VR,MC,MR)
OVERLOAD_COPY(Copy,STAR,VR,MC,STAR)
OVERLOAD_COPY(Copy,STAR,VR,MD,STAR)
OVERLOAD_COPY(Copy,STAR,VR,MR,MC)
OVERLOAD_COPY(Copy,STAR,VR,MR,STAR)
OVERLOAD_COPY(Copy,STAR,VR,STAR,MC)
OVERLOAD_COPY(Copy,STAR,VR,STAR,MD)
OVERLOAD_COPY(Copy,STAR,VR,STAR,MR)
OVERLOAD_COPY(Copy,STAR,VR,STAR,STAR)
OVERLOAD_COPY(Copy,STAR,VR,STAR,VC)
OVERLOAD_COPY(Copy,STAR,VR,STAR,VR)
OVERLOAD_COPY(Copy,STAR,VR,VC,STAR)
OVERLOAD_COPY(Copy,STAR,VR,VR,STAR)
OVERLOAD_COPY(Copy,VC,STAR,CIRC,CIRC)
OVERLOAD_COPY(Copy,VC,STAR,MC,MR)
OVERLOAD_COPY(Copy,VC,STAR,MC,STAR)
OVERLOAD_COPY(Copy,VC,STAR,MD,STAR)
OVERLOAD_COPY(Copy,VC,STAR,MR,MC)
OVERLOAD_COPY(Copy,VC,STAR,MR,STAR)
OVERLOAD_COPY(Copy,VC,STAR,STAR,MC)
OVERLOAD_COPY(Copy,VC,STAR,STAR,MD)
OVERLOAD_COPY(Copy,VC,STAR,STAR,MR)
OVERLOAD_COPY(Copy,VC,STAR,STAR,STAR)
OVERLOAD_COPY(Copy,VC,STAR,STAR,VC)
OVERLOAD_COPY(Copy,VC,STAR,STAR,VR)
OVERLOAD_COPY(Copy,VC,STAR,VC,STAR)
OVERLOAD_COPY(Copy,VC,STAR,VR,STAR)
OVERLOAD_COPY(Copy,VR,STAR,CIRC,CIRC)
OVERLOAD_COPY(Copy,VR,STAR,MC,MR)
OVERLOAD_COPY(Copy,VR,STAR,MC,STAR)
OVERLOAD_COPY(Copy,VR,STAR,MD,STAR)
OVERLOAD_COPY(Copy,VR,STAR,MR,MC)
OVERLOAD_COPY(Copy,VR,STAR,MR,STAR)
OVERLOAD_COPY(Copy,VR,STAR,STAR,MC)
OVERLOAD_COPY(Copy,VR,STAR,STAR,MD)
OVERLOAD_COPY(Copy,VR,STAR,STAR,MR)
OVERLOAD_COPY(Copy,VR,STAR,STAR,STAR)
OVERLOAD_COPY(Copy,VR,STAR,STAR,VC)
OVERLOAD_COPY(Copy,VR,STAR,STAR,VR)
OVERLOAD_COPY(Copy,VR,STAR,VC,STAR)
OVERLOAD_COPY(Copy,VR,STAR,VR,STAR)
OVERLOAD02_int(DiagonalScale)
OVERLOAD02(DiagonalSolve)
OVERLOAD01_int(Dot)
OVERLOAD01_int(Dotu)
OVERLOAD0_int(MakeHermitian)
OVERLOAD01_int(MakeReal)
OVERLOAD0_int(MakeSymmetric)
OVERLOAD01_int(MakeTrapezoidal)
OVERLOAD01_int(MakeTriangular)
OVERLOAD0(Nrm2)
OVERLOAD01_int(Scale)
OVERLOAD01_int(ScaleTrapezoid)
OVERLOAD01_int(SetDiagonal)
OVERLOAD02_int(Transpose)
OVERLOAD01_int(Zero)
};

/*
 * BLAS LEVEL 2
 */

// The equivalent of elemental/blas-like/level2.hpp
%include "elemental/blas-like/level2/Gemv.hpp"
%include "elemental/blas-like/level2/Ger.hpp"
%include "elemental/blas-like/level2/Geru.hpp"
%include "elemental/blas-like/level2/Hemv.hpp"
%include "elemental/blas-like/level2/Her.hpp"
%include "elemental/blas-like/level2/Her2.hpp"
%include "elemental/blas-like/level2/Symv.hpp"
%include "elemental/blas-like/level2/Syr.hpp"
%include "elemental/blas-like/level2/Syr2.hpp"
%include "elemental/blas-like/level2/Trmv.hpp"
%include "elemental/blas-like/level2/Trsv.hpp"

// Most of these could support ints (except for Trsv)
namespace elem {
OVERLOAD0(Gemv)
OVERLOAD0(Ger)
OVERLOAD0(Geru)
OVERLOAD0(Hemv)
OVERLOAD0(Her)
OVERLOAD0(Her2)
OVERLOAD0(Symv)
OVERLOAD0(Syr)
OVERLOAD0(Syr2)
OVERLOAD0(Trsv)
};

/* 
 * BLAS LEVEL 3
 */ 
 
// The equivalent of elemental/blas-like/level3.hpp
%include "elemental/blas-like/level3/Gemm.hpp"
%include "elemental/blas-like/level3/Hemm.hpp"
%include "elemental/blas-like/level3/Her2k.hpp"
%include "elemental/blas-like/level3/Herk.hpp"
%include "elemental/blas-like/level3/Symm.hpp"
%include "elemental/blas-like/level3/Syr2k.hpp"
%include "elemental/blas-like/level3/Syrk.hpp"
%include "elemental/blas-like/level3/Trdtrmm.hpp"
%include "elemental/blas-like/level3/Trmm.hpp"
%include "elemental/blas-like/level3/Trsm.hpp"
%include "elemental/blas-like/level3/Trtrmm.hpp"
%include "elemental/blas-like/level3/Trtrsm.hpp"
%include "elemental/blas-like/level3/TwoSidedTrmm.hpp"
%include "elemental/blas-like/level3/TwoSidedTrsm.hpp"

namespace elem {
// Most of these could support ints if necessary
OVERLOAD0(Gemm)
OVERLOAD0(Hemm)
OVERLOAD0(Her2k)
OVERLOAD0(Herk)
OVERLOAD0(Symm)
OVERLOAD0(Syr2k)
OVERLOAD0(Syrk)
OVERLOAD0(Trdtrmm)
OVERLOAD0(Trmm)
OVERLOAD0(Trsm)
OVERLOAD0(Trtrmm)
OVERLOAD0(Trtrsm)
OVERLOAD0(TwoSidedTrmm)
OVERLOAD0(TwoSidedTrsm)
};
