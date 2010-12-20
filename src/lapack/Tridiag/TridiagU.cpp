/*
   Copyright (c) 2009-2010, Jack Poulson
   All rights reserved.

   This file is part of Elemental.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

    - Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    - Neither the name of the owner nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/
#include "elemental/blas_internal.hpp"
#include "elemental/lapack_internal.hpp"
using namespace std;
using namespace elemental;

template<typename R>
void
elemental::lapack::internal::TridiagU
( DistMatrix<R,MC,MR>& A )
{
#ifndef RELEASE
    PushCallStack("lapack::internal::TridiagU");
    if( A.Height() != A.Width() )
        throw logic_error( "A must be square." );
#endif
    const Grid& g = A.Grid();

    // Matrix views 
    DistMatrix<R,MC,MR> 
        ATL(g), ATR(g),  A00(g), A01(g), A02(g),  
        ABL(g), ABR(g),  A10(g), A11(g), A12(g),
                         A20(g), A21(g), A22(g),
        A11Expanded(g);

    // Temporary distributions
    DistMatrix<R,Star,Star> A11_Star_Star(g);
    DistMatrix<R,MD,  Star> e1(g);
    DistMatrix<R,MC,  MR  > W01(g), W11(g),  WPan(g);

    PartitionUpDiagonal
    ( A, ATL, ATR,
         ABL, ABR, 0 );
    while( ABR.Height() < A.Height() )
    {
        RepartitionUpDiagonal
        ( ATL, /**/ ATR,  A00, A01, /**/ A02,
               /**/       A10, A11, /**/ A12,
         /*************/ /******************/
          ABL, /**/ ABR,  A20, A21, /**/ A22 );

        if( A00.Height() > 0 )
        {
            A11Expanded.View
            ( ATL, 
              A00.Height()-1, A00.Width()-1, A11.Height()+1, A11.Width()+1 );
            WPan.AlignWith( A01 );
            WPan.ResizeTo( ATL.Height(), A11.Width() );
            PartitionUp
            ( WPan, W01,
                    W11, A11.Height() );
            e1.AlignWithDiag( A11Expanded, 1 );
            e1.ResizeTo( WPan.Width(), 1 );
            //----------------------------------------------------------------//
            lapack::internal::PanelTridiagU( ATL, WPan, e1 );
            blas::Syr2k( Upper, Normal, (R)-1, A01, W01, (R)1, A00 );
            A11Expanded.SetDiagonal( e1, 1 );
            //----------------------------------------------------------------//
            WPan.FreeAlignments();
            e1.FreeAlignments();
        }
        else
        {
            A11_Star_Star = A11;
            lapack::Tridiag( Upper, A11_Star_Star.LocalMatrix() );
            A11 = A11_Star_Star;
        }

        SlidePartitionUpDiagonal
        ( ATL, /**/ ATR,  A00, /**/ A01, A02,
         /*************/ /******************/
               /**/       A10, /**/ A11, A12,
          ABL, /**/ ABR,  A20, /**/ A21, A22 );
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

