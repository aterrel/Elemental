/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef LAPACK_EXPANDPACKEDREFLECTORS_LV_HPP
#define LAPACK_EXPANDPACKEDREFLECTORS_LV_HPP

#include "elemental/blas-like/level1/Adjoint.hpp"
#include "elemental/blas-like/level1/MakeTrapezoidal.hpp"
#include "elemental/blas-like/level1/SetDiagonal.hpp"
#include "elemental/blas-like/level1/Transpose.hpp"
#include "elemental/blas-like/level3/Gemm.hpp"
#include "elemental/blas-like/level3/Herk.hpp"
#include "elemental/blas-like/level3/Syrk.hpp"
#include "elemental/blas-like/level3/Trsm.hpp"
#include "elemental/matrices/Identity.hpp"
#include "elemental/matrices/Zeros.hpp"

namespace elem {
namespace expand_packed_reflectors {

//
// Since applying Householder transforms from vectors stored right-to-left
// implies that we will be forming a generalization of
//
//   (I - tau_0 u_0 u_0^H) (I - tau_1 u_1 u_1^H) = 
//   I - tau_0 u_0 u_0^H - tau_1 u_1 u_1^H + (tau_0 tau_1 u_0^H u_1) u_0 u_1^H =
//   I - [ u_0, u_1 ] [ tau_0, -tau_0 tau_1 u_0^H u_1 ] [ u_0^H ]
//                    [ 0,      tau_1                 ] [ u_1^H ],
//
// which has an upper-triangular center matrix, say S, we will form S as 
// the inverse of a matrix T, which can easily be formed as
// 
//   triu(T) = triu( U^H U ),  diag(T) = 1/t or 1/conj(t),
//
// where U is the matrix of Householder vectors and t is the vector of scalars.
//

template<typename F>
inline void
LV( Conjugation conjugation, int offset, Matrix<F>& H, const Matrix<F>& t )
{
#ifndef RELEASE
    CallStackEntry entry("expand_packed_reflectors::LV");
    if( offset > 0 || offset < -H.Height() )
        throw std::logic_error("Transforms out of bounds");
    if( t.Height() != H.DiagonalLength( offset ) )
        throw std::logic_error("t must be the same length as H's offset diag");
#endif
    // Start by zeroing everything above the offset and setting that diagonal
    // to all ones. We can also ensure that H is not wider than it is tall.
    if( H.Width() > H.Height() )
        H.ResizeTo( H.Height(), H.Height() );
    MakeTrapezoidal( LOWER, H, offset );
    SetDiagonal( H, F(1), offset );
    const int dimDiff = H.Height() - H.Width();

    Matrix<F>
        HTL, HTR,  H00, H01, H02,  HPan, HPanCopy, HPanT,
        HBL, HBR,  H10, H11, H12,                  HPanB,
                   H20, H21, H22;
    Matrix<F> HEffected, 
              HEffectedNew, HEffectedOld, 
              HEffectedOldT,
              HEffectedOldB;
    Matrix<F>
        tT,  t0,
        tB,  t1,
             t2;

    Matrix<F> SInv, Z, ZNew, ZOld;

    LockedPartitionUpDiagonal
    ( H, HTL, HTR,
         HBL, HBR, 0 );
    LockedPartitionUp
    ( t, tT,
         tB, 0 );
    int oldEffectedHeight=dimDiff;
    while( HBR.Height() < H.Height() && HBR.Width() < H.Width() )
    {
        LockedRepartitionUpDiagonal
        ( HTL, /**/ HTR,  H00, H01, /**/ H02,
               /**/       H10, H11, /**/ H12,
         /*************/ /******************/
          HBL, /**/ HBR,  H20, H21, /**/ H22 );

        const int HPanHeight = H11.Height() + H21.Height();
        const int effectedHeight = std::max(HPanHeight+offset,0);
        const int HPanWidth = std::min( H11.Width(), effectedHeight );

        const int effectedWidth = effectedHeight - dimDiff;
        const int oldEffectedWidth = oldEffectedHeight - dimDiff;
        const int newEffectedWidth = effectedWidth - oldEffectedWidth;

        LockedView( HPan, H, H00.Height(), H00.Width(), HPanHeight, HPanWidth );
        LockedPartitionDown
        ( HPan, HPanT,
                HPanB, newEffectedWidth /* to match ZNew */ );

        View
        ( HEffected, H, H.Height()-effectedHeight, H.Width()-effectedWidth, 
          effectedHeight, effectedWidth ); 
        PartitionLeft
        ( HEffected, HEffectedNew, HEffectedOld, oldEffectedWidth );
        PartitionUp
        ( HEffectedOld, HEffectedOldT,
                        HEffectedOldB, oldEffectedHeight );

        LockedRepartitionUp
        ( tT,  t0,
               t1,
         /**/ /**/
          tB,  t2, HPanWidth );

        Z.ResizeTo( HPanWidth, effectedWidth );
        PartitionLeft( Z, ZNew, ZOld, oldEffectedWidth );
        //--------------------------------------------------------------------//
        Herk( UPPER, ADJOINT, F(1), HPan, SInv );
        FixDiagonal( conjugation, t1, SInv );

        // Interleave the updates of the already effected portion of the matrix
        // with the newly effected portion to increase performance
        Adjoint( HPanT, ZNew );
        MakeZeros( ZOld );
        Gemm( ADJOINT, NORMAL, F(1), HPanB, HEffectedOldB, F(0), ZOld );
        Trsm( LEFT, UPPER, NORMAL, NON_UNIT, F(1), SInv, Z );
        HPanCopy = HPan;
        MakeIdentity( HEffectedNew );
        Gemm( NORMAL, NORMAL, F(-1), HPanCopy, Z, F(1), HEffected );
        //--------------------------------------------------------------------//

        oldEffectedHeight = effectedHeight;

        SlideLockedPartitionUpDiagonal
        ( HTL, /**/ HTR,  H00, /**/ H01, H02,
         /*************/ /******************/
               /**/       H10, /**/ H11, H12,
          HBL, /**/ HBR,  H20, /**/ H21, H22 );

        SlideLockedPartitionUp
        ( tT,  t0,
         /**/ /**/
               t1,
          tB,  t2 );
    }

    // Take care of any untouched columns on the left side of H
    const int oldEffectedWidth = oldEffectedHeight - dimDiff;
    if( oldEffectedWidth < H.Width() )
    {
        View( HEffectedNew, H, 0, 0, H.Height(), H.Width()-oldEffectedWidth );
        MakeZeros( HEffectedNew );
        SetDiagonal( HEffectedNew, F(1) );
    }
}

template<typename F>
inline void
LV
( Conjugation conjugation, int offset, 
  DistMatrix<F>& H, const DistMatrix<F,MD,STAR>& t )
{
#ifndef RELEASE
    CallStackEntry entry("expand_packed_reflectors::LV");
    if( H.Grid() != t.Grid() )
        throw std::logic_error("H and t must be distributed over same grid");
    if( offset > 0 || offset < -H.Height() )
        throw std::logic_error("Transforms out of bounds");
    if( t.Height() != H.DiagonalLength( offset ) )
        throw std::logic_error("t must be the same length as H's offset diag");
    if( !t.AlignedWithDiagonal( H, offset ) )
        throw std::logic_error("t must be aligned with H's 'offset' diagonal");
#endif
    // Start by zeroing everything above the offset and setting that diagonal
    // to all ones. We can also ensure that H is not wider than it is tall.
    if( H.Width() > H.Height() )
        H.ResizeTo( H.Height(), H.Height() );
    MakeTrapezoidal( LOWER, H, offset );
    SetDiagonal( H, F(1), offset );
    const int dimDiff = H.Height() - H.Width();

    const Grid& g = H.Grid();
    DistMatrix<F>
        HTL(g), HTR(g),  H00(g), H01(g), H02(g),  HPan(g),
        HBL(g), HBR(g),  H10(g), H11(g), H12(g),  
                         H20(g), H21(g), H22(g);
    DistMatrix<F> HEffected(g),
                  HEffectedNew(g), HEffectedOld(g),
                  HEffectedOldT(g),
                  HEffectedOldB(g);
    DistMatrix<F,MD,STAR>
        tT(g),  t0(g),
        tB(g),  t1(g),
                t2(g);

    DistMatrix<F,VC,STAR> HPan_VC_STAR(g);
    DistMatrix<F,MC,STAR> HPan_MC_STAR(g), HPanT_MC_STAR(g),
                                           HPanB_MC_STAR(g);

    DistMatrix<F,STAR,MR> Z_STAR_MR(g),
                          ZNew_STAR_MR(g), ZOld_STAR_MR(g);
    DistMatrix<F,STAR,VR> Z_STAR_VR(g),
                          ZNew_STAR_VR(g), ZOld_STAR_VR(g);
    DistMatrix<F,STAR,STAR> t1_STAR_STAR(g);
    DistMatrix<F,STAR,STAR> SInv_STAR_STAR(g);

    LockedPartitionUpDiagonal
    ( H, HTL, HTR,
         HBL, HBR, 0 );
    LockedPartitionUp
    ( t, tT,
         tB, 0 );
    int oldEffectedHeight=dimDiff;
    while( HBR.Height() < H.Height() && HBR.Width() < H.Width() )
    {
        LockedRepartitionUpDiagonal
        ( HTL, /**/ HTR,  H00, H01, /**/ H02,
               /**/       H10, H11, /**/ H12,
         /*************/ /******************/
          HBL, /**/ HBR,  H20, H21, /**/ H22 );

        const int HPanHeight = H11.Height() + H21.Height();
        const int effectedHeight = std::max(HPanHeight+offset,0);
        const int HPanWidth = std::min( H11.Width(), effectedHeight );

        const int effectedWidth = effectedHeight - dimDiff;
        const int oldEffectedWidth = oldEffectedHeight - dimDiff;
        const int newEffectedWidth = effectedWidth - oldEffectedWidth;

        LockedView( HPan, H, H00.Height(), H00.Width(), HPanHeight, HPanWidth );

        View
        ( HEffected, H, H.Height()-effectedHeight, H.Width()-effectedWidth, 
          effectedHeight, effectedWidth ); 
        PartitionLeft
        ( HEffected, HEffectedNew, HEffectedOld, oldEffectedWidth );
        PartitionUp
        ( HEffectedOld, HEffectedOldT,
                        HEffectedOldB, oldEffectedHeight );

        LockedRepartitionUp
        ( tT,  t0,
               t1,
         /**/ /**/
          tB,  t2, HPanWidth );

        HPan_MC_STAR.AlignWith( HEffected );
        Z_STAR_VR.AlignWith( HEffected );
        Z_STAR_MR.AlignWith( HEffected );
        Z_STAR_MR.ResizeTo( HPanWidth, effectedWidth );
        Z_STAR_VR.ResizeTo( HPanWidth, effectedWidth );
        PartitionLeft
        ( Z_STAR_MR, ZNew_STAR_MR, ZOld_STAR_MR, oldEffectedWidth );
        PartitionLeft
        ( Z_STAR_VR, ZNew_STAR_VR, ZOld_STAR_VR, oldEffectedWidth );
        //--------------------------------------------------------------------//
        HPan_VC_STAR = HPan;
        Zeros( SInv_STAR_STAR, HPanWidth, HPanWidth );
        Herk
        ( UPPER, ADJOINT, 
          F(1), HPan_VC_STAR.LockedMatrix(), 
          F(0), SInv_STAR_STAR.Matrix() );
        SInv_STAR_STAR.SumOverGrid();
        t1_STAR_STAR = t1;
        FixDiagonal( conjugation, t1_STAR_STAR, SInv_STAR_STAR );

        HPan_MC_STAR = HPan;
        LockedPartitionDown
        ( HPan_MC_STAR, HPanT_MC_STAR,
                        HPanB_MC_STAR, newEffectedWidth /* to match ZNew */ );

        // Interleave the updates of the already effected portion of the matrix
        // with the newly effected portion to lower latency and increase 
        // performance
        Adjoint( HPanT_MC_STAR, ZNew_STAR_VR );
        MakeZeros( ZOld_STAR_MR );
        LocalGemm
        ( ADJOINT, NORMAL, 
          F(1), HPanB_MC_STAR, HEffectedOldB, F(0), ZOld_STAR_MR );
        ZOld_STAR_VR.SumScatterFrom( ZOld_STAR_MR );
        LocalTrsm
        ( LEFT, UPPER, NORMAL, NON_UNIT, F(1), SInv_STAR_STAR, Z_STAR_VR );
        Z_STAR_MR = Z_STAR_VR;
        MakeIdentity( HEffectedNew );
        LocalGemm
        ( NORMAL, NORMAL, F(-1), HPan_MC_STAR, Z_STAR_MR, F(1), HEffected );
        //--------------------------------------------------------------------//

        oldEffectedHeight = effectedHeight;

        SlideLockedPartitionUp
        ( tT,  t0,
         /**/ /**/
               t1,
          tB,  t2 );

        SlideLockedPartitionUpDiagonal
        ( HTL, /**/ HTR,  H00, /**/ H01, H02,
         /*************/ /******************/
               /**/       H10, /**/ H11, H12,
          HBL, /**/ HBR,  H20, /**/ H21, H22 );
    }

    // Take care of any untouched columns on the left side of H
    const int oldEffectedWidth = oldEffectedHeight - dimDiff;
    if( oldEffectedWidth < H.Width() )
    {
        View( HEffectedNew, H, 0, 0, H.Height(), H.Width()-oldEffectedWidth );
        MakeZeros( HEffectedNew );
        SetDiagonal( HEffectedNew, F(1) );
    }
}

} // namespace expand_packed_reflectors
} // namespace elem

#endif // ifndef LAPACK_EXPANDPACKEDREFLECTORS_LV_HPP
