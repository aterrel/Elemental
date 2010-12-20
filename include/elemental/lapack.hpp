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
#ifndef ELEMENTAL_LAPACK_HPP
#define ELEMENTAL_LAPACK_HPP 1

#include "elemental/blas.hpp"

namespace elemental {
namespace lapack {

//----------------------------------------------------------------------------//
// Chol:                                                                      //
//                                                                            //
// Overwrite a triangle of A with the Cholesky factor of A. 'shape'           //
// determines whether it is the upper or lower triangle.                      //
//----------------------------------------------------------------------------//

// Serial version
template<typename T>
void
Chol
( Shape shape, Matrix<T>& A );

// Parallel version
template<typename T>
void
Chol
( Shape shape, DistMatrix<T,MC,MR>& A );

//----------------------------------------------------------------------------//
// GaussElim (Gaussian Elimination):                                          //
//                                                                            //
// Uses an LU factorization with partial pivoting to overwrite B := A^-1 B    //
//----------------------------------------------------------------------------//

// TODO: Add a serial version

// Parallel version
template<typename T>
void
GaussElim
( DistMatrix<T,MC,MR>& A, DistMatrix<T,MC,MR>& B );

//----------------------------------------------------------------------------//
// GeneralizedHermitianEig (Hermitian Eigensolver)                            //
//                                                                            //
//----------------------------------------------------------------------------//

#ifndef WITHOUT_PMRRR

enum GenEigType { AXBX=1, ABX=2, BAX=3 };

// Grab the full set of eigenpairs of real symmetric A and SPD B
template<typename R>
void
GeneralizedHermitianEig
( GenEigType genEigType, Shape shape, 
  DistMatrix<R,MC,  MR>& A, 
  DistMatrix<R,MC,  MR>& B, 
  DistMatrix<R,Star,VR>& w,
  DistMatrix<R,MC,  MR>& X,
  bool tryForHighAccuracy = false );
// Grab a partial set of eigenpairs. 
// The partial set is determined by the inclusive zero-indexed range 
//   a,a+1,...,b    ; a >= 0, b < n  
// of the n eigenpairs sorted from smallest to largest eigenvalues.  
template<typename R>
void
GeneralizedHermitianEig
( GenEigType genEigType, Shape shape,
  DistMatrix<R,MC,  MR>& A,
  DistMatrix<R,MC,  MR>& B,
  DistMatrix<R,Star,VR>& w,
  DistMatrix<R,MC,  MR>& X,
  int a, int b, bool tryForHighAccuracy = false );
// Grab a partial set of eigenpairs.
// The partial set is determined by the half-open interval (a,b]
template<typename R>
void
GeneralizedHermitianEig
( GenEigType genEigType, Shape shape,
  DistMatrix<R,MC,  MR>& A,
  DistMatrix<R,MC,  MR>& B,
  DistMatrix<R,Star,VR>& w,
  DistMatrix<R,MC,  MR>& X,
  R a, R b, bool tryForHighAccuracy = false );
// Grab the full set of eigenvalues of real symmetric A and SPD B
template<typename R>
void
GeneralizedHermitianEig
( GenEigType genEigType, Shape shape, 
  DistMatrix<R,MC,  MR>& A, 
  DistMatrix<R,MC,  MR>& B, 
  DistMatrix<R,Star,VR>& w,
  bool tryForHighAccuracy = false );
// Grab a partial set of eigenvalues. 
// The partial set is determined by the inclusive zero-indexed range 
//   a,a+1,...,b    ; a >= 0, b < n  
// of the n eigenpairs sorted from smallest to largest eigenvalues.  
template<typename R>
void
GeneralizedHermitianEig
( GenEigType genEigType, Shape shape,
  DistMatrix<R,MC,  MR>& A,
  DistMatrix<R,MC,  MR>& B,
  DistMatrix<R,Star,VR>& w,
  int a, int b, bool tryForHighAccuracy = false );
// Grab a partial set of eigenvalues.
// The partial set is determined by the half-open interval (a,b]
template<typename R>
void
GeneralizedHermitianEig
( GenEigType genEigType, Shape shape,
  DistMatrix<R,MC,  MR>& A,
  DistMatrix<R,MC,  MR>& B,
  DistMatrix<R,Star,VR>& w,
  R a, R b, bool tryForHighAccuracy = false );

#ifndef WITHOUT_COMPLEX
// Grab the full set of eigenpairs of complex Hermitian A and HPD B
template<typename R>
void
GeneralizedHermitianEig    
( GenEigType genEigType, Shape shape,
  DistMatrix<std::complex<R>,MC,  MR>& A,
  DistMatrix<std::complex<R>,MC,  MR>& B,
  DistMatrix<             R, Star,VR>& w,
  DistMatrix<std::complex<R>,MC,  MR>& X,
  bool tryForHighAccuracy = false );
// Grab a partial set of eigenpairs. 
// The partial set is determined by the inclusive zero-indexed range 
//   a,a+1,...,b    ; a >= 0, b < n  
// of the n eigenpairs sorted from smallest to largest eigenvalues.  
template<typename R>
void
GeneralizedHermitianEig
( GenEigType genEigType, Shape shape,
  DistMatrix<std::complex<R>,MC,  MR>& A,
  DistMatrix<std::complex<R>,MC,  MR>& B,
  DistMatrix<             R, Star,VR>& w,
  DistMatrix<std::complex<R>,MC,  MR>& X,
  int a, int b, bool tryForHighAccuracy = false );
// Grab a partial set of eigenpairs.
// The partial set is determined by the half-open interval (a,b]
template<typename R>
void
GeneralizedHermitianEig
( GenEigType genEigType, Shape shape,
  DistMatrix<std::complex<R>,MC,  MR>& A,
  DistMatrix<std::complex<R>,MC,  MR>& B,
  DistMatrix<             R, Star,VR>& w,
  DistMatrix<std::complex<R>,MC,  MR>& X,
  R a, R b, bool tryForHighAccuracy = false );
// Grab the full set of eigenvalues of complex Hermitian A and HPD B
template<typename R>
void
GeneralizedHermitianEig    
( GenEigType genEigType, Shape shape,
  DistMatrix<std::complex<R>,MC,  MR>& A,
  DistMatrix<std::complex<R>,MC,  MR>& B,
  DistMatrix<             R, Star,VR>& w,
  bool tryForHighAccuracy = false );
// Grab a partial set of eigenvalues. 
// The partial set is determined by the inclusive zero-indexed range 
//   a,a+1,...,b    ; a >= 0, b < n  
// of the n eigenpairs sorted from smallest to largest eigenvalues.  
template<typename R>
void
GeneralizedHermitianEig
( GenEigType genEigType, Shape shape,
  DistMatrix<std::complex<R>,MC,  MR>& A,
  DistMatrix<std::complex<R>,MC,  MR>& B,
  DistMatrix<             R, Star,VR>& w,
  int a, int b, bool tryForHighAccuracy = false );
// Grab a partial set of eigenvalues.
// The partial set is determined by the half-open interval (a,b]
template<typename R>
void
GeneralizedHermitianEig
( GenEigType genEigType, Shape shape,
  DistMatrix<std::complex<R>,MC,  MR>& A,
  DistMatrix<std::complex<R>,MC,  MR>& B,
  DistMatrix<             R, Star,VR>& w,
  R a, R b, bool tryForHighAccuracy = false );
#endif // WITHOUT_COMPLEX
#endif // WITHOUT_PMRRR

//----------------------------------------------------------------------------//
// Hegst (HErmitian GEneralized to STandard eigenvalue problem):              //
//                                                                            //
// If side==Left,                                                             //
//   reduce the problems                                                      //
//                      A B X = X Lambda to A X = X Lambda                    //
//                      B A X = X Lambda to A X = X Lambda                    //
// If side==Right,                                                            //
//   reduce the problem A X = B X Lambda to A X = X Lambda                    //
//                                                                            //
// D contains the Cholesky factor of B in the triangle corresponding to the   //
// parameter 'shape'.                                                         //
//----------------------------------------------------------------------------//

// Serial version
template<typename T>
void
Hegst
( Side side, Shape shape, 
  Matrix<T>& A, const Matrix<T>& B );

// Parallel version
template<typename T>
void
Hegst
( Side side, Shape shape, 
  DistMatrix<T,MC,MR>& A, const DistMatrix<T,MC,MR>& B );

//----------------------------------------------------------------------------//
// HermitianEig (Hermitian Eigensolver)                                       //
//                                                                            //
//----------------------------------------------------------------------------//

#ifndef WITHOUT_PMRRR
// Grab the full set of eigenpairs of the real, symmetric matrix A
void
HermitianEig
( Shape shape, 
  DistMatrix<double,MC,  MR>& A, 
  DistMatrix<double,Star,VR>& w,
  DistMatrix<double,MC,  MR>& Z,
  bool tryForHighAccuracy = false );
// Grab a partial set of eigenpairs of the real, symmetric n x n matrix A. 
// The partial set is determined by the inclusive zero-indexed range 
//   a,a+1,...,b    ; a >= 0, b < n  
// of the n eigenpairs sorted from smallest to largest eigenvalues.  
void
HermitianEig
( Shape shape,
  DistMatrix<double,MC,  MR>& A,
  DistMatrix<double,Star,VR>& w,
  DistMatrix<double,MC,  MR>& Z,
  int a, int b, bool tryForHighAccuracy = false );
// Grab a partial set of eigenpairs of the real, symmetric n x n matrix A. 
// The partial set is determined by the half-open interval (a,b]
void
HermitianEig
( Shape shape,
  DistMatrix<double,MC,  MR>& A,
  DistMatrix<double,Star,VR>& w,
  DistMatrix<double,MC,  MR>& Z,
  double a, double b, bool tryForHighAccuracy = false );
// Grab the full set of eigenvalues of the real, symmetric matrix A
void
HermitianEig
( Shape shape,
  DistMatrix<double,MC,  MR>& A,
  DistMatrix<double,Star,VR>& w,
  bool tryForHighAccuracy = false );
// Grab a partial set of eigenvalues of the real, symmetric n x n matrix A. 
// The partial set is determined by the inclusive zero-indexed range 
//   a,a+1,...,b    ; a >= 0, b < n  
// of the n eigenpairs sorted from smallest to largest eigenvalues.  
void
HermitianEig
( Shape shape,
  DistMatrix<double,MC,  MR>& A,
  DistMatrix<double,Star,VR>& w,
  int a, int b, bool tryForHighAccuracy = false );
// Grab a partial set of eigenvalues of the real, symmetric n x n matrix A. 
// The partial set is determined by the half-open interval (a,b]
void
HermitianEig
( Shape shape,
  DistMatrix<double,MC,  MR>& A,
  DistMatrix<double,Star,VR>& w,
  double a, double b, bool tryForHighAccuracy = false );
#ifndef WITHOUT_COMPLEX
// Grab the full set of eigenpairs of the complex, Hermitian matrix A
void
HermitianEig    
( Shape shape,
  DistMatrix<std::complex<double>,MC,  MR>& A,
  DistMatrix<             double, Star,VR>& w,
  DistMatrix<std::complex<double>,MC,  MR>& Z,
  bool tryForHighAccuracy = false );
// Grab a partial set of eigenpairs of the complex, Hermitian n x n matrix A. 
// The partial set is determined by the inclusive zero-indexed range 
//   a,a+1,...,b    ; a >= 0, b < n  
// of the n eigenpairs sorted from smallest to largest eigenvalues.  
void
HermitianEig
( Shape shape,
  DistMatrix<std::complex<double>,MC,  MR>& A,
  DistMatrix<             double, Star,VR>& w,
  DistMatrix<std::complex<double>,MC,  MR>& Z,
  int a, int b, bool tryForHighAccuracy = false );
// Grab a partial set of eigenpairs of the complex, Hermitian n x n matrix A. 
// The partial set is determined by the half-open interval (a,b]
void
HermitianEig
( Shape shape,
  DistMatrix<std::complex<double>,MC,  MR>& A,
  DistMatrix<             double, Star,VR>& w,
  DistMatrix<std::complex<double>,MC,  MR>& Z,
  double a, double b, bool tryForHighAccuracy = false );
// Grab the full set of eigenvalues of the complex, Hermitian matrix A
void
HermitianEig
( Shape shape,
  DistMatrix<std::complex<double>,MC,  MR>& A,
  DistMatrix<             double, Star,VR>& w,
  bool tryForHighAccuracy = false );
// Grab a partial set of eigenvalues of the complex, Hermitian n x n matrix A. 
// The partial set is determined by the inclusive zero-indexed range 
//   a,a+1,...,b    ; a >= 0, b < n  
// of the n eigenpairs sorted from smallest to largest eigenvalues.  
void
HermitianEig
( Shape shape,
  DistMatrix<std::complex<double>,MC,  MR>& A,
  DistMatrix<             double, Star,VR>& w,
  int a, int b, bool tryForHighAccuracy = false );
// Grab a partial set of eigenvalues of the complex, Hermitian n x n matrix A. 
// The partial set is determined by the half-open interval (a,b]
void
HermitianEig
( Shape shape,
  DistMatrix<std::complex<double>,MC,  MR>& A,
  DistMatrix<             double, Star,VR>& w,
  double a, double b, bool tryForHighAccuracy = false );
#endif // WITHOUT_COMPLEX
#endif // WITHOUT_PMRRR

//----------------------------------------------------------------------------//
// LU (LU factorization with partial pivoting):                               //
//                                                                            //
// Overwrite A with its LU factorization after partial pivoting: P A = L U.   //
// P is compressed into the vector p by storing the location of the nonzero   //
// element of each row.                                                       //
//----------------------------------------------------------------------------//

template<typename T>
void
LU( Matrix<T>& A, Matrix<int>& p );

template<typename T>
void
LU( DistMatrix<T,MC,MR>& A, DistMatrix<int,VC,Star>& p );

#ifdef TIMING
namespace lu {
void PrintTimings();
}
#endif

//----------------------------------------------------------------------------//
// FrobeniusNorm                                                              //
//                                                                            //
// Computes the elementwise L2 norm of the matrix.                            //
//----------------------------------------------------------------------------//

template<typename R>
R
FrobeniusNorm( const Matrix<R>& A );

template<typename R>
R
FrobeniusNorm( const Matrix< std::complex<R> >& A );

template<typename R>
R
FrobeniusNorm( const DistMatrix<R,MC,MR>& A );

template<typename R>
R
FrobeniusNorm( const DistMatrix<std::complex<R>,MC,MR>& A );

//----------------------------------------------------------------------------//
// HermitianFrobeniusNorm                                                     //
//                                                                            //
// Computes the elementwise L2 norm of a Hermitian matrix.                    //
//----------------------------------------------------------------------------//

template<typename R>
R
HermitianFrobeniusNorm
( Shape shape, const Matrix<R>& A );

template<typename R>
R
HermitianFrobeniusNorm
( Shape shape, const Matrix< std::complex<R> >& A );

template<typename R>
R
HermitianFrobeniusNorm
( Shape shape, const DistMatrix<R,MC,MR>& A );

template<typename R>
R
HermitianFrobeniusNorm
( Shape shape, const DistMatrix<std::complex<R>,MC,MR>& A );

//----------------------------------------------------------------------------//
// InfinityNorm                                                               //
//                                                                            //
// Computes the operator L infinity norm of a matrix,                         //
//           max     ||A x||_inf                                              //
//       ||x||_inf=1                                                          //
//  which corresponds to the maximum L1 norm of the rows of A.                //
//----------------------------------------------------------------------------//

template<typename R>
R
InfinityNorm( const Matrix<R>& A );

template<typename R>
R
InfinityNorm( const Matrix< std::complex<R> >& A );

template<typename R>
R
InfinityNorm( const DistMatrix<R,MC,MR>& A );

template<typename R>
R
InfinityNorm( const DistMatrix<std::complex<R>,MC,MR>& A );

//----------------------------------------------------------------------------//
// HermitianInfinityNorm                                                      //
//                                                                            //
// Computes the operator L infinity norm of a Hermitian matrix,               //
//           max     ||A x||_inf                                              //
//       ||x||_inf=1                                                          //
//  which corresponds to the maximum L1 norm of the rows of A.                //
//                                                                            //
// Note: For Hermitian matrices, the L1 and Linf norms are identical.         //
//----------------------------------------------------------------------------//

template<typename R>
R
HermitianInfinityNorm
( Shape shape, const Matrix<R>& A );

template<typename R>
R
HermitianInfinityNorm
( Shape shape, const Matrix< std::complex<R> >& A );

template<typename R>
R
HermitianInfinityNorm
( Shape shape, const DistMatrix<R,MC,MR>& A );

template<typename R>
R
HermitianInfinityNorm
( Shape shape, const DistMatrix<std::complex<R>,MC,MR>& A );

//----------------------------------------------------------------------------//
// OneNorm                                                                    //
//                                                                            //
// Computes the operator L1 norm of a matrix,                                 //
//           max    ||A x||_1                                                 //
//       ||x||_1=1                                                            //
//  which corresponds to the maximum L1 norm of the columns of A.             //
//----------------------------------------------------------------------------//

template<typename R>
R
OneNorm( const Matrix<R>& A );

template<typename R>
R
OneNorm( const Matrix< std::complex<R> >& A );

template<typename R>
R
OneNorm( const DistMatrix<R,MC,MR>& A );

template<typename R>
R
OneNorm( const DistMatrix<std::complex<R>,MC,MR>& A );

//----------------------------------------------------------------------------//
// HermitianOneNorm                                                           //
//                                                                            //
// Computes the operator L1 norm of a Hermitian matrix,                       //
//           max    ||A x||_1                                                 //
//       ||x||_1=1                                                            //
//  which corresponds to the maximum L1 norm of the columns of A.             //
//                                                                            //
// Note: For Hermitian matrices, the L1 and Linf norms are identical.         //
//----------------------------------------------------------------------------//

template<typename R>
R
HermitianOneNorm
( Shape shape, const Matrix<R>& A );

template<typename R>
R
HermitianOneNorm
( Shape shape, const Matrix< std::complex<R> >& A );

template<typename R>
R
HermitianOneNorm
( Shape shape, const DistMatrix<R,MC,MR>& A );

template<typename R>
R
HermitianOneNorm
( Shape shape, const DistMatrix<std::complex<R>,MC,MR>& A );

//----------------------------------------------------------------------------//
// QR (QR factorization):                                                     //
//                                                                            //
// Performs a Householder QR factorization that overwrites the upper triangle //
// of A with R and fills the lower triangle with the scaled Householder       //
// transforms used to generate Q (they are implicitly one on the diagonal of  //
// A). The scaling factors for the Householder transforms are stored in t.    //
//                                                                            //
// For the complex case, 't' holds the Householder reflection coefficients    //
// that define the Householder transformation                                 //
//     House(tau,u) = I - tau u u^H                                           //
//                                                                            //
// IMPORTANT NOTE: The LAPACK convention for early-exiting when computing the //
// Householder reflection for a vector a = [ alpha11, a12 ]^T, where          //
// || a12 ||_2 = 0 and Im( alpha11 ) = 0, is to set 'tau' to zero in the      //
// Householder reflector equation:                                            //
//                                                                            //
//   House(tau,u) = I - tau u u^H                                             //
//                                                                            //
// which is not a valid Householder reflection due to the requirement that    //
// u be normalizable. We thus take the approach of setting tau = 2 when       //
// || a12 ||_2 = 0 and Im( alpha11 ) = 0, so that                             //
//                                                                            //
//   House(2,u) a = (I - 2 | 1 | | 1 0 | ) | alpha11 | = | -alpha11 |         //
//                         | 0 |           |    0    |   |     0    |         //
//                                                                            //
// This allows for the computation of the triangular matrix in the Compact WY //
// transform / UT transform to be computed mainly with Level 3 BLAS.          //
//----------------------------------------------------------------------------//

// Serial version for real datatypes
template<typename R>
void
QR( Matrix<R>& A );

#ifndef WITHOUT_COMPLEX
// Serial version for complex datatypes
template<typename R>
void
QR( Matrix< std::complex<R> >& A, Matrix< std::complex<R> >& t );
#endif

// Parallel version for real datatypes
template<typename R>
void
QR( DistMatrix<R,MC,MR>& A );

#ifndef WITHOUT_COMPLEX
// Parallel version for complex datatypes
template<typename R>
void
QR( DistMatrix<std::complex<R>,MC,  MR  >& A, 
    DistMatrix<std::complex<R>,Star,Star>& t );
#endif

//----------------------------------------------------------------------------//
// Reflector (Householder reflector):                                         //
//----------------------------------------------------------------------------//

// Serial version for real datatypes
template<typename R>
R
Reflector( Matrix<R>& chi, Matrix<R>& x );

#ifndef WITHOUT_COMPLEX
// Serial version for complex datatypes
template<typename R>
std::complex<R>
Reflector( Matrix< std::complex<R> >& chi, Matrix< std::complex<R> >& x );
#endif

// Parallel version
template<typename T>
T
Reflector( DistMatrix<T,MC,MR>& chi, DistMatrix<T,MC,MR>& x );

//----------------------------------------------------------------------------//
// SVD (Singular Value Decomposition):                                        //
//                                                                            //
// Two approaches:                                                            //
// (1)                                                                        //
//   Compute the skinny SVD of A, A = U Sigma V^T, where, if A is m x n,      //
//   U is m x min(m,n) and V^T is min(m,n) x n. We store U in A, V^T in V,    //
//   and Sigma in SigmaDiag.                                                  //
// (2)                                                                        //
//   Compute the skinny SVD of A, A = U Sigma V^T, where, if A is m x n,      //
//   U is m x min(m,n) and V^T is min(m,n) x n. We store U in U, V^T in V,    //
//   and Sigma in SigmaDiag.                                                  //
//----------------------------------------------------------------------------//

// Serial versions where A is overwritten by U
template<typename R>
void
SVD( Matrix<R>& A, Matrix<R>& VT, std::vector<R>& SigmaDiag );

#ifndef WITHOUT_COMPLEX
template<typename R>
void
SVD
( Matrix< std::complex<R> >& A, Matrix< std::complex<R> >& VT, 
  std::vector<R>& SigmaDiag );     
#endif

// Serial versions where A is not overwritten
template<typename R>
void
SVD( Matrix<R>& A, Matrix<R>& U, Matrix<R>& VT, std::vector<R>& SigmaDiag );

#ifndef WITHOUT_COMPLEX
template<typename R>
void
SVD
( Matrix< std::complex<R> >& A, Matrix< std::complex<R> >& U, 
  Matrix< std::complex<R> >& VT, std::vector<R>& SigmaDiag );
#endif

//----------------------------------------------------------------------------//
// Tridiag (Householder tridiagonalization):                                  //
//                                                                            //
// The diagonal and sub/super-diagonal of A are overwritten with a similar    //
// tridiagonal matrix that is found by successively applying Householder      //
// reflections to zero the matrix outside of the tridiagonal band.            //
//                                                                            //
// 'shape' decided which triangle of A specifies the Hermitian matrix, and on //
// exit the transforms are stored above the super/sub-diagonal and are        //
// implicitly one on the super/sub-diagonal.                                  //
//                                                                            //
// See the above note for QR factorizations detailing 't' and the difference  //
// in Householder transform early-exit approaches for the serial and parallel //
// routines.                                                                  //
//----------------------------------------------------------------------------//

// Serial version for real datatypes
template<typename R>
void
Tridiag
( Shape shape, Matrix<R>& A );

#ifndef WITHOUT_COMPLEX
// Serial version for complex datatypes
template<typename R>
void
Tridiag
( Shape shape, Matrix< std::complex<R> >& A, Matrix< std::complex<R> >& t );
#endif

// Parallel version for real datatypes
template<typename R>
void
Tridiag
( Shape shape, DistMatrix<R,MC,MR>& A );

#ifndef WITHOUT_COMPLEX
// Parallel version for complex datatypes
template<typename R>
void
Tridiag
( Shape shape,
  DistMatrix<std::complex<R>,MC,  MR  >& A,
  DistMatrix<std::complex<R>,Star,Star>& t );
#endif

//----------------------------------------------------------------------------//
// Trinv (TRiangular INVersion):                                              //
//                                                                            //
// Inverts a triangular matrix. 'shape' determines whether A is assumed to be //
// upper or lower triangular, and 'diagonal' determines whether or not A is   //
// to be treated as having a unit diagonal.                                   //
//----------------------------------------------------------------------------//

// Serial version
template<typename T>
void
Trinv
( Shape shape, Diagonal diagonal, Matrix<T>& A );

// Parallel version
template<typename T>
void
Trinv
( Shape shape, Diagonal diagonal, DistMatrix<T,MC,MR>& A  );

//----------------------------------------------------------------------------//
// UT (UT transform):                                                         //
//                                                                            //
// Applies the accumulated Householder transforms that are stored in the      //
// triangle of H specified by 'shape' to the matrix A.                        //
//                                                                            //
// If 'shape' is set to 'Lower', then offset determines the diagonal that the //
// transforms are stored above (they are implicitly one on that diagonal).    //
// Due to the conventions of the LAPACK routines 'chetrd' and 'zhetrd', the   //
// transforms are assumed to be accumulated left-to-right.                    //
//                                                                            //
// If 'shape' is set to 'Upper', then offset determines the diagonal that the //
// transforms are stored below (they are implicitly one on that diagonal).    //
// Due to the conventions of the LAPACK routines 'chetrd' and 'zhetrd', the   //
// transforms are assumed to be accumulated right-to-left.                    //
//                                                                            //
// See the above note for QR factorizations regarding the vector 't' and      //
// Householder early-exit conditions.                                         //
//----------------------------------------------------------------------------//

// TODO: Add serial versions

// Parallel version for real datatypes
template<typename R>
void
UT( Side side, Shape shape, Orientation orientation, int offset,
    const DistMatrix<R,MC,MR>& H, 
          DistMatrix<R,MC,MR>& A );

#ifndef WITHOUT_COMPLEX
template<typename R>
void
UT( Side side, Shape shape, Orientation orientation, int offset,
    const DistMatrix<std::complex<R>,MC,  MR  >& H,
    const DistMatrix<std::complex<R>,MD,  Star>& t,
          DistMatrix<std::complex<R>,MC,  MR  >& A );
template<typename R>
void
UT( Side side, Shape shape, Orientation orientation, int offset,
    const DistMatrix<std::complex<R>,MC,  MR  >& H,
    const DistMatrix<std::complex<R>,Star,Star>& t,
          DistMatrix<std::complex<R>,MC,  MR  >& A );
#endif

} // lapack
} // elemental

