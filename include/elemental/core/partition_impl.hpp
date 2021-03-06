/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef CORE_PARTITION_IMPL_HPP
#define CORE_PARTITION_IMPL_HPP

namespace elem {

// To make our life easier. Undef'd at the bottom of the header
#define M  Matrix<T,Int>
#define DM DistMatrix<T,U,V,Int>

//
// PartitionUp
//

template<typename T,typename Int>
inline void
PartitionUp
( M& A, M& AT,
        M& AB, Int heightAB )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionUp [Matrix]");
#endif
    PartitionDown( A, AT, AB, A.Height()-heightAB );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
PartitionUp
( DM& A, DM& AT,
         DM& AB, Int heightAB )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionUp [DistMatrix]");
#endif
    PartitionDown( A, AT, AB, A.Height()-heightAB );
}

template<typename T,typename Int>
inline void
LockedPartitionUp
( const M& A, M& AT,
              M& AB, Int heightAB )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionUp [Matrix]");
#endif
    LockedPartitionDown( A, AT, AB, A.Height()-heightAB );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
LockedPartitionUp
( const DM& A, DM& AT,
               DM& AB, Int heightAB )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionUp [DistMatrix]");
#endif
    LockedPartitionDown( A, AT, AB, A.Height()-heightAB );
}

//
// PartitionDown
//

template<typename T,typename Int>
inline void
PartitionDown
( M& A, M& AT,
        M& AB, Int heightAT ) 
{
#ifndef RELEASE
    CallStackEntry entry("PartitionDown [Matrix]");
#endif
    heightAT = std::max(std::min(heightAT,A.Height()),0);
    const Int heightAB = A.Height()-heightAT;
    View( AT, A, 0,        0, heightAT, A.Width() );
    View( AB, A, heightAT, 0, heightAB, A.Width() );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
PartitionDown
( DM& A, DM& AT,
         DM& AB, Int heightAT )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionDown [DistMatrix]");
#endif
    heightAT = std::max(std::min(heightAT,A.Height()),0);
    const Int heightAB = A.Height()-heightAT;
    View( AT, A, 0,        0, heightAT, A.Width() );
    View( AB, A, heightAT, 0, heightAB, A.Width() );
}

template<typename T,typename Int>
inline void
LockedPartitionDown
( const M& A, M& AT,
              M& AB, Int heightAT ) 
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionDown [Matrix]");
#endif
    heightAT = std::max(std::min(heightAT,A.Height()),0);
    const Int heightAB = A.Height()-heightAT;
    LockedView( AT, A, 0,        0, heightAT, A.Width() );
    LockedView( AB, A, heightAT, 0, heightAB, A.Width() );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
LockedPartitionDown
( const DM& A, DM& AT,
               DM& AB, Int heightAT )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionDown [DistMatrix]");
#endif
    heightAT = std::max(std::min(heightAT,A.Height()),0);
    const Int heightAB = A.Height()-heightAT;
    LockedView( AT, A, 0,        0, heightAT, A.Width() );
    LockedView( AB, A, heightAT, 0, heightAB, A.Width() );
}

//
// PartitionLeft
//

template<typename T,typename Int>
inline void
PartitionLeft( M& A, M& AL, M& AR, Int widthAR )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionLeft [Matrix]");
#endif
    PartitionRight( A, AL, AR, A.Width()-widthAR );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
PartitionLeft( DM& A, DM& AL, DM& AR, Int widthAR )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionLeft [DistMatrix]");
#endif
    PartitionRight( A, AL, AR, A.Width()-widthAR );
}

template<typename T,typename Int>
inline void
LockedPartitionLeft( const M& A, M& AL, M& AR, Int widthAR )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionLeft [Matrix]");
#endif
    LockedPartitionRight( A, AL, AR, A.Width()-widthAR );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
LockedPartitionLeft( const DM& A, DM& AL, DM& AR, Int widthAR )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionLeft [DistMatrix]");
#endif
    LockedPartitionRight( A, AL, AR, A.Width()-widthAR );
}

//
// PartitionRight
//

template<typename T,typename Int>
inline void
PartitionRight( M& A, M& AL, M& AR, Int widthAL )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionRight [Matrix]");
#endif
    widthAL = std::max(std::min(widthAL,A.Width()),0);
    const Int widthAR = A.Width()-widthAL;
    View( AL, A, 0, 0,       A.Height(), widthAL );
    View( AR, A, 0, widthAL, A.Height(), widthAR );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
