/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef MATRICES_WALSH_HPP
#define MATRICES_WALSH_HPP

namespace elem {

template<typename T> 
inline void
MakeWalsh( Matrix<T>& A, int k, bool binary=false )
{
#ifndef RELEASE
    CallStackEntry entry("MakeWalsh");
#endif
    if( k < 1 )
        throw std::logic_error("Walsh matrices are only defined for k>=1");

    const unsigned n = 1u<<k;
    if( A.Height() != int(n) || A.Width() != int(n) )
        throw std::logic_error("Invalid input matrix size");

    // Run a simple O(n^2 log n) algorithm for computing the entries
    // based upon successive sign flips
    const T onValue = 1;
    const T offValue = ( binary ? 0 : -1 );
    for( unsigned j=0; j<n; ++j )
    {
        for( unsigned i=0; i<n; ++i )
        {
            // Recurse on the quadtree, flipping the sign of the entry each
            // time we are in the bottom-right quadrant
            unsigned r = i;     
            unsigned s = j;
            unsigned t = n;
            bool on = true;
            while( t != 1u )
            {
                t >>= 1;
                if( r >= t && s >= t )
                    on = !on;
                r %= t;
                s %= t;
            }

            if( on )
                A.Set( i, j, onValue );
            else
                A.Set( i, j, offValue );
        }
    }
}

template<typename T,Distribution U,Distribution V>
inline void
MakeWalsh( DistMatrix<T,U,V>& A, int k, bool binary=false )
{
#ifndef RELEASE
    CallStackEntry entry("MakeWalsh");
#endif
    if( k < 1 )
        throw std::logic_error("Walsh matrices are only defined for k>=1");

    const unsigned n = 1u<<k;
    if( A.Height() != int(n) || A.Width() != int(n) )
        throw std::logic_error("Invalid input matrix size");

    // Run an O(n^2 log n / p) algorithm based upon successive sign flips
    const T onValue = 1;
    const T offValue = ( binary ? 0 : -1 );
    const unsigned localHeight = A.LocalHeight();
    const unsigned localWidth = A.LocalWidth();
    const unsigned colShift = A.ColShift();
    const unsigned rowShift = A.RowShift();
    const unsigned colStride = A.ColStride();
    const unsigned rowStride = A.RowStride();
    for( unsigned jLoc=0; jLoc<localWidth; ++jLoc )
    {
        const unsigned j = rowShift + jLoc*rowStride;
        for( unsigned iLoc=0; iLoc<localHeight; ++iLoc )
        {
            const unsigned i = colShift + iLoc*colStride;

            // Recurse on the quadtree, flipping the sign of the entry each
            // time we are in the bottom-right quadrant
            unsigned r = i;     
            unsigned s = j;
            unsigned t = n;
            bool on = true;
            while( t != 1u )
            {
                t >>= 1;
                if( r >= t && s >= t )
                    on = !on;
                r %= t;
                s %= t;
            }
            if( on )
                A.SetLocal( iLoc, jLoc, onValue );
            else
                A.SetLocal( iLoc, jLoc, offValue );
        }
    }
}

template<typename T> 
inline void
Walsh( Matrix<T>& A, int k, bool binary=false )
{
#ifndef RELEASE
    CallStackEntry entry("Walsh");
#endif
    if( k < 1 )
        throw std::logic_error("Walsh matrices are only defined for k>=1");
    const unsigned n = 1u<<k;
    A.ResizeTo( n, n );
    MakeWalsh( A, k, binary );
}

template<typename T,Distribution U,Distribution V>
inline void
Walsh( DistMatrix<T,U,V>& A, int k, bool binary=false )
{
#ifndef RELEASE
    CallStackEntry entry("Walsh");
#endif
    if( k < 1 )
        throw std::logic_error("Walsh matrices are only defined for k>=1");
    const unsigned n = 1u<<k;
    A.ResizeTo( n, n );
    MakeWalsh( A, k, binary );
}

} // namespace elem

#endif // ifndef MATRICES_WALSH_HPP