#ifndef WITHOUT_COMPLEX
template<typename R>
void
elemental::lapack::internal::TridiagU
( DistMatrix<complex<R>,MC,  MR  >& A,
  DistMatrix<complex<R>,Star,Star>& t )
{
#ifndef RELEASE
    PushCallStack("lapack::internal::TridiagU");
    if( A.Grid() != t.Grid() )
        throw logic_error( "A and t must be distributed over the same grid." );
#endif
    const Grid& g = A.Grid();
#ifndef RELEASE
    if( A.Height() != A.Width() )
        throw logic_error( "A must be square." );
    if( t.Viewing() )
        throw logic_error( "t must not be a view." );
#endif
    typedef complex<R> C;
    DistMatrix<C,MD,Star> tDiag(g);
    tDiag.AlignWithDiag( A, 1 );
    tDiag.ResizeTo( A.DiagonalLength( 1 ), 1 );

    // Matrix views 
    DistMatrix<C,MC,MR> 
        ATL(g), ATR(g),  A00(g), A01(g), A02(g),  
        ABL(g), ABR(g),  A10(g), A11(g), A12(g),
                         A20(g), A21(g), A22(g),
        A11Expanded(g);
    DistMatrix<C,MD,Star> tT(g),  t0(g),
                          tB(g),  t1(g), 
                                  t2(g);

    // Temporary distributions
    Matrix<C> A11_Herm;
    DistMatrix<C,Star,Star> A11_Star_Star(g);
    DistMatrix<R,MD,  Star> e1(g);
    DistMatrix<C,Star,Star> t1_Star_Star(g);
    DistMatrix<C,MC,  MR  > W01(g), W11(g),  WPan(g);

    PartitionUpDiagonal
    ( A, ATL, ATR,
         ABL, ABR, 0 );
    PartitionUp
    ( tDiag, tT,
             tB, 0 );
    while( ABR.Height() < A.Height() )
    {
        RepartitionUpDiagonal
        ( ATL, /**/ ATR,  A00, A01, /**/ A02,
               /**/       A10, A11, /**/ A12,
         /*************/ /******************/
          ABL, /**/ ABR,  A20, A21, /**/ A22 );

        RepartitionUp
        ( tT,  t0,
               t1,
         /**/ /**/
          tB,  t2 );

        if( A00.Height() > 0 )
        {
            A11Expanded.View
            ( ATL, 
              A00.Height()-1, A00.Width()-1, A11.Height()+1, A11.Width()+1 );
            WPan.AlignWith( A01 );
            WPan.ResizeTo( ATL.Height(), A11.Width() );
            PartitionUp
            ( WPan, W01,
                    W11, A11.Height() );
            e1.AlignWithDiag( A11Expanded, 1 );
            e1.ResizeTo( WPan.Width(), 1 );
            //----------------------------------------------------------------//
            lapack::internal::PanelTridiagU( ATL, WPan, e1, t1 );
            blas::Her2k( Upper, Normal, (C)-1, A01, W01, (C)1, A00 );
            A11Expanded.SetDiagonal( e1, 1 );
            //----------------------------------------------------------------//
            WPan.FreeAlignments();
            e1.FreeAlignments();
        }
        else
        {
            A11_Star_Star = A11;
            t1_Star_Star.ResizeTo( t1.Height(), 1 );

            lapack::Tridiag
            ( Upper, 
              A11_Star_Star.LocalMatrix(),        
              t1_Star_Star.LocalMatrix() );
            
            A11 = A11_Star_Star;
            t1 = t1_Star_Star;
        }

        SlidePartitionUpDiagonal
        ( ATL, /**/ ATR,  A00, /**/ A01, A02,
         /*************/ /******************/
               /**/       A10, /**/ A11, A12,
          ABL, /**/ ABR,  A20, /**/ A21, A22 );

        SlidePartitionUp
        ( tT,  t0,
         /**/ /**/
               t1,
          tB,  t2 );
    }
    // Redistribute from matrix-diag form to fully replicated
    t = tDiag;
#ifndef RELEASE
    PopCallStack();
#endif
}
#endif // WITHOUT_COMPLEX

template void elemental::lapack::internal::TridiagU
( DistMatrix<float,MC,MR>& A );

template void elemental::lapack::internal::TridiagU
( DistMatrix<double,MC,MR>& A );

#ifndef WITHOUT_COMPLEX
template void elemental::lapack::internal::TridiagU
( DistMatrix<scomplex,MC,  MR  >& A,
  DistMatrix<scomplex,Star,Star>& t );

template void elemental::lapack::internal::TridiagU
( DistMatrix<dcomplex,MC,  MR  >& A,
  DistMatrix<dcomplex,Star,Star>& t );
#endif

