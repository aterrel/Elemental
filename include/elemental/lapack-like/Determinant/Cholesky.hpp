/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef LAPACK_HPDDETERMINANT_CHOLESKY_HPP
#define LAPACK_HPDDETERMINANT_CHOLESKY_HPP

#include "elemental/lapack-like/Cholesky.hpp"

namespace elem {
namespace hpd_determinant {

template<typename F>
inline SafeProduct<F> 
AfterCholesky( UpperOrLower uplo, const Matrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("hpd_determinant::AfterCholesky");
#endif
    typedef BASE(F) R;
    const int n = A.Height();

    Matrix<F> d;
    A.GetDiagonal( d );
    SafeProduct<F> det( n );
    det.rho = F(1);

    const R scale = R(n)/R(2);
    for( int i=0; i<n; ++i )
    {
        const R delta = RealPart(d.Get(i,0));
        det.kappa += Log(delta)/scale;
    }

    return det;
}

template<typename F>
inline SafeProduct<F> 
Cholesky( UpperOrLower uplo, Matrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("hpd_determinant::Cholesky");
#endif
    SafeProduct<F> det( A.Height() );
    try
    {
        elem::Cholesky( uplo, A );
        det = hpd_determinant::AfterCholesky( uplo, A );
    }
    catch( NonHPDMatrixException& e )
    {
        det.rho = 0;
        det.kappa = 0;
    }
    return det;
}

template<typename F> 
inline SafeProduct<F> 
AfterCholesky( UpperOrLower uplo, const DistMatrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("hpd_determinant::AfterCholesky");
#endif
    typedef BASE(F) R;
    const int n = A.Height();
    const Grid& g = A.Grid();

    DistMatrix<F,MD,STAR> d(g);
    A.GetDiagonal( d );
    R localKappa = 0; 
    if( d.Participating() )
    {
        const R scale = R(n)/R(2);
        const int nLocalDiag = d.LocalHeight();
        for( int iLoc=0; iLoc<nLocalDiag; ++iLoc )
        {
            const R delta = RealPart(d.GetLocal(iLoc,0));
            localKappa += Log(delta)/scale;
        }
    }
    SafeProduct<F> det( n );
    mpi::AllReduce( &localKappa, &det.kappa, 1, mpi::SUM, g.VCComm() );
    det.rho = F(1);

    return det;
}

template<typename F> 
inline SafeProduct<F> 
Cholesky( UpperOrLower uplo, DistMatrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("hpd_determinant::Cholesky");
#endif
    SafeProduct<F> det( A.Height() );
    try
    {
        elem::Cholesky( uplo, A );
        det = hpd_determinant::AfterCholesky( uplo, A );
    }
    catch( NonHPDMatrixException& e )
    {
        det.rho = 0;
        det.kappa = 0;
    }
    return det;
}

} // namespace hpd_determinant
} // namespace elem

#endif // ifndef LAPACK_HPDDETERMINANT_CHOLESKY_HPP
