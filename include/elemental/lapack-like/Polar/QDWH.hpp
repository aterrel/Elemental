/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef LAPACK_POLAR_QDWH_HPP
#define LAPACK_POLAR_QDWH_HPP

#include "elemental/blas-like/level1/Axpy.hpp"
#include "elemental/blas-like/level1/MakeHermitian.hpp"
#include "elemental/blas-like/level1/Scale.hpp"
#include "elemental/blas-like/level3/Gemm.hpp"
#include "elemental/blas-like/level3/Herk.hpp"
#include "elemental/blas-like/level3/Trsm.hpp"
#include "elemental/lapack-like/Cholesky.hpp"
#include "elemental/lapack-like/QR.hpp"
#include "elemental/lapack-like/Norm/Frobenius.hpp"
#include "elemental/matrices/Identity.hpp"

namespace elem {

//
// Based on Yuji Nakatsukasa's implementation of a QR-based dynamically 
// weighted Halley iteration for the polar decomposition. In particular, this
// implementation mirrors the routine 'qdwh', which is part of the zip-file
// available here:
//     http://www.mathworks.com/matlabcentral/fileexchange/36830
//
// No support for column-pivoting or row-sorting yet 
// (though qr::BusingerGolub exists).
//
// The careful calculation of the coefficients is due to a suggestion from
// Gregorio Quintana Orti.
//

namespace polar {

template<typename F>
inline int 
QDWH( Matrix<F>& A, BASE(F) lowerBound, BASE(F) upperBound, int maxIts=100 )
{
#ifndef RELEASE
    CallStackEntry entry("polar::QDWH");
#endif
    typedef BASE(F) R;
    const int height = A.Height();
    const int width = A.Width();
    const R oneThird = R(1)/R(3);

    if( height < width )
        throw std::logic_error("Height cannot be less than width");

    const R epsilon = lapack::MachineEpsilon<R>();
    const R tol = 5*epsilon;
    const R cubeRootTol = Pow(tol,oneThird);

    // Form the first iterate
    Scale( 1/upperBound, A );

    R frobNormADiff;
    Matrix<F> ALast, ATemp, C;
    Matrix<F> Q( height+width, width );
    Matrix<F> QT, QB;
    PartitionDown( Q, QT,
                      QB, height );
    int numIts=0;
    while( numIts < maxIts )
    {
        ALast = A;

        R L2;
        Complex<R> dd, sqd;
        if( Abs(1-lowerBound) < tol )
        {
            L2 = 1;
            dd = 0;
            sqd = 1;
        }
        else
        {
            L2 = lowerBound*lowerBound;
            dd = Pow( 4*(1-L2)/(L2*L2), oneThird );
            sqd = Sqrt( 1+dd );
        }
        const Complex<R> arg = 8 - 4*dd + 8*(2-L2)/(L2*sqd);
        const R a = (sqd + Sqrt( arg )/2).real;
        const R b = (a-1)*(a-1)/4;
        const R c = a+b-1;
        const Complex<R> alpha = a-b/c;
        const Complex<R> beta = b/c;

        lowerBound = lowerBound*(a+b*L2)/(1+c*L2);

        if( c > 100 )
        {
            //
            // The standard QR-based algorithm
            //
            QT = A;
            Scale( Sqrt(c), QT );
            MakeIdentity( QB );
            qr::Explicit( Q );
            Gemm( NORMAL, ADJOINT, alpha/Sqrt(c), QT, QB, beta, A );
        }
        else
        {
            //
            // Use faster Cholesky-based algorithm since A is well-conditioned
            //
            Identity( C, width, width );
            Herk( LOWER, ADJOINT, F(c), A, F(1), C );
            Cholesky( LOWER, C );
            ATemp = A;
            Trsm( RIGHT, LOWER, ADJOINT, NON_UNIT, F(1), C, ATemp );
            Trsm( RIGHT, LOWER, NORMAL, NON_UNIT, F(1), C, ATemp );
            Scale( beta, A );
            Axpy( alpha, ATemp, A );
        }

        ++numIts;
        Axpy( F(-1), A, ALast );
        frobNormADiff = FrobeniusNorm( ALast );
        if( frobNormADiff <= cubeRootTol && Abs(1-lowerBound) <= tol )
            break;
    }
    return numIts;
}

template<typename F>
inline int 
QDWH
( Matrix<F>& A, Matrix<F>& P, 
  BASE(F) lowerBound, BASE(F) upperBound, int maxIts=100 )
{
#ifndef RELEASE
    CallStackEntry entry("polar::QDWH");
#endif
    Matrix<F> ACopy( A );
    const int numIts = QDWH( A, lowerBound, upperBound, maxIts );
    Gemm( NORMAL, NORMAL, F(1), A, ACopy, P );
    return numIts;
}

template<typename F>
inline int 
QDWH( DistMatrix<F>& A, BASE(F) lowerBound, BASE(F) upperBound, int maxIts=100 )
{
#ifndef RELEASE
    CallStackEntry entry("polar::QDWH");
#endif
    typedef BASE(F) R;
    const Grid& g = A.Grid();
    const int height = A.Height();
    const int width = A.Width();
    const R oneThird = R(1)/R(3);

    if( height < width )
        throw std::logic_error("Height cannot be less than width");

    const R epsilon = lapack::MachineEpsilon<R>();
    const R tol = 5*epsilon;
    const R cubeRootTol = Pow(tol,oneThird);

    // Form the first iterate
    Scale( 1/upperBound, A );

    R frobNormADiff;
    DistMatrix<F> ALast(g), ATemp(g), C(g);
    DistMatrix<F> Q( height+width, width, g );
    DistMatrix<F> QT(g), QB(g);
    PartitionDown( Q, QT,
                      QB, height );
    int numIts=0;
    while( numIts < maxIts )
    {
        ALast = A;

        R L2;
        Complex<R> dd, sqd;
        if( Abs(1-lowerBound) < tol )
        {
            L2 = 1;
            dd = 0;
            sqd = 1;
        }
        else
        {
            L2 = lowerBound*lowerBound;
            dd = Pow( 4*(1-L2)/(L2*L2), oneThird );
            sqd = Sqrt( 1+dd );
        }
        const Complex<R> arg = 8 - 4*dd + 8*(2-L2)/(L2*sqd);
        const R a = (sqd + Sqrt( arg )/2).real;
        const R b = (a-1)*(a-1)/4;
        const R c = a+b-1;
        const Complex<R> alpha = a-b/c;
        const Complex<R> beta = b/c;

        lowerBound = lowerBound*(a+b*L2)/(1+c*L2);

        if( c > 100 )
        {
            //
            // The standard QR-based algorithm
            //
            QT = A;
            Scale( Sqrt(c), QT );
            MakeIdentity( QB );
            qr::Explicit( Q );
            Gemm( NORMAL, ADJOINT, alpha/Sqrt(c), QT, QB, beta, A );
        }
        else
        {
            //
            // Use faster Cholesky-based algorithm since A is well-conditioned
            //
            Identity( C, width, width );
            Herk( LOWER, ADJOINT, F(c), A, F(1), C );
            Cholesky( LOWER, C );
            ATemp = A;
            Trsm( RIGHT, LOWER, ADJOINT, NON_UNIT, F(1), C, ATemp );
            Trsm( RIGHT, LOWER, NORMAL, NON_UNIT, F(1), C, ATemp );
            Scale( beta, A );
            Axpy( alpha, ATemp, A );
        }

        ++numIts;
        Axpy( F(-1), A, ALast );
        frobNormADiff = FrobeniusNorm( ALast );
        if( frobNormADiff <= cubeRootTol && Abs(1-lowerBound) <= tol )
            break;
    }
    return numIts;
}

template<typename F>
inline int 
QDWH
( DistMatrix<F>& A, DistMatrix<F>& P, 
  BASE(F) lowerBound, BASE(F) upperBound, int maxIts=100 )
{
#ifndef RELEASE
    CallStackEntry entry("polar::QDWH");
#endif
    DistMatrix<F> ACopy( A );
    const int numIts = QDWH( A, lowerBound, upperBound, maxIts );
    Gemm( NORMAL, NORMAL, F(1), A, ACopy, P );
    return numIts;
}

} // namespace polar

namespace hermitian_polar {

template<typename F>
inline int
QDWH
( UpperOrLower uplo, Matrix<F>& A, BASE(F) lowerBound, BASE(F) upperBound, 
  int maxIts=100 )
{
#ifndef RELEASE
    CallStackEntry entry("hermitian_polar::QDWH");
#endif
    if( A.Height() != A.Width() )
        throw std::logic_error("Height must be same as width");

    typedef BASE(F) R;
    const int height = A.Height();
    const R oneThird = R(1)/R(3);

    const R epsilon = lapack::MachineEpsilon<R>();
    const R tol = 5*epsilon;
    const R cubeRootTol = Pow(tol,oneThird);

    // Form the first iterate
    Scale( 1/upperBound, A );

    R frobNormADiff;
    Matrix<F> ALast, ATemp, C;
    Matrix<F> Q( 2*height, height );
    Matrix<F> QT, QB;
    PartitionDown( Q, QT,
                      QB, height );
    int it=0;
    while( it < maxIts )
    {
        ALast = A;

        R L2;
        Complex<R> dd, sqd;
        if( Abs(1-lowerBound) < tol )
        {
            L2 = 1;
            dd = 0;
            sqd = 1;
        }
        else
        {
            L2 = lowerBound*lowerBound;
            dd = Pow( 4*(1-L2)/(L2*L2), oneThird );
            sqd = Sqrt( 1+dd );
        }
        const Complex<R> arg = 8 - 4*dd + 8*(2-L2)/(L2*sqd);
        const R a = (sqd + Sqrt( arg )/2).real;
        const R b = (a-1)*(a-1)/4;
        const R c = a+b-1;
        const Complex<R> alpha = a-b/c;
        const Complex<R> beta = b/c;

        lowerBound = lowerBound*(a+b*L2)/(1+c*L2);

        if( c > 100 )
        {
            //
            // The standard QR-based algorithm
            //
            MakeHermitian( uplo, A );
            QT = A;
            Scale( Sqrt(c), QT );
            MakeIdentity( QB );
            qr::Explicit( Q );
            Trrk( uplo, NORMAL, ADJOINT, alpha/Sqrt(c), QT, QB, beta, A );
        }
        else
        {
            //
            // Use faster Cholesky-based algorithm since A is well-conditioned
            //
            // TODO: Think of how to better exploit the symmetry of A,
            //       e.g., by halving the work in the first Herk through 
            //       a custom routine for forming L^2, where L is strictly lower
            MakeHermitian( uplo, A );
            Identity( C, height, height );
            Herk( LOWER, ADJOINT, F(c), A, F(1), C );
            Cholesky( LOWER, C );
            ATemp = A;
            Trsm( RIGHT, LOWER, ADJOINT, NON_UNIT, F(1), C, ATemp );
            Trsm( RIGHT, LOWER, NORMAL, NON_UNIT, F(1), C, ATemp );
            Scale( beta, A );
            Axpy( alpha, ATemp, A );
        }

        Axpy( F(-1), A, ALast );
        frobNormADiff = HermitianFrobeniusNorm( uplo, ALast );

        ++it;
        if( frobNormADiff <= cubeRootTol && Abs(1-lowerBound) <= tol )
            break;
    }

    MakeHermitian( uplo, A );
    return it;
}

template<typename F>
inline int
QDWH
( UpperOrLower uplo, DistMatrix<F>& A, BASE(F) lowerBound, BASE(F) upperBound,
  int maxIts=100 )
{
#ifndef RELEASE
    CallStackEntry entry("hermitian_polar::QDWH");
#endif
    if( A.Height() != A.Width() )
        throw std::logic_error("Height must be same as width");

    typedef BASE(F) R;
    const Grid& g = A.Grid();
    const int height = A.Height();
    const R oneThird = R(1)/R(3);

    const R epsilon = lapack::MachineEpsilon<R>();
    const R tol = 5*epsilon;
    const R cubeRootTol = Pow(tol,oneThird);

    // Form the first iterate
    Scale( 1/upperBound, A );

    R frobNormADiff;
    DistMatrix<F> ALast(g), ATemp(g), C(g);
    DistMatrix<F> Q( 2*height, height, g );
    DistMatrix<F> QT(g), QB(g);
    PartitionDown( Q, QT,
                      QB, height );
    int numIts=0;
    while( numIts < maxIts )
    {
        ALast = A;

        R L2;
        Complex<R> dd, sqd;
        if( Abs(1-lowerBound) < tol )
        {
            L2 = 1;
            dd = 0;
            sqd = 1;
        }
        else
        {
            L2 = lowerBound*lowerBound;
            dd = Pow( 4*(1-L2)/(L2*L2), oneThird );
            sqd = Sqrt( 1+dd );
        }
        const Complex<R> arg = 8 - 4*dd + 8*(2-L2)/(L2*sqd);
        const R a = (sqd + Sqrt( arg )/2).real;
        const R b = (a-1)*(a-1)/4;
        const R c = a+b-1;
        const Complex<R> alpha = a-b/c;
        const Complex<R> beta = b/c;

        lowerBound = lowerBound*(a+b*L2)/(1+c*L2);

        if( c > 100 )
        {
            //
            // The standard QR-based algorithm
            //
            MakeHermitian( uplo, A );
            QT = A;
            Scale( Sqrt(c), QT );
            MakeIdentity( QB );
            qr::Explicit( Q );
            Trrk( uplo, NORMAL, ADJOINT, alpha/Sqrt(c), QT, QB, beta, A );
        }
        else
        {
            //
            // Use faster Cholesky-based algorithm since A is well-conditioned
            //
            // TODO: Think of how to better exploit the symmetry of A,
            //       e.g., by halving the work in the first Herk through 
            //       a custom routine for forming L^2, where L is strictly lower
            MakeHermitian( uplo, A );
            Identity( C, height, height );
            Herk( LOWER, ADJOINT, F(c), A, F(1), C );
            Cholesky( LOWER, C );
            ATemp = A;
            Trsm( RIGHT, LOWER, ADJOINT, NON_UNIT, F(1), C, ATemp );
            Trsm( RIGHT, LOWER, NORMAL, NON_UNIT, F(1), C, ATemp );
            Scale( beta, A );
            Axpy( alpha, ATemp, A );
        }

        ++numIts;
        Axpy( F(-1), A, ALast );
        frobNormADiff = HermitianFrobeniusNorm( uplo, ALast );
        if( frobNormADiff <= cubeRootTol && Abs(1-lowerBound) <= tol )
            break;
    }
    MakeHermitian( uplo, A );
    return numIts;
}

} // namespace hermitian_polar

} // namespace elem

#endif // ifndef LAPACK_POLAR_QDWH_HPP
