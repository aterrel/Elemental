/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   Copyright (c) 2013, The University of Texas at Austin
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef BLAS_TRMM_LUT_HPP
#define BLAS_TRMM_LUT_HPP

#include "elemental/blas-like/level1/Axpy.hpp"
#include "elemental/blas-like/level1/MakeTriangular.hpp"
#include "elemental/blas-like/level1/Scale.hpp"
#include "elemental/blas-like/level1/SetDiagonal.hpp"
#include "elemental/blas-like/level1/Transpose.hpp"
#include "elemental/blas-like/level3/Gemm.hpp"
#include "elemental/matrices/Zeros.hpp"

namespace elem {
namespace internal {

template<typename T>
inline void
LocalTrmmAccumulateLUT
( Orientation orientation, UnitOrNonUnit diag, T alpha,
  const DistMatrix<T>& U,
  const DistMatrix<T,MC,STAR>& X_MC_STAR,
        DistMatrix<T,MR,STAR>& Z_MR_STAR )
{
#ifndef RELEASE
    CallStackEntry entry("internal::LocalTrmmAccumulateLUT");
    if( U.Grid() != X_MC_STAR.Grid() ||
        X_MC_STAR.Grid() != Z_MR_STAR.Grid() )
        throw std::logic_error
        ("{U,X,Z} must be distributed over the same grid");
    if( U.Height() != U.Width() ||
        U.Height() != X_MC_STAR.Height() ||
        U.Height() != Z_MR_STAR.Height() )
    {
        std::ostringstream msg;
        msg << "Nonconformal LocalTrmmAccumulateLUT: " << "\n"
            << "  U ~ " << U.Height() << " x " << U.Width() << "\n"
            << "  X[MC,* ] ~ " << X_MC_STAR.Height() << " x "
                               << X_MC_STAR.Width() << "\n"
            << "  Z[MR,* ] ` " << Z_MR_STAR.Height() << " x "
                               << Z_MR_STAR.Width() << "\n";
        throw std::logic_error( msg.str().c_str() );
    }
    if( X_MC_STAR.ColAlignment() != U.ColAlignment() ||
        Z_MR_STAR.ColAlignment() != U.RowAlignment() )
        throw std::logic_error("Partial matrix distributions are misaligned");
#endif
    const Grid& g = U.Grid();

    // Matrix views
    DistMatrix<T>
        UTL(g), UTR(g),  U00(g), U01(g), U02(g),
        UBL(g), UBR(g),  U10(g), U11(g), U12(g),
                         U20(g), U21(g), U22(g);

    DistMatrix<T> D11(g);

    DistMatrix<T,MC,STAR>
        XT_MC_STAR(g),  X0_MC_STAR(g),
        XB_MC_STAR(g),  X1_MC_STAR(g),
                        X2_MC_STAR(g);

    DistMatrix<T,MR,STAR>
        ZT_MR_STAR(g),  Z0_MR_STAR(g),
        ZB_MR_STAR(g),  Z1_MR_STAR(g),
                        Z2_MR_STAR(g);

    const int ratio = std::max( g.Height(), g.Width() );
    PushBlocksizeStack( ratio*Blocksize() );

    LockedPartitionDownDiagonal
    ( U, UTL, UTR,
         UBL, UBR, 0 );
    LockedPartitionDown
    ( X_MC_STAR, XT_MC_STAR,
                 XB_MC_STAR, 0 );
    PartitionDown
    ( Z_MR_STAR, ZT_MR_STAR,
                 ZB_MR_STAR, 0 );
    while( UTL.Height() < U.Height() )
    {
        LockedRepartitionDownDiagonal
        ( UTL, /**/ UTR,  U00, /**/ U01, U02,
         /*************/ /******************/
               /**/       U10, /**/ U11, U12,
          UBL, /**/ UBR,  U20, /**/ U21, U22 );

        LockedRepartitionDown
        ( XT_MC_STAR,  X0_MC_STAR,
         /**********/ /**********/
                       X1_MC_STAR,
          XB_MC_STAR,  X2_MC_STAR );

        RepartitionDown
        ( ZT_MR_STAR,  Z0_MR_STAR,
         /**********/ /**********/
                       Z1_MR_STAR,
          ZB_MR_STAR,  Z2_MR_STAR );

        D11.AlignWith( U11 );
        //--------------------------------------------------------------------//
        D11 = U11;
        MakeTriangular( UPPER, D11 );
        if( diag == UNIT )
            SetDiagonal( D11, T(1) );
        LocalGemm
        ( orientation, NORMAL, alpha, D11, X1_MC_STAR, T(1), Z1_MR_STAR );
        LocalGemm
        ( orientation, NORMAL, alpha, U01, X0_MC_STAR, T(1), Z1_MR_STAR );
        //--------------------------------------------------------------------//

        SlideLockedPartitionDownDiagonal
        ( UTL, /**/ UTR,  U00, U01, /**/ U02,
               /**/       U10, U11, /**/ U12,
         /*************/ /******************/
          UBL, /**/ UBR,  U20, U21, /**/ U22 );

        SlideLockedPartitionDown
        ( XT_MC_STAR,  X0_MC_STAR,
                       X1_MC_STAR,
         /**********/ /**********/
          XB_MC_STAR,  X2_MC_STAR );

        SlidePartitionDown
        ( ZT_MR_STAR,  Z0_MR_STAR,
                       Z1_MR_STAR,
         /**********/ /**********/
          ZB_MR_STAR,  Z2_MR_STAR );
    }
    PopBlocksizeStack();
}

template<typename T>
inline void
TrmmLUTA
( Orientation orientation,
  UnitOrNonUnit diag,
  T alpha,
  const DistMatrix<T>& U,
        DistMatrix<T>& X )
{
#ifndef RELEASE
    CallStackEntry entry("internal::TrmmLUTA");
    if( U.Grid() != X.Grid() )
        throw std::logic_error
        ("U and X must be distributed over the same grid");
    if( orientation == NORMAL )
        throw std::logic_error
        ("TrmmLUTA expects a (Conjugate)Transpose option");
    if( U.Height() != U.Width() || U.Height() != X.Height() )
    {
        std::ostringstream msg;
        msg << "Nonconformal TrmmLUTA: \n"
            << "  U ~ " << U.Height() << " x " << U.Width() << "\n"
            << "  X ~ " << X.Height() << " x " << X.Width() << "\n";
        throw std::logic_error( msg.str().c_str() );
    }
#endif
    const Grid& g = U.Grid();

    // Matrix views
    DistMatrix<T>
        UTL(g), UTR(g),  U00(g), U01(g), U02(g),
        UBL(g), UBR(g),  U10(g), U11(g), U12(g),
                         U20(g), U21(g), U22(g);

    DistMatrix<T>
        XL(g), XR(g),
        X0(g), X1(g), X2(g);

    DistMatrix<T,MC,STAR> X1_MC_STAR(g);
    DistMatrix<T,MR,STAR> Z1_MR_STAR(g);
    DistMatrix<T,MR,MC  > Z1_MR_MC(g);

    X1_MC_STAR.AlignWith( U );
    Z1_MR_STAR.AlignWith( U );

    PartitionRight( X, XL, XR, 0 );
    while( XL.Width() < X.Width() )
    {
        RepartitionRight
        ( XL, /**/ XR,
          X0, /**/ X1, X2 );

        //--------------------------------------------------------------------//
        X1_MC_STAR = X1;
        Zeros( Z1_MR_STAR, X1.Height(), X1.Width() );
        LocalTrmmAccumulateLUT
        ( orientation, diag, alpha, U, X1_MC_STAR, Z1_MR_STAR );

        Z1_MR_MC.SumScatterFrom( Z1_MR_STAR );
        X1 = Z1_MR_MC;
        //--------------------------------------------------------------------//

        SlidePartitionRight
        ( XL,     /**/ XR,
          X0, X1, /**/ X2 );
    }
}

template<typename T>
inline void
TrmmLUTCOld
( Orientation orientation, 
  UnitOrNonUnit diag,
  T alpha, const DistMatrix<T>& U,
                 DistMatrix<T>& X )
{
#ifndef RELEASE
    CallStackEntry entry("internal::TrmmLUTCOld");
    if( U.Grid() != X.Grid() )
        throw std::logic_error
        ("U and X must be distributed over the same grid");
    if( orientation == NORMAL )
        throw std::logic_error
        ("TrmmLUTC expects a (Conjugate)Transpose option");
    if( U.Height() != U.Width() || U.Height() != X.Height() )
    {
        std::ostringstream msg;
        msg << "Nonconformal TrmmLUTC: \n"
            << "  U ~ " << U.Height() << " x " << U.Width() << "\n"
            << "  X ~ " << X.Height() << " x " << X.Width() << "\n";
        throw std::logic_error( msg.str().c_str() );
    }
#endif
    const Grid& g = U.Grid();
    const bool conjugate = ( orientation == ADJOINT );

    // Matrix views
    DistMatrix<T> 
        UTL(g), UTR(g),  U00(g), U01(g), U02(g),
        UBL(g), UBR(g),  U10(g), U11(g), U12(g),
                         U20(g), U21(g), U22(g);

    DistMatrix<T> XT(g),  X0(g),
                  XB(g),  X1(g),
                          X2(g);

    // Temporary distributions
    DistMatrix<T,MC,  STAR> U01_MC_STAR(g);
    DistMatrix<T,STAR,STAR> U11_STAR_STAR(g); 
    DistMatrix<T,STAR,VR  > X1_STAR_VR(g);
    DistMatrix<T,MR,  STAR> D1Trans_MR_STAR(g);
    DistMatrix<T,MR,  MC  > D1Trans_MR_MC(g);
    DistMatrix<T,MC,  MR  > D1(g);

    // Start the algorithm
    Scale( alpha, X );
    LockedPartitionUpDiagonal
    ( U, UTL, UTR,
         UBL, UBR, 0 );
    PartitionUp
    ( X, XT,
         XB, 0 );
    while( XT.Height() > 0 )
    {
        LockedRepartitionUpDiagonal
        ( UTL, /**/ UTR,  U00, U01, /**/ U02,
               /**/       U10, U11, /**/ U12,
         /*************/ /******************/
          UBL, /**/ UBR,  U20, U21, /**/ U22 );

        RepartitionUp
        ( XT,  X0,
               X1,
         /**/ /**/
          XB,  X2 );

        U01_MC_STAR.AlignWith( X0 );
        D1Trans_MR_STAR.AlignWith( X1 );
        D1Trans_MR_MC.AlignWith( X1 );
        D1.AlignWith( X1 );
        //--------------------------------------------------------------------//
        X1_STAR_VR = X1;
        U11_STAR_STAR = U11;
        LocalTrmm
        ( LEFT, UPPER, orientation, diag, T(1), U11_STAR_STAR, X1_STAR_VR );
        X1 = X1_STAR_VR;
        
        U01_MC_STAR = U01;
        LocalGemm
        ( orientation, NORMAL, T(1), X0, U01_MC_STAR, D1Trans_MR_STAR );
        D1Trans_MR_MC.SumScatterFrom( D1Trans_MR_STAR );
        Zeros( D1, X1.Height(), X1.Width() );
        Transpose( D1Trans_MR_MC.Matrix(), D1.Matrix(), conjugate );
        Axpy( T(1), D1, X1 );
        //--------------------------------------------------------------------//

        SlideLockedPartitionUpDiagonal
        ( UTL, /**/ UTR,   U00, /**/ U01, U02,
         /*************/  /******************/
               /**/        U10, /**/ U11, U12,
          UBL, /**/ UBR,   U20, /**/ U21, U22 );

        SlidePartitionUp
        ( XT,  X0,
         /**/ /**/
               X1,
          XB,  X2 );
    }
}

template<typename T>
inline void
TrmmLUTC
( Orientation orientation, 
  UnitOrNonUnit diag,
  T alpha, const DistMatrix<T>& U,
                 DistMatrix<T>& X )
{
#ifndef RELEASE
    CallStackEntry entry("internal::TrmmLUTC");
    if( U.Grid() != X.Grid() )
        throw std::logic_error
        ("U and X must be distributed over the same grid");
    if( orientation == NORMAL )
        throw std::logic_error
        ("TrmmLUTC expects a (Conjugate)Transpose option");
    if( U.Height() != U.Width() || U.Height() != X.Height() )
    {
        std::ostringstream msg;
        msg << "Nonconformal TrmmLUTC: \n"
            << "  U ~ " << U.Height() << " x " << U.Width() << "\n"
            << "  X ~ " << X.Height() << " x " << X.Width() << "\n";
        throw std::logic_error( msg.str().c_str() );
    }
#endif
    const Grid& g = U.Grid();

    // Matrix views
    DistMatrix<T> 
        UTL(g), UTR(g),  U00(g), U01(g), U02(g),
        UBL(g), UBR(g),  U10(g), U11(g), U12(g),
                         U20(g), U21(g), U22(g);

    DistMatrix<T> XT(g),  X0(g),
                  XB(g),  X1(g),
                          X2(g);

    // Temporary distributions
    DistMatrix<T,STAR,MC  > U12_STAR_MC(g);
    DistMatrix<T,STAR,STAR> U11_STAR_STAR(g);
    DistMatrix<T,STAR,VR  > X1_STAR_VR(g);
    DistMatrix<T,MR,  STAR> X1Trans_MR_STAR(g);

    // Start the algorithm
    Scale( alpha, X );
    LockedPartitionUpDiagonal
    ( U, UTL, UTR,
         UBL, UBR, 0 );
    PartitionUp
    ( X, XT,
         XB, 0 );
    while( XT.Height() > 0 )
    {
        LockedRepartitionUpDiagonal
        ( UTL, /**/ UTR,  U00, U01, /**/ U02,
               /**/       U10, U11, /**/ U12,
         /*************/ /******************/
          UBL, /**/ UBR,  U20, U21, /**/ U22 );

        RepartitionUp
        ( XT,  X0,
               X1,
         /**/ /**/
          XB,  X2 );

        U12_STAR_MC.AlignWith( X2 );
        X1Trans_MR_STAR.AlignWith( X2 ); 
        X1_STAR_VR.AlignWith( X1 );
        //--------------------------------------------------------------------//
        U12_STAR_MC = U12;
        X1Trans_MR_STAR.TransposeFrom( X1 );
        LocalGemm
        ( orientation, TRANSPOSE, 
          T(1), U12_STAR_MC, X1Trans_MR_STAR, T(1), X2 );

        U11_STAR_STAR = U11;
        X1_STAR_VR.TransposeFrom( X1Trans_MR_STAR );
        LocalTrmm
        ( LEFT, UPPER, orientation, diag, T(1), U11_STAR_STAR, X1_STAR_VR );
        X1 = X1_STAR_VR;
        //--------------------------------------------------------------------//

        SlideLockedPartitionUpDiagonal
        ( UTL, /**/ UTR,   U00, /**/ U01, U02,
         /*************/  /******************/
               /**/        U10, /**/ U11, U12,
          UBL, /**/ UBR,   U20, /**/ U21, U22 );

        SlidePartitionUp
        ( XT,  X0,
         /**/ /**/
               X1,
          XB,  X2 );
    }
}

// Left Upper (Conjugate)Transpose (Non)Unit Trmm
//   X := triu(U)^T  X, 
//   X := triu(U)^H  X,
//   X := triuu(U)^T X, or
//   X := triuu(U)^H X
template<typename T>
inline void
TrmmLUT
( Orientation orientation, 
  UnitOrNonUnit diag,
  T alpha, const DistMatrix<T>& U,
                 DistMatrix<T>& X )
{
#ifndef RELEASE
    CallStackEntry entry("internal::TrmmLUT");
#endif
    // TODO: Come up with a better routing mechanism
    if( U.Height() > 5*X.Width() )
        TrmmLUTA( orientation, diag, alpha, U, X );
    else
        TrmmLUTC( orientation, diag, alpha, U, X );
}

} // namespace internal
} // namespace elem

#endif // ifndef BLAS_TRMM_LUT_HPP
