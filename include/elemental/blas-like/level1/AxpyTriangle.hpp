/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef BLAS_AXPYTRIANGLE_HPP
#define BLAS_AXPYTRIANGLE_HPP

namespace elem {

template<typename T>
inline void
AxpyTriangle( UpperOrLower uplo, T alpha, const Matrix<T>& X, Matrix<T>& Y )
{
#ifndef RELEASE
    CallStackEntry entry("AxpyTriangle");
    if( X.Height() != X.Width() || Y.Height() != Y.Width() || 
        X.Height() != Y.Height() )
        throw std::logic_error("Nonconformal AxpyTriangle");
#endif
    if( uplo == UPPER )
    {
        for( int j=0; j<X.Width(); ++j )
            blas::Axpy( j+1, alpha, X.LockedBuffer(0,j), 1, Y.Buffer(0,j), 1 );
    }
    else
    {
        const int n = X.Height();
        for( int j=0; j<X.Width(); ++j )
            blas::Axpy( n-j, alpha, X.LockedBuffer(j,j), 1, Y.Buffer(j,j), 1 );
    }
}

#ifndef SWIG
template<typename T>
inline void
AxpyTriangle
( UpperOrLower uplo, BASE(T) alpha, 
  const Matrix<T>& X, Matrix<T>& Y )
{ AxpyTriangle( uplo, T(alpha), X, Y ); }
#endif

template<typename T,Distribution U,Distribution V>
inline void
AxpyTriangle
( UpperOrLower uplo, T alpha, const DistMatrix<T,U,V>& X, DistMatrix<T,U,V>& Y )
{
#ifndef RELEASE
    CallStackEntry entry("AxpyTriangle");
    if( X.Grid() != Y.Grid() )
        throw std::logic_error
        ("X and Y must be distributed over the same grid");
    if( X.Height() != X.Width() || Y.Height() != Y.Width() || 
        X.Height() != Y.Height() )
        throw std::logic_error("Nonconformal AxpyTriangle");
#endif
    if( X.ColAlignment() == Y.ColAlignment() &&
        X.RowAlignment() == Y.RowAlignment() )
    {
        const int localHeight = X.LocalHeight();
        const int localWidth = X.LocalWidth();
        const int colShift = X.ColShift();
        const int rowShift = X.RowShift();
        const int colStride = X.ColStride();
        const int rowStride = X.RowStride();
        const T* XBuffer = X.LockedBuffer();
        T* YBuffer = Y.Buffer();
        const int XLDim = X.LDim();
        const int YLDim = Y.LDim();
        if( uplo == UPPER )
        {
            for( int jLoc=0; jLoc<localWidth; ++jLoc )
            {
                const int j = rowShift + jLoc*rowStride;        
                const int localHeightAbove = Length( j+1, colShift, colStride );
                blas::Axpy
                ( localHeightAbove, alpha, 
                  &XBuffer[jLoc*XLDim], 1, &YBuffer[jLoc*YLDim], 1 );
            }
        }
        else
        {
            for( int jLoc=0; jLoc<localWidth; ++jLoc )
            {
                const int j = rowShift + jLoc*rowStride;
                const int localHeightAbove = Length( j, colShift, colStride );
                const int localHeightBelow = localHeight - localHeightAbove;
                blas::Axpy
                ( localHeightBelow, alpha, 
                  &XBuffer[localHeightAbove+jLoc*XLDim], 1,
                  &YBuffer[localHeightAbove+jLoc*YLDim], 1 );
            }
        }
    }
    else
    {
        DistMatrix<T,U,V> XCopy( X.Grid() );
        XCopy.AlignWith( Y );
        XCopy = X;
        AxpyTriangle( uplo, alpha, XCopy.LockedMatrix(), Y.Matrix() );
    }
}

#ifndef SWIG
template<typename T,Distribution U,Distribution V>
inline void
AxpyTriangle
( UpperOrLower uplo, BASE(T) alpha,
  const DistMatrix<T,U,V>& X, DistMatrix<T,U,V>& Y )
{ AxpyTriangle( uplo, T(alpha), X, Y ); }
#endif

} // namespace elem

#endif // ifndef BLAS_AXPYTRIANGLE_HPP