PartitionRight( DM& A, DM& AL, DM& AR, Int widthAL )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionRight [DistMatrix]");
#endif
    widthAL = std::max(std::min(widthAL,A.Width()),0);
    const Int widthAR = A.Width()-widthAL;
    View( AL, A, 0, 0,       A.Height(), widthAL );
    View( AR, A, 0, widthAL, A.Height(), widthAR );
}

template<typename T,typename Int>
inline void
LockedPartitionRight( const M& A, M& AL, M& AR, Int widthAL )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionRight [Matrix]");
#endif
    widthAL = std::max(std::min(widthAL,A.Width()),0);
    const Int widthAR = A.Width()-widthAL;
    LockedView( AL, A, 0, 0,       A.Height(), widthAL );
    LockedView( AR, A, 0, widthAL, A.Height(), widthAR );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
LockedPartitionRight( const DM& A, DM& AL, DM& AR, Int widthAL )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionRight [DistMatrix]");
#endif
    widthAL = std::max(std::min(widthAL,A.Width()),0);
    const Int widthAR = A.Width()-widthAL;
    LockedView( AL, A, 0, 0,       A.Height(), widthAL );
    LockedView( AR, A, 0, widthAL, A.Height(), widthAR );
}

//
// PartitionUpDiagonal
//

