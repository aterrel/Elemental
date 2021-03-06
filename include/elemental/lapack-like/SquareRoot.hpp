/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef LAPACK_SQUAREROOT_HPP
#define LAPACK_SQUAREROOT_HPP

#include "elemental/blas-like/level1/Axpy.hpp"
#include "elemental/lapack-like/HermitianFunction.hpp"
#include "elemental/lapack-like/LU.hpp"
#include "elemental/lapack-like/Norm/Max.hpp"
#include "elemental/lapack-like/Norm/One.hpp"

// See Eq. 6.3 of Nicholas J. Higham and Awad H. Al-Mohy's "Computing Matrix
// Functions", which is currently available at:
// http://eprints.ma.man.ac.uk/1451/01/covered/MIMS_ep2010_18.pdf
//
// TODO: Determine whether stopping criterion should be different than that of
//       Sign

namespace elem {
namespace square_root {

template<typename F>
inline void
NewtonStep
( const Matrix<F>& A, const Matrix<F>& X, Matrix<F>& XNew, Matrix<F>& XTmp )
{
#ifndef RELEASE
    CallStackEntry entry("square_root::NewtonStep");
#endif
    // XNew := inv(X) A
    XTmp = X;
    Matrix<int> p;
    LU( XTmp, p );
    XNew = A;
    lu::SolveAfter( NORMAL, XTmp, p, XNew );

    // XNew := 1/2 ( X + XNew )
    typedef BASE(F) R;
    Axpy( R(1)/R(2), X, XNew );
}

template<typename F>
inline void
NewtonStep
( const DistMatrix<F>& A, const DistMatrix<F>& X, 
  DistMatrix<F>& XNew, DistMatrix<F>& XTmp )
{
#ifndef RELEASE
    CallStackEntry entry("square_root::NewtonStep");
#endif
    // XNew := inv(X) A
    XTmp = X;
    Matrix<int> p;
    LU( XTmp, p );
    XNew = A;
    lu::SolveAfter( NORMAL, XTmp, p, XNew );

    // XNew := 1/2 ( X + XNew )
    typedef BASE(F) R;
    Axpy( R(1)/R(2), X, XNew );
}

template<typename F>
inline int
Newton( Matrix<F>& A, int maxIts=100, BASE(F) tol=0 )
{
#ifndef RELEASE
    CallStackEntry entry("square_root::Newton");
#endif
    typedef BASE(F) R;
    Matrix<F> B(A), C, XTmp;
    Matrix<F> *X=&B, *XNew=&C;

    if( tol == R(0) )
        tol = A.Height()*lapack::MachineEpsilon<R>();

    int numIts=0;
    while( numIts < maxIts )
    {
        // Overwrite XNew with the new iterate
        NewtonStep( A, *X, *XNew, XTmp );

        // Use the difference in the iterates to test for convergence
        Axpy( R(-1), *XNew, *X );
        const R oneDiff = OneNorm( *X );
        const R oneNew = OneNorm( *XNew );

        // Ensure that X holds the current iterate and break if possible
        ++numIts;
        std::swap( X, XNew );
        if( oneDiff/oneNew <= tol )
            break;
    }
    A = *X;
    return numIts;
}

template<typename F>
inline int
Newton( DistMatrix<F>& A, int maxIts=100, BASE(F) tol=0 )
{
#ifndef RELEASE
    CallStackEntry entry("square_root::Newton");
#endif
    typedef BASE(F) R;
    DistMatrix<F> B(A), C(A.Grid()), XTmp(A.Grid());
    DistMatrix<F> *X=&B, *XNew=&C;

    if( tol == R(0) )
        tol = A.Height()*lapack::MachineEpsilon<R>();

    int numIts=0;
    while( numIts < maxIts )
    {
        // Overwrite XNew with the new iterate
        NewtonStep( A, *X, *XNew, XTmp );

        // Use the difference in the iterates to test for convergence
        Axpy( R(-1), *XNew, *X );
        const R oneDiff = OneNorm( *X );
        const R oneNew = OneNorm( *XNew );

        // Ensure that X holds the current iterate and break if possible
        ++numIts;
        std::swap( X, XNew );
        if( oneDiff/oneNew < tol )
            break;
    }
    A = *X;
    return numIts;
}

} // namespace square_root

template<typename F>
inline void
SquareRoot( Matrix<F>& A )
{
#ifndef RELEASE
    PushCallStack("SquareRoot");
#endif
    square_root::Newton( A );
}

template<typename F>
inline void
SquareRoot( DistMatrix<F>& A )
{
#ifndef RELEASE
    PushCallStack("SquareRoot");
#endif
    square_root::Newton( A );
}

//
// Square-root the eigenvalues of A
//

template<typename F>
inline void
HPSDSquareRoot( UpperOrLower uplo, Matrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("HPSDSquareRoot");
#endif
    typedef BASE(F) R;

    // Get the EVD of A
    Matrix<R> w;
    Matrix<F> Z;
    HermitianEig( uplo, A, w, Z );

    // Compute the two-norm of A as the maximum absolute value of the eigvals
    const R twoNorm = MaxNorm( w );

    // Compute the smallest eigenvalue of A
    R minEig = twoNorm;
    const int n = w.Height();
    for( int i=0; i<n; ++i )
    {
        const R omega = w.Get(i,0);
        minEig = std::min(minEig,omega);
    }

    // Set the tolerance equal to n ||A||_2 eps
    const R eps = lapack::MachineEpsilon<R>();
    const R tolerance = n*twoNorm*eps;

    // Ensure that the minimum eigenvalue is not less than - n ||A||_2 eps
    if( minEig < -tolerance )
        throw NonHPSDMatrixException();

    // Overwrite the eigenvalues with f(w)
    for( int i=0; i<n; ++i )
    {
        const R omega = w.Get(i,0);
        if( omega > R(0) )
            w.Set(i,0,Sqrt(omega));
        else
            w.Set(i,0,0);
    }

    // Form the pseudoinverse
    HermitianFromEVD( uplo, A, w, Z );
}

template<typename F>
inline void
HPSDSquareRoot( UpperOrLower uplo, DistMatrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("HPSDSquareRoot");
#endif
    EnsurePMRRR();
    typedef BASE(F) R;

    // Get the EVD of A
    const Grid& g = A.Grid();
    DistMatrix<R,VR,STAR> w(g);
    DistMatrix<F> Z(g);
    HermitianEig( uplo, A, w, Z );

    // Compute the two-norm of A as the maximum absolute value of the eigvals
    const R twoNorm = MaxNorm( w );

    // Compute the smallest eigenvalue of A
    R minLocalEig = twoNorm;
    const int numLocalEigs = w.LocalHeight();
    for( int iLoc=0; iLoc<numLocalEigs; ++iLoc )
    {
        const R omega = w.GetLocal(iLoc,0);
        minLocalEig = std::min(minLocalEig,omega);
    }
    R minEig;
    mpi::AllReduce( &minLocalEig, &minEig, 1, mpi::MIN, g.VCComm() );

    // Set the tolerance equal to n ||A||_2 eps
    const int n = A.Height();
    const R eps = lapack::MachineEpsilon<R>();
    const R tolerance = n*twoNorm*eps;

    // Ensure that the minimum eigenvalue is not less than - n ||A||_2 eps
    if( minEig < -tolerance )
        throw NonHPSDMatrixException();

    // Overwrite the eigenvalues with f(w)
    for( int iLoc=0; iLoc<numLocalEigs; ++iLoc )
    {
        const R omega = w.GetLocal(iLoc,0);
        if( omega > R(0) )
            w.SetLocal(iLoc,0,Sqrt(omega));
        else
            w.SetLocal(iLoc,0,0);
    }

    // Form the pseudoinverse
    HermitianFromEVD( uplo, A, w, Z );
}

} // namespace elem

#endif // ifndef LAPACK_SQUAREROOT_HPP
