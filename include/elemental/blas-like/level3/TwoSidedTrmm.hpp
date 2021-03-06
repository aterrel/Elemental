/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef BLAS_TWOSIDEDTRMM_HPP
#define BLAS_TWOSIDEDTRMM_HPP

namespace elem {
namespace internal {

template<typename T>
inline void 
TwoSidedTrmmLUnb( UnitOrNonUnit diag, Matrix<T>& A, const Matrix<T>& L )
{
#ifndef RELEASE
    CallStackEntry entry("internal::TwoSidedTrmmLUnb");
#endif
    // Use the Variant 4 algorithm
    // (which annoyingly requires conjugations for the Her2)
    const int n = A.Height();
    const int lda = A.LDim();
    const int ldl = L.LDim();
    T* ABuffer = A.Buffer();
    const T* LBuffer = L.LockedBuffer();
    std::vector<T> a10Conj( n ), l10Conj( n );
    for( int j=0; j<n; ++j )
    {
        const int a21Height = n - (j+1);

        // Extract and store the diagonal values of A and L
        const T alpha11 = ABuffer[j+j*lda];
        const T lambda11 = ( diag==UNIT ? 1 : LBuffer[j+j*ldl] );

        // a10 := a10 + (alpha11/2)l10
        T* a10 = &ABuffer[j];
        const T* l10 = &LBuffer[j];
        for( int k=0; k<j; ++k )
            a10[k*lda] += (alpha11/2)*l10[k*ldl];

        // A00 := A00 + (a10' l10 + l10' a10)
        T* A00 = ABuffer;
        for( int k=0; k<j; ++k )
            a10Conj[k] = Conj(a10[k*lda]);
        for( int k=0; k<j; ++k )
            l10Conj[k] = Conj(l10[k*ldl]);
        blas::Her2( 'L', j, T(1), &a10Conj[0], 1, &l10Conj[0], 1, A00, lda );

        // a10 := a10 + (alpha11/2)l10
        for( int k=0; k<j; ++k )
            a10[k*lda] += (alpha11/2)*l10[k*ldl];

        // a10 := conj(lambda11) a10
        if( diag != UNIT )
            for( int k=0; k<j; ++k )
                a10[k*lda] *= Conj(lambda11);

        // alpha11 := alpha11 * |lambda11|^2
        ABuffer[j+j*lda] *= Conj(lambda11)*lambda11;

        // A20 := A20 + a21 l10
        T* a21 = &ABuffer[(j+1)+j*lda];
        T* A20 = &ABuffer[j+1];
        blas::Geru( a21Height, j, T(1), a21, 1, l10, ldl, A20, lda );

        // a21 := lambda11 a21
        if( diag != UNIT )
            for( int k=0; k<a21Height; ++k )
                a21[k] *= lambda11;
    }
}

template<typename T>
inline void 
TwoSidedTrmmUUnb( UnitOrNonUnit diag, Matrix<T>& A, const Matrix<T>& U )
{
#ifndef RELEASE
    CallStackEntry entry("internal::TwoSidedTrmmUUnb");
#endif
    // Use the Variant 4 algorithm
    const int n = A.Height();
    const int lda = A.LDim();
    const int ldu = U.LDim();
    T* ABuffer = A.Buffer();
    const T* UBuffer = U.LockedBuffer();
    for( int j=0; j<n; ++j )
    {
        const int a21Height = n - (j+1);

        // Extract and store the diagonal values of A and U
        const T alpha11 = ABuffer[j+j*lda];
        const T upsilon11 = ( diag==UNIT ? 1 : UBuffer[j+j*ldu] );

        // a01 := a01 + (alpha11/2)u01
        T* a01 = &ABuffer[j*lda];
        const T* u01 = &UBuffer[j*ldu];
        for( int k=0; k<j; ++k )
            a01[k] += (alpha11/2)*u01[k];

        // A00 := A00 + (u01 a01' + a01 u01')
        T* A00 = ABuffer;
        blas::Her2( 'U', j, T(1), u01, 1, a01, 1, A00, lda );

        // a01 := a01 + (alpha11/2)u01
        for( int k=0; k<j; ++k )
            a01[k] += (alpha11/2)*u01[k];

        // a01 := conj(upsilon11) a01
        if( diag != UNIT )
            for( int k=0; k<j; ++k )
                a01[k] *= Conj(upsilon11);

        // A02 := A02 + u01 a12
        T* a12 = &ABuffer[j+(j+1)*lda];
        T* A02 = &ABuffer[(j+1)*lda];
        blas::Geru( j, a21Height, T(1), u01, 1, a12, lda, A02, lda );

        // alpha11 := alpha11 * |upsilon11|^2
        ABuffer[j+j*lda] *= Conj(upsilon11)*upsilon11;

        // a12 := upsilon11 a12
        if( diag != UNIT )
            for( int k=0; k<a21Height; ++k )
                a12[k*lda] *= upsilon11;
    }
}

} // namespace internal

template<typename T>
inline void
LocalTwoSidedTrmm
( UpperOrLower uplo, UnitOrNonUnit diag,
  DistMatrix<T,STAR,STAR>& A, const DistMatrix<T,STAR,STAR>& B )
{
#ifndef RELEASE
    CallStackEntry entry("LocalTwoSidedTrmm");
#endif
    TwoSidedTrmm( uplo, diag, A.Matrix(), B.LockedMatrix() );
}

} // namespace elem

#include "./TwoSidedTrmm/LVar4.hpp"
#include "./TwoSidedTrmm/UVar4.hpp"

namespace elem {

template<typename T> 
inline void
TwoSidedTrmm
( UpperOrLower uplo, UnitOrNonUnit diag, Matrix<T>& A, const Matrix<T>& B )
{
#ifndef RELEASE
    CallStackEntry entry("TwoSidedTrmm");
#endif
    if( uplo == LOWER )
        internal::TwoSidedTrmmLVar4( diag, A, B );
    else
        internal::TwoSidedTrmmUVar4( diag, A, B );
}

template<typename T> 
inline void
TwoSidedTrmm
( UpperOrLower uplo, UnitOrNonUnit diag, 
  DistMatrix<T>& A, const DistMatrix<T>& B )
{
#ifndef RELEASE
    CallStackEntry entry("TwoSidedTrmm");
#endif
    if( uplo == LOWER )
        internal::TwoSidedTrmmLVar4( diag, A, B );
    else
        internal::TwoSidedTrmmUVar4( diag, A, B );
}

} // namespace elem

#endif // ifndef BLAS_TWOSIDEDTRMM_HPP