template<typename T,typename Int>
inline void
PartitionUpDiagonal
( M& A, M& ATL, M& ATR,
        M& ABL, M& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionUpDiagonal [Matrix]");
#endif
    PartitionUpOffsetDiagonal( 0, A, ATL, ATR, ABL, ABR, diagDist );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
PartitionUpDiagonal
( DM& A, DM& ATL, DM& ATR,
         DM& ABL, DM& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionUpDiagonal [DistMatrix]");
#endif
    PartitionUpOffsetDiagonal( 0, A, ATL, ATR, ABL, ABR, diagDist );
}

template<typename T,typename Int>
inline void
LockedPartitionUpDiagonal
( const M& A, M& ATL, M& ATR,
              M& ABL, M& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionUpDiagonal [Matrix]");
#endif
    LockedPartitionUpOffsetDiagonal( 0, A, ATL, ATR, ABL, ABR, diagDist );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
LockedPartitionUpDiagonal
( const DM& A, DM& ATL, DM& ATR,
               DM& ABL, DM& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionUpDiagonal [DistMatrix]");
#endif
    LockedPartitionUpOffsetDiagonal( 0, A, ATL, ATR, ABL, ABR, diagDist );
}

//
// PartitionUpOffsetDiagonal
//

template<typename T,typename Int>
inline void
PartitionUpOffsetDiagonal
( Int offset,
  M& A, M& ATL, M& ATR,
        M& ABL, M& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionUpOffsetDiagonal [Matrix]");
#endif
    PartitionDownOffsetDiagonal
    ( offset, A, ATL, ATR, ABL, ABR, A.DiagonalLength(offset)-diagDist );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
PartitionUpOffsetDiagonal
( Int offset,
  DM& A, DM& ATL, DM& ATR,
         DM& ABL, DM& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionUpOffsetDiagonal [DistMatrix]");
#endif
    PartitionDownOffsetDiagonal
    ( offset, A, ATL, ATR, ABL, ABR, A.DiagonalLength(offset)-diagDist );
}

template<typename T,typename Int>
inline void
LockedPartitionUpOffsetDiagonal
( Int offset,
  const M& A, M& ATL, M& ATR,
              M& ABL, M& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionUpOffsetDiagonal [Matrix]");
#endif
    LockedPartitionDownOffsetDiagonal
    ( offset, A, ATL, ATR, ABL, ABR, A.DiagonalLength(offset)-diagDist );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
LockedPartitionUpOffsetDiagonal
( Int offset,
  const DM& A, DM& ATL, DM& ATR,
               DM& ABL, DM& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionUpOffsetDiagonal [DistMatrix]");
#endif
    LockedPartitionDownOffsetDiagonal
    ( offset, A, ATL, ATR, ABL, ABR, A.DiagonalLength(offset)-diagDist );
}

//
// PartitionDownDiagonal
//

template<typename T,typename Int>
inline void
PartitionDownDiagonal
( M& A, M& ATL, M& ATR,
        M& ABL, M& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionDownDiagonal [Matrix]");
#endif
    PartitionDownOffsetDiagonal( 0, A, ATL, ATR, ABL, ABR, diagDist );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
PartitionDownDiagonal
( DM& A, DM& ATL, DM& ATR,
         DM& ABL, DM& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionDownDiagonal [DistMatrix]");
#endif
    PartitionDownOffsetDiagonal( 0, A, ATL, ATR, ABL, ABR, diagDist );
}

template<typename T,typename Int>
inline void
LockedPartitionDownDiagonal
( const M& A, M& ATL, M& ATR,
              M& ABL, M& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionDownDiagonal [Matrix]");
#endif
    LockedPartitionDownOffsetDiagonal( 0, A, ATL, ATR, ABL, ABR, diagDist );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
LockedPartitionDownDiagonal
( const DM& A, DM& ATL, DM& ATR,
               DM& ABL, DM& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionDownDiagonal [DistMatrix]");
#endif
    LockedPartitionDownOffsetDiagonal( 0, A, ATL, ATR, ABL, ABR, diagDist );
}

//
// PartitionDownOffsetDiagonal
//

template<typename T,typename Int>
inline void
PartitionDownOffsetDiagonal
( Int offset,
  M& A, M& ATL, M& ATR,
        M& ABL, M& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionDownOffsetDiagonal [Matrix]");
#endif
    const int m = A.Height();
    const int n = A.Width();
    const Int diagLength = A.DiagonalLength(offset);
    diagDist = std::max(std::min(diagDist,diagLength),0);
    
    const int mCut = ( offset<=0 ? -offset+diagDist : diagDist );
    const int nCut = ( offset<=0 ? diagDist : offset+diagDist );
    View( ATL, A, 0,    0,    mCut,   nCut   );
    View( ATR, A, 0,    nCut, mCut,   n-nCut );
    View( ABL, A, mCut, 0,    m-mCut, nCut   );
    View( ABR, A, mCut, nCut, m-mCut, n-nCut );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
PartitionDownOffsetDiagonal
( Int offset,
  DM& A, DM& ATL, DM& ATR,
         DM& ABL, DM& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("PartitionDownOffsetDiagonal [DistMatrix]");
#endif
    const int m = A.Height();
    const int n = A.Width();
    const Int diagLength = A.DiagonalLength(offset);
    diagDist = std::max(std::min(diagDist,diagLength),0);

    const int mCut = ( offset<=0 ? -offset+diagDist : diagDist );
    const int nCut = ( offset<=0 ? diagDist : offset+diagDist );
    View( ATL, A, 0,    0,    mCut,   nCut   );
    View( ATR, A, 0,    nCut, mCut,   n-nCut );
    View( ABL, A, mCut, 0,    m-mCut, nCut   );
    View( ABR, A, mCut, nCut, m-mCut, n-nCut );
}

template<typename T,typename Int>
inline void
LockedPartitionDownOffsetDiagonal
( Int offset,
  const M& A, M& ATL, M& ATR,
              M& ABL, M& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionDownOffsetDiagonal [Matrix]");
#endif
    const int m = A.Height();
    const int n = A.Width();
    const Int diagLength = A.DiagonalLength(offset);
    diagDist = std::max(std::min(diagDist,diagLength),0);
    
    const int mCut = ( offset<=0 ? -offset+diagDist : diagDist );
    const int nCut = ( offset<=0 ? diagDist : offset+diagDist );
    LockedView( ATL, A, 0,    0,    mCut,   nCut   );
    LockedView( ATR, A, 0,    nCut, mCut,   n-nCut );
    LockedView( ABL, A, mCut, 0,    m-mCut, nCut   );
    LockedView( ABR, A, mCut, nCut, m-mCut, n-nCut );
}

template<typename T, Distribution U, Distribution V,typename Int>
inline void
LockedPartitionDownOffsetDiagonal
( Int offset,
  const DM& A, DM& ATL, DM& ATR,
               DM& ABL, DM& ABR, Int diagDist )
{
#ifndef RELEASE
    CallStackEntry entry("LockedPartitionDownOffsetDiagonal [DistMatrix]");
#endif
    const int m = A.Height();
    const int n = A.Width();
    const Int diagLength = A.DiagonalLength(offset);
    diagDist = std::max(std::min(diagDist,diagLength),0);
    
    const int mCut = ( offset<=0 ? -offset+diagDist : diagDist );
    const int nCut = ( offset<=0 ? diagDist : offset+diagDist );
    LockedView( ATL, A, 0,    0,    mCut,   nCut   );
    LockedView( ATR, A, 0,    nCut, mCut,   n-nCut );
    LockedView( ABL, A, mCut, 0,    m-mCut, nCut   );
    LockedView( ABR, A, mCut, nCut, m-mCut, n-nCut );
}

#undef DM
#undef M

} // namespace elem

#endif // ifndef CORE_PARTITION_IMPL_HPP