//----------------------------------------------------------------------------//
// Implementation begins here                                                 //
//----------------------------------------------------------------------------//

template<typename T>
inline void
elemental::lapack::Chol
( Shape shape, Matrix<T>& A )
{
#ifndef RELEASE
    PushCallStack("lapack::Chol");
    if( A.Height() != A.Width() )
        throw std::logic_error( "A must be square." );
#endif
    const char uplo = ShapeToChar( shape );
    wrappers::lapack::Chol( uplo, A.Height(), A.Buffer(), A.LDim() );
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T>
inline void
elemental::lapack::Hegst
( Side side, Shape shape, Matrix<T>& A, const Matrix<T>& B )
{
#ifndef RELEASE
    PushCallStack("lapack::Hegst");
    if( A.Height() != A.Width() )
        throw std::logic_error( "A must be square." );
    if( B.Height() != B.Width() )
        throw std::logic_error( "B must be square." );
    if( A.Height() != B.Height() )
        throw std::logic_error( "A and B must be the same size." );
#endif
    const int itype = ( side==Left ? 2 : 1 );
    const char uplo = ShapeToChar( shape );
    wrappers::lapack::Hegst
    ( itype, uplo, A.Height(), 
      A.Buffer(), A.LDim(), B.LockedBuffer(), B.LDim() );
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T>
inline void
elemental::lapack::LU
( Matrix<T>& A, Matrix<int>& p )
{
#ifndef RELEASE
    PushCallStack("lapack::LU");
    if( p.Height() != A.Height() )
        throw std::logic_error( "A and p must be the same height." );
#endif
    wrappers::lapack::LU
    ( A.Height(), A.Width(), A.Buffer(), A.LDim(), p.Buffer() );
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename R>
inline void
elemental::lapack::QR
( Matrix<R>& A )
{
#ifndef RELEASE
    PushCallStack("lapack::QR");
#endif
    wrappers::lapack::QR
    ( A.Height(), A.Width(), A.Buffer(), A.LDim() );
#ifndef RELEASE
    PopCallStack();
#endif
}

#ifndef WITHOUT_COMPLEX
template<typename R>
inline void
elemental::lapack::QR
( Matrix< std::complex<R> >& A, Matrix< std::complex<R> >& t )
{
#ifndef RELEASE
    PushCallStack("lapack::QR");
    if( t.Viewing() && 
        (t.Height() != std::min(A.Height(),A.Width()) || t.Width() != 1) )
        throw std::logic_error
              ( "t must be a vector of length equal to the min. dim. of A." );
#endif
    if( !t.Viewing() )
        t.ResizeTo( A.Height(), 1 );
    wrappers::lapack::QR
    ( A.Height(), A.Width(), A.Buffer(), A.LDim(), t.Buffer() );
#ifndef RELEASE
    PopCallStack();
#endif
}
#endif

template<typename R>
inline void
elemental::lapack::SVD
( Matrix<R>& A, Matrix<R>& VT, std::vector<R>& SigmaDiag )
{
#ifndef RELEASE
    PushCallStack("lapack::SVD");
#endif
    const int m = A.Height();
    const int n = A.Width();
    SigmaDiag.resize( std::min(m,n) );
    VT.ResizeTo( std::min(m,n), n );
    wrappers::lapack::SVD
    ( 'O', 'S', m, n, A.Buffer(), A.LDim(), &SigmaDiag[0], 0, 0, 
      VT.Buffer(), VT.LDim() );
#ifndef RELEASE
    PopCallStack();
#endif
}

#ifndef WITHOUT_COMPLEX
template<typename R>
inline void
elemental::lapack::SVD
( Matrix< std::complex<R> >& A, Matrix< std::complex<R> >& VT, 
  std::vector<R>& SigmaDiag )
{
#ifndef RELEASE
    PushCallStack("lapack::SVD");
#endif
    const int m = A.Height();
    const int n = A.Width();
    SigmaDiag.resize( std::min(m,n) );
    VT.ResizeTo( std::min(m,n), n );
    wrappers::lapack::SVD
    ( 'O', 'S', m, n, A.Buffer(), A.LDim(), &SigmaDiag[0], 0, 0, 
      VT.Buffer(), VT.LDim() );
#ifndef RELEASE
    PopCallStack();
#endif
}
#endif

template<typename R>
inline void
elemental::lapack::SVD
( Matrix<R>& A, Matrix<R>& U, Matrix<R>& VT, std::vector<R>& SigmaDiag )
{
#ifndef RELEASE
    PushCallStack("lapack::SVD");
#endif
    const int m = A.Height();
    const int n = A.Width();
    SigmaDiag.resize( std::min(m,n) );
    U.ResizeTo( m, std::min(m,n) );
    VT.ResizeTo( std::min(m,n), n );
    wrappers::lapack::SVD
    ( 'S', 'S', m, n, A.Buffer(), A.LDim(), &SigmaDiag[0], U.Buffer(), U.LDim(),
      VT.Buffer(), VT.LDim() );
#ifndef RELEASE
    PopCallStack();
#endif
}

#ifndef WITHOUT_COMPLEX
template<typename R>
inline void
elemental::lapack::SVD
( Matrix< std::complex<R> >& A, Matrix< std::complex<R> >& U, 
  Matrix< std::complex<R> >& VT, std::vector<R>& SigmaDiag )
{
#ifndef RELEASE
    PushCallStack("lapack::SVD");
#endif
    const int m = A.Height();
    const int n = A.Width();
    SigmaDiag.resize( std::min(m,n) );
    U.ResizeTo( m, std::min(m,n) );
    VT.ResizeTo( std::min(m,n), n );
    wrappers::lapack::SVD
    ( 'S', 'S', m, n, A.Buffer(), A.LDim(), &SigmaDiag[0], U.Buffer(), U.LDim(),
      VT.Buffer(), VT.LDim() );
#ifndef RELEASE
    PopCallStack();
#endif
}
#endif

template<typename T>
inline void
elemental::lapack::Trinv
( Shape shape, Diagonal diagonal, Matrix<T>& A )
{
#ifndef RELEASE
    PushCallStack("lapack::Trinv");
    if( A.Height() != A.Width() )
        throw std::logic_error( "A must be square." );
#endif
    const char uplo = ShapeToChar( shape );
    const char diag = DiagonalToChar( diagonal );
    wrappers::lapack::Trinv
    ( uplo, diag, A.Height(), A.Buffer(), A.LDim() );
#ifndef RELEASE
    PopCallStack();
#endif
}

#endif /* ELEMENTAL_LAPACK_HPP */

