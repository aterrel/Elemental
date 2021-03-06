/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef BLAS_MAKETRAPEZOIDAL_HPP
#define BLAS_MAKETRAPEZOIDAL_HPP

namespace elem {

template<typename T>
inline void
MakeTrapezoidal
( UpperOrLower uplo, Matrix<T>& A, int offset=0, LeftOrRight side=LEFT )
{
#ifndef RELEASE
    CallStackEntry entry("MakeTrapezoidal");
#endif
    const int height = A.Height();
    const int width = A.Width();
    const int ldim = A.LDim();
    T* buffer = A.Buffer();

    if( uplo == LOWER )
    {
        if( side == LEFT )
        {
#ifdef HAVE_OPENMP
#pragma omp parallel for
#endif
            for( int j=std::max(0,offset+1); j<width; ++j )
            {
                const int lastZeroRow = j-offset-1;
                const int numZeroRows = std::min( lastZeroRow+1, height );
                MemZero( &buffer[j*ldim], numZeroRows );
            }
        }
        else
        {
#ifdef HAVE_OPENMP
#pragma omp parallel for
#endif
            for( int j=std::max(0,offset-height+width+1); j<width; ++j )
            {
                const int lastZeroRow = j-offset+height-width-1;
                const int numZeroRows = std::min( lastZeroRow+1, height );
                MemZero( &buffer[j*ldim], numZeroRows );
            }
        }
    }
    else
    {
        if( side == LEFT )
        {
#ifdef HAVE_OPENMP
#pragma omp parallel for
#endif
            for( int j=0; j<width; ++j )
            {
                const int firstZeroRow = std::max(j-offset+1,0);
                if( firstZeroRow < height )
                    MemZero
                    ( &buffer[firstZeroRow+j*ldim], height-firstZeroRow );
            }
        }
        else
        {
#ifdef HAVE_OPENMP
#pragma omp parallel for
#endif
            for( int j=0; j<width; ++j )
            {
                const int firstZeroRow = std::max(j-offset+height-width+1,0);
                if( firstZeroRow < height )
                    MemZero
                    ( &buffer[firstZeroRow+j*ldim], height-firstZeroRow );
            }
        }
    }
}

template<typename T,Distribution U,Distribution V>
inline void
MakeTrapezoidal
( UpperOrLower uplo, DistMatrix<T,U,V>& A, int offset=0, LeftOrRight side=LEFT )
{
#ifndef RELEASE
    CallStackEntry entry("MakeTrapezoidal");
#endif
    const int height = A.Height();
    const int width = A.Width();
    const int localHeight = A.LocalHeight();
    const int localWidth = A.LocalWidth();
    const int colShift = A.ColShift();
    const int rowShift = A.RowShift();
    const int colStride = A.ColStride();
    const int rowStride = A.RowStride();

    T* buffer = A.Buffer();
    const int ldim = A.LDim();

    if( uplo == LOWER )
    {

#ifdef HAVE_OPENMP
#pragma omp parallel for
#endif
        for( int jLoc=0; jLoc<localWidth; ++jLoc )
        {
            const int j = rowShift + jLoc*rowStride;
            const int lastZeroRow =
                ( side==LEFT ? j-offset-1
                             : j-offset+height-width-1 );
            if( lastZeroRow >= 0 )
            {
                const int boundary = std::min( lastZeroRow+1, height );
                const int numZeroRows =
                    Length_( boundary, colShift, colStride );
                MemZero( &buffer[jLoc*ldim], numZeroRows );
            }
        }
    }
    else
    {
#ifdef HAVE_OPENMP
#pragma omp parallel for
#endif
        for( int jLoc=0; jLoc<localWidth; ++jLoc )
        {
            const int j = rowShift + jLoc*rowStride;
            const int firstZeroRow =
                ( side==LEFT ? std::max(j-offset+1,0)
                             : std::max(j-offset+height-width+1,0) );
            const int numNonzeroRows =
                Length_(firstZeroRow,colShift,colStride);
            if( numNonzeroRows < localHeight )
            {
                T* col = &buffer[numNonzeroRows+jLoc*ldim];
                MemZero( col, localHeight-numNonzeroRows );
            }
        }
    }
}

} // namespace elem

#endif // ifndef BLAS_MAKETRAPEZOIDAL_HPP
