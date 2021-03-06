/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef LAPACK_QR_HOUSEHOLDER_HPP
#define LAPACK_QR_HOUSEHOLDER_HPP

#include "elemental/lapack-like/ApplyPackedReflectors.hpp"
#include "elemental/lapack-like/QR/PanelHouseholder.hpp"

namespace elem {
namespace qr {

// On exit, the upper triangle of A is overwritten by R, and the Householder
// transforms that determine Q are stored below the diagonal of A with an 
// implicit one on the diagonal. 
//
// In the complex case, the column-vector t stores the unit-magnitude complex 
// rotations that map the norms of the implicit Householder vectors to their
// coefficient:  
//                psi_j = 2 tau_j / ( u_j^H u_j ),
// where tau_j is the j'th entry of t and u_j is the j'th unscaled Householder
// reflector.

template<typename F> 
inline void
Householder( Matrix<F>& A, Matrix<F>& t )
{
#ifndef RELEASE
    CallStackEntry entry("qr::Householder");
#endif
    t.ResizeTo( std::min(A.Height(),A.Width()), 1 );

    // Matrix views
    Matrix<F>
        ATL, ATR,  A00, A01, A02,  ALeftPan, ARightPan,
        ABL, ABR,  A10, A11, A12,
                   A20, A21, A22;
    Matrix<F>
        tT,  t0,
        tB,  t1,
             t2;

    PartitionDownDiagonal
    ( A, ATL, ATR,
         ABL, ABR, 0 );
    PartitionDown
    ( t, tT,
         tB, 0 );
    while( ATL.Height() < A.Height() && ATL.Width() < A.Width() )
    {
        RepartitionDownDiagonal
        ( ATL, /**/ ATR,  A00, /**/ A01, A02,
         /*************/ /******************/
               /**/       A10, /**/ A11, A12,
          ABL, /**/ ABR,  A20, /**/ A21, A22 );

        RepartitionDown
        ( tT,  t0,
         /**/ /**/
               t1,
          tB,  t2 );

        View2x1
        ( ALeftPan, A11,
                    A21 );

        View2x1
        ( ARightPan, A12,
                     A22 );

        //--------------------------------------------------------------------//
        PanelHouseholder( ALeftPan, t1 );
        ApplyPackedReflectors
        ( LEFT, LOWER, VERTICAL, FORWARD, CONJUGATED, 
          0, ALeftPan, t1, ARightPan );
        //--------------------------------------------------------------------//

        SlidePartitionDown
        ( tT,  t0,
               t1,
         /**/ /**/
          tB,  t2 );

        SlidePartitionDownDiagonal
        ( ATL, /**/ ATR,  A00, A01, /**/ A02,
               /**/       A10, A11, /**/ A12,
         /*************/ /******************/
          ABL, /**/ ABR,  A20, A21, /**/ A22 );
    }
}

template<typename F> 
inline void
Householder( Matrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("qr::Householder");
#endif
    Matrix<F> t;
    Householder( A, t );
}

template<typename F> 
inline void
Householder( DistMatrix<F>& A, DistMatrix<F,MD,STAR>& t )
{
#ifndef RELEASE
    CallStackEntry entry("qr::Householder");
    if( A.Grid() != t.Grid() )
        throw std::logic_error("{A,s} must be distributed over the same grid");
#endif
    const Grid& g = A.Grid();
    if( t.Viewing() )
    {
        if( !t.AlignedWithDiagonal( A ) ) 
            throw std::logic_error("t was not aligned with A");
        if( t.Height() != std::min(A.Height(),A.Width()) || t.Width() != 1 )
            throw std::logic_error("t was not the appropriate shape");
    }
    else
    {
        t.AlignWithDiagonal( A );
        t.ResizeTo( std::min(A.Height(),A.Width()), 1 );
    }

    // Matrix views
    DistMatrix<F>
        ATL(g), ATR(g),  A00(g), A01(g), A02(g),  ALeftPan(g), ARightPan(g),
        ABL(g), ABR(g),  A10(g), A11(g), A12(g),
                         A20(g), A21(g), A22(g);
    DistMatrix<F,MD,STAR>
        tT(g),  t0(g),
        tB(g),  t1(g),
                t2(g);

    PartitionDownDiagonal
    ( A, ATL, ATR,
         ABL, ABR, 0 );
    PartitionDown
    ( t, tT,
         tB, 0 );
    while( ATL.Height() < A.Height() && ATL.Width() < A.Width() )
    {
        RepartitionDownDiagonal
        ( ATL, /**/ ATR,  A00, /**/ A01, A02,
         /*************/ /******************/
               /**/       A10, /**/ A11, A12,
          ABL, /**/ ABR,  A20, /**/ A21, A22 );

        RepartitionDown
        ( tT,  t0,
         /**/ /**/
               t1,
          tB,  t2 );

        View2x1
        ( ALeftPan, A11,
                    A21 );

        View2x1
        ( ARightPan, A12,
                     A22 );

        //--------------------------------------------------------------------//
        PanelHouseholder( ALeftPan, t1 );
        ApplyPackedReflectors
        ( LEFT, LOWER, VERTICAL, FORWARD, CONJUGATED, 
          0, ALeftPan, t1, ARightPan );
        //--------------------------------------------------------------------//

        SlidePartitionDown
        ( tT,  t0,
               t1,
         /**/ /**/
          tB,  t2 );

        SlidePartitionDownDiagonal
        ( ATL, /**/ ATR,  A00, A01, /**/ A02,
               /**/       A10, A11, /**/ A12,
         /*************/ /******************/
          ABL, /**/ ABR,  A20, A21, /**/ A22 );
    }
}

template<typename F> 
inline void
Householder( DistMatrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("qr::Householder");
#endif
    DistMatrix<F,MD,STAR> t(A.Grid());
    Householder( A, t );
}

} // namespace qr
} // namespace elem

#endif // ifndef LAPACK_QR_HOUSEHOLDER_HPP
