/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef BLAS_TRSV_LN_HPP
#define BLAS_TRSV_LN_HPP

#include "elemental/blas-like/level1/Axpy.hpp"
#include "elemental/blas-like/level2/Gemv.hpp"
#include "elemental/matrices/Zeros.hpp"

namespace elem {
namespace internal {

template<typename F>
inline void
TrsvLN( UnitOrNonUnit diag, const DistMatrix<F>& L, DistMatrix<F>& x )
{
#ifndef RELEASE
    CallStackEntry entry("internal::TrsvLN");
    if( L.Grid() != x.Grid() )
        throw std::logic_error("{L,x} must be distributed over the same grid");
    if( L.Height() != L.Width() )
        throw std::logic_error("L must be square");
    if( x.Width() != 1 && x.Height() != 1 )
        throw std::logic_error("x must be a vector");
    const int xLength = ( x.Width() == 1 ? x.Height() : x.Width() );
    if( L.Width() != xLength )
        throw std::logic_error("Nonconformal TrsvLN");
#endif
    const Grid& g = L.Grid();

    if( x.Width() == 1 )
    {
        // Matrix views 
        DistMatrix<F> L11(g), 
                      L21(g);
        DistMatrix<F> 
            xT(g),  x0(g),
            xB(g),  x1(g),
                    x2(g);

        // Temporary distributions
        DistMatrix<F,STAR,STAR> L11_STAR_STAR(g);
        DistMatrix<F,STAR,STAR> x1_STAR_STAR(g);
        DistMatrix<F,MR,  STAR> x1_MR_STAR(g);
        DistMatrix<F,MC,  STAR> z_MC_STAR(g);

        // Views of z[MC,* ], which will store updates to x
        DistMatrix<F,MC,STAR> z1_MC_STAR(g),
                              z2_MC_STAR(g);

        z_MC_STAR.AlignWith( L );
        Zeros( z_MC_STAR, x.Height(), 1 );

        // Start the algorithm
        PartitionDown
        ( x, xT,
             xB, 0 );
        while( xB.Height() > 0 )
        {
            RepartitionDown
            ( xT,  x0,
             /**/ /**/
                   x1,
              xB,  x2 );

            const int n0 = x0.Height();
            const int n1 = x1.Height();
            const int n2 = x2.Height();
            LockedView( L11, L, n0,    n0, n1, n1 );
            LockedView( L21, L, n0+n1, n0, n2, n1 );
            View( z1_MC_STAR, z_MC_STAR, n0,    0, n1, 1 );
            View( z2_MC_STAR, z_MC_STAR, n0+n1, 0, n2, 1 );

            x1_MR_STAR.AlignWith( L21 );
            //----------------------------------------------------------------//
            if( x0.Height() != 0 )
                x1.SumScatterUpdate( F(1), z1_MC_STAR );

            x1_STAR_STAR = x1;
            L11_STAR_STAR = L11;
            Trsv
            ( LOWER, NORMAL, diag,
              L11_STAR_STAR.LockedMatrix(),
              x1_STAR_STAR.Matrix() );
            x1 = x1_STAR_STAR;

            x1_MR_STAR = x1_STAR_STAR;
            LocalGemv( NORMAL, F(-1), L21, x1_MR_STAR, F(1), z2_MC_STAR );
            //----------------------------------------------------------------//

            SlidePartitionDown
            ( xT,  x0,
                   x1,
             /**/ /**/
              xB,  x2 );
        }
    }
    else
    {
        // Matrix views 
        DistMatrix<F> L11(g),
                      L21(g);
        DistMatrix<F> 
            xL(g), xR(g),
            x0(g), x1(g), x2(g);

        // Temporary distributions
        DistMatrix<F,STAR,STAR> L11_STAR_STAR(g);
        DistMatrix<F,STAR,STAR> x1_STAR_STAR(g);
        DistMatrix<F,STAR,MR  > x1_STAR_MR(g);
        DistMatrix<F,MC,  MR  > z1(g);
        DistMatrix<F,MR,  MC  > z1_MR_MC(g);
        DistMatrix<F,STAR,MC  > z_STAR_MC(g);

        // Views of z[* ,MC]
        DistMatrix<F,STAR,MC> z1_STAR_MC(g),
                              z2_STAR_MC(g);

        z_STAR_MC.AlignWith( L );
        Zeros( z_STAR_MC, 1, x.Width() );

        // Start the algorithm
        PartitionRight( x,  xL, xR, 0 );
        while( xR.Width() > 0 )
        {
            RepartitionRight
            ( xL, /**/ xR,
              x0, /**/ x1, x2 );

            const int n0 = x0.Width();
            const int n1 = x1.Width();
            const int n2 = x2.Width();
            LockedView( L11, L, n0,    n0, n1, n1 );
            LockedView( L21, L, n0+n1, n0, n2, n1 );
            View( z1_STAR_MC, z_STAR_MC, 0, n0,    1, n1 );
            View( z2_STAR_MC, z_STAR_MC, 0, n0+n1, 1, n2 );

            x1_STAR_MR.AlignWith( L21 );
            z1.AlignWith( x1 );
            //----------------------------------------------------------------//
            if( x0.Width() != 0 )
            {
                z1_MR_MC.SumScatterFrom( z1_STAR_MC );
                z1 = z1_MR_MC;
                Axpy( F(1), z1, x1 );
            }

            x1_STAR_STAR = x1;
            L11_STAR_STAR = L11;
            Trsv
            ( LOWER, NORMAL, diag,
              L11_STAR_STAR.LockedMatrix(),
              x1_STAR_STAR.Matrix() );
            x1 = x1_STAR_STAR;

            x1_STAR_MR = x1_STAR_STAR;
            LocalGemv( NORMAL, F(-1), L21, x1_STAR_MR, F(1), z2_STAR_MC );
            //----------------------------------------------------------------//

            SlidePartitionRight
            ( xL,     /**/ xR,
              x0, x1, /**/ x2 );
        }
    }
}

} // namespace internal
} // namespace elem

#endif // ifndef BLAS_TRSV_LN_HPP
