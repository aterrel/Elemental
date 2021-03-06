/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef LAPACK_NORM_TWO_HPP
#define LAPACK_NORM_TWO_HPP

#include "elemental/blas-like/level1/MakeSymmetric.hpp"
#include "elemental/lapack-like/Norm/Infinity.hpp"
#include "elemental/lapack-like/Norm/Max.hpp"
#include "elemental/lapack-like/SVD.hpp"

namespace elem {

template<typename F> 
inline BASE(F)
TwoNorm( const Matrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("TwoNorm");
#endif
    typedef BASE(F) R;
    Matrix<F> B( A );
    Matrix<R> s;
    SVD( B, s );
    return InfinityNorm( s );
}

template<typename F>
inline BASE(F)
HermitianTwoNorm( UpperOrLower uplo, const Matrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("HermitianTwoNorm");
#endif
    typedef BASE(F) R;
    Matrix<F> B( A );
    Matrix<R> s;
    HermitianSVD( uplo, B, s );
    return InfinityNorm( s );
}

template<typename F>
inline BASE(F)
SymmetricTwoNorm( UpperOrLower uplo, const Matrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("SymmetricTwoNorm");
#endif
    typedef BASE(F) R;
    Matrix<F> B( A );
    Matrix<R> s;
    MakeSymmetric( uplo, B );
    SVD( B, s );
    return MaxNorm( s );
}

template<typename F,Distribution U,Distribution V> 
inline BASE(F)
TwoNorm( const DistMatrix<F,U,V>& A )
{
#ifndef RELEASE
    CallStackEntry entry("TwoNorm");
#endif
    typedef BASE(F) R;
    DistMatrix<F> B( A );
    DistMatrix<R,VR,STAR> s( A.Grid() );
    SVD( B, s );
    return InfinityNorm( s );
}

template<typename F,Distribution U,Distribution V>
inline BASE(F)
HermitianTwoNorm( UpperOrLower uplo, const DistMatrix<F,U,V>& A )
{
#ifndef RELEASE
    CallStackEntry entry("HermitianTwoNorm");
#endif
    typedef BASE(F) R;
    DistMatrix<F,U,V> B( A );
    DistMatrix<R,VR,STAR> s( A.Grid() );
    HermitianSVD( uplo, B, s );
    return InfinityNorm( s );
}

template<typename F,Distribution U,Distribution V>
inline BASE(F)
SymmetricTwoNorm( UpperOrLower uplo, const DistMatrix<F,U,V>& A )
{
#ifndef RELEASE
    CallStackEntry entry("SymmetricTwoNorm");
#endif
    typedef BASE(F) R;
    DistMatrix<F,U,V> B( A );
    DistMatrix<R,VR,STAR> s( A.Grid() );
    MakeSymmetric( uplo, B );
    SVD( B, s );
    return MaxNorm( s );
}

} // namespace elem

#endif // ifndef LAPACK_NORM_TWO_HPP
