/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "elemental.hpp"

extern "C" {

// Machine constants
float LAPACK(slamch)( const char* cmach );
double LAPACK(dlamch)( const char* cmach );

// Safe norms
float LAPACK(slapy2)
( const float* alpha, const float* beta );
double LAPACK(dlapy2)
( const double* alpha, const double* beta );
float LAPACK(slapy3)
( const float* alpha, const float* beta, const float* gamma );
double LAPACK(dlapy3)
( const double* alpha, const double* beta, const double* gamma );

// Safely compute a Givens rotation
void LAPACK(slartg)
( const float* phi, const float* gamma,
  float* c, float* s, float* rho );
void LAPACK(dlartg)
( const double* phi, const double* gamma,
  double* c, double* s, double* rho );
void LAPACK(clartg)
( const elem::scomplex* phi, const elem::scomplex* gamma,
  float* c, elem::scomplex* s, elem::scomplex* rho );
void LAPACK(zlartg)
( const elem::dcomplex* phi, const elem::dcomplex* gamma,
  double* c, elem::dcomplex* s, elem::dcomplex* rho );

// Bidiagonal DQDS
void LAPACK(slasq1)
( const int* n, float* d, float* e, float* work, int* info );
void LAPACK(dlasq1)
( const int* n, double* d, double* e, double* work, int* info );

// Bidiagonal QR
void LAPACK(sbdsqr)
( const char* uplo, const int* n, const int* numColsVTrans, const int* numRowsU,
  const int* numColsC, float* d, float* e, float* VTrans, const int* ldVTrans,
  float* U, const int* ldU, float* C, const int* ldC, float* work, int* info );
void LAPACK(dbdsqr)
( const char* uplo, const int* n, const int* numColsVTrans, const int* numRowsU,
  const int* numColsC, double* d, double* e,
  double* VTrans, const int* ldVTrans, double* U, const int* ldU,
  double* C, const int* ldC, double* work, int* info );
void LAPACK(cbdsqr)
( const char* uplo, const int* n, const int* numColsVAdj, const int* numRowsU,
  const int* numColsC, float* d, float* e,
  elem::scomplex* VAdj, const int* ldVAdj, elem::scomplex* U, const int* ldU,
  elem::scomplex* C, const int* ldC, float* work, int* info );
void LAPACK(zbdsqr)
( const char* uplo, const int* n, const int* numColsVAdj, const int* numRowsU,
  const int* numColsC, double* d, double* e,
  elem::dcomplex* VAdj, const int* ldVAdj, elem::dcomplex* U, const int* ldU,
  elem::dcomplex* C, const int* ldC, double* work, int* info );

// Divide and Conquer SVD
void LAPACK(sgesdd)
( const char* jobz, const int* m, const int* n, float* A, const int* lda,
  float* s, float* U, const int* ldu, float* VTrans, const int* ldvt,
  float* work, const int* lwork, int* iwork, int* info );
void LAPACK(dgesdd)
( const char* jobz, const int* m, const int* n, double* A, const int* lda,
  double* s, double* U, const int* ldu, double* VTrans, const int* ldvt,
  double* work, const int* lwork, int* iwork, int* info );
void LAPACK(cgesdd)
( const char* jobz, const int* m, const int* n,
  elem::scomplex* A, const int* lda, float* s,
  elem::scomplex* U, const int* ldu, elem::scomplex* VTrans, const int* ldvt,
  elem::scomplex* work, const int* lwork, float* rwork,
  int* iwork, int* info );
void LAPACK(zgesdd)
( const char* jobz, const int* m, const int* n,
  elem::dcomplex* A, const int* lda, double* s,
  elem::dcomplex* U, const int* ldu, elem::dcomplex* VAdj, const int* ldva,
  elem::dcomplex* work, const int* lwork, double* rwork,
  int* iwork, int* info );

// QR-algorithm SVD
void LAPACK(sgesvd)
( const char* jobu, const char* jobvt, const int* m, const int* n,
  float* A, const int* lda,
  float* s, float* U, const int* ldu, float* VTrans, const int* ldvt,
  float* work, const int* lwork, int* info );
void LAPACK(dgesvd)
( const char* jobu, const char* jobvt, const int* m, const int* n,
  double* A, const int* lda,
  double* s, double* U, const int* ldu, double* VTrans, const int* ldvt,
  double* work, const int* lwork, int* info );
void LAPACK(cgesvd)
( const char* jobu, const char* jobva, const int* m, const int* n,
  elem::scomplex* A, const int* lda, float* s,
  elem::scomplex* U, const int* ldu, elem::scomplex* VTrans, const int* ldvt,
  elem::scomplex* work, const int* lwork, float* rwork, int* info );
void LAPACK(zgesvd)
( const char* jobu, const char* jobva, const int* m, const int* n,
  elem::dcomplex* A, const int* lda, double* s,
  elem::dcomplex* U, const int* ldu, elem::dcomplex* VAdj, const int* ldva,
  elem::dcomplex* work, const int* lwork, double* rwork, int* info );

// Hessenberg QR algorithm
void LAPACK(shseqr)
( const char* job, const char* compz, const int* n, 
  const int* ilo, const int* ihi, float* H, const int* ldh, 
  float* wr, float* wi, float* Z, const int* ldz, 
  float* work, const int* lwork, int* info );
void LAPACK(dhseqr)
( const char* job, const char* compz, const int* n, 
  const int* ilo, const int* ihi, double* H, const int* ldh, 
  double* wr, double* wi, double* Z, const int* ldz, 
  double* work, const int* lwork, int* info );
void LAPACK(chseqr)
( const char* job, const char* compz, const int* n,
  const int* ilo, const int* ihi, elem::scomplex* H, const int* ldh,
  elem::scomplex* w, elem::scomplex* Z, const int* ldz,
  elem::scomplex* work, const int* lwork, int* info );
void LAPACK(zhseqr)
( const char* job, const char* compz, const int* n,
  const int* ilo, const int* ihi, elem::dcomplex* H, const int* ldh,
  elem::dcomplex* w, elem::dcomplex* Z, const int* ldz,
  elem::dcomplex* work, const int* lwork, int* info );

// Hermitian eigensolvers (via MRRR)
void LAPACK(ssyevr)
( const char* job, const char* range, const char* uplo, const int* n,
  float* A, const int* lda, const float* vl, const float* vu, 
  const int* il, const int* iu, const float* abstol, int * m, 
  float* w, float* Z, const int* ldz, int* isuppz, 
  float* work, const int* lwork, int* iwork, const int* liwork, int* info );
void LAPACK(dsyevr)
( const char* job, const char* range, const char* uplo, const int* n,
  double* A, const int* lda, const double* vl, const double* vu, 
  const int* il, const int* iu, const double* abstol, int * m, 
  double* w, double* Z, const int* ldz, int* isuppz, 
  double* work, const int* lwork, int* iwork, const int* liwork, int* info );
void LAPACK(cheevr)
( const char* job, const char* range, const char* uplo, const int* n,
  elem::scomplex* A, const int* lda, const float* vl, const float* vu, 
  const int* il, const int* iu, const float* abstol, int* m,
  float* w, elem::scomplex* Z, const int* ldz, int* isuppz, 
  elem::scomplex* work, const int* lwork, float* rwork, const int* lrwork, 
  int* iwork, const int* liwork, int* info );
void LAPACK(zheevr)
( const char* job, const char* range, const char* uplo, const int* n,
  elem::dcomplex* A, const int* lda, const double* vl, const double* vu, 
  const int* il, const int* iu, const double* abstol, int* m,
  double* w, elem::dcomplex* Z, const int* ldz, int* isuppz, 
  elem::dcomplex* work, const int* lwork, double* rwork, const int* lrwork, 
  int* iwork, const int* liwork, int* info );

} // extern "C"

namespace elem {
namespace lapack {

//
// Machine constants
//

template<>
float MachineEpsilon<float>()
{
    const char cmach = 'E';
    return LAPACK(slamch)( &cmach );
}

template<> 
double MachineEpsilon<double>()
{
    const char cmach = 'E';
    return LAPACK(dlamch)( &cmach );
}

template<> 
float MachineSafeMin<float>()
{
    const char cmach = 'S';
    return LAPACK(slamch)( &cmach );
}

template<> 
double MachineSafeMin<double>()
{
    const char cmach = 'S';
    return LAPACK(dlamch)( &cmach );
}

template<> 
float MachineBase<float>()
{
    const char cmach = 'B';
    return LAPACK(slamch)( &cmach );
}

template<> 
double MachineBase<double>()
{
    const char cmach = 'B';
    return LAPACK(dlamch)( &cmach );
}

template<>
float MachinePrecision<float>()
{
    const char cmach = 'P';
    return LAPACK(slamch)( &cmach );
}

template<> 
double MachinePrecision<double>()
{
    const char cmach = 'P';
    return LAPACK(dlamch)( &cmach );
}

template<> 
float MachineUnderflowExponent<float>()
{
    const char cmach = 'M';
    return LAPACK(slamch)( &cmach );
}

template<> 
double MachineUnderflowExponent<double>()
{
    const char cmach = 'M';
    return LAPACK(dlamch)( &cmach );
}

template<>
float MachineUnderflowThreshold<float>()
{
    const char cmach = 'U';
    return LAPACK(slamch)( &cmach );
}

template<> 
double MachineUnderflowThreshold<double>()
{
    const char cmach = 'U';
    return LAPACK(dlamch)( &cmach );
}

template<> 
float MachineOverflowExponent<float>()
{
    const char cmach = 'L';
    return LAPACK(slamch)( &cmach );
}

template<> 
double MachineOverflowExponent<double>()
{
    const char cmach = 'L';
    return LAPACK(dlamch)( &cmach );
}

template<> 
float MachineOverflowThreshold<float>()
{
    const char cmach = 'O';
    return LAPACK(slamch)( &cmach );
}

template<> 
double MachineOverflowThreshold<double>()
{
    const char cmach = 'O';
    return LAPACK(dlamch)( &cmach );
}

//
// Safely compute norms
//

float SafeNorm( float alpha, float beta )
{ return LAPACK(slapy2)( &alpha, &beta ); }

double SafeNorm( double alpha, double beta )
{ return LAPACK(dlapy2)( &alpha, &beta ); }

float SafeNorm( float alpha, float beta, float gamma )
{ return LAPACK(slapy3)( &alpha, &beta, &gamma ); }

double SafeNorm( double alpha, double beta, double gamma )
{ return LAPACK(dlapy3)( &alpha, &beta, &gamma ); }

//
// Safely compute Givens rotations (using Demmel and Kahan's algorithm)
//

void ComputeGivens
( float phi, float gamma,
  float* c, float* s, float* rho )
{ LAPACK(slartg)( &phi, &gamma, c, s, rho ); }

void ComputeGivens
( double phi, double gamma,
  double* c, double* s, double* rho )
{ LAPACK(dlartg)( &phi, &gamma, c, s, rho ); }

void ComputeGivens
( scomplex phi, scomplex gamma,
  float* c, scomplex* s, scomplex* rho )
{ LAPACK(clartg)( &phi, &gamma, c, s, rho ); }

void ComputeGivens
( dcomplex phi, dcomplex gamma,
  double* c, dcomplex* s, dcomplex* rho )
{ LAPACK(zlartg)( &phi, &gamma, c, s, rho ); }

//
// Bidiagonal DQDS for singular values
//

void BidiagDQDS( int n, float* d, float* e )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::BidiagDQDS");
#endif
    int info;
    std::vector<float> work( 4*n );
    LAPACK(slasq1)( &n, d, e, &work[0], &info );
    if( info != 0 )
    {
        std::ostringstream msg;
        if( info < 0 )
            msg << "Argument " << -info << " had an illegal value";
        else if( info == 1 )
            msg << "A split was marked in a positive value in E";
        else if( info == 2 )
            msg << "Current block of Z not bidiagonalized after 30*k its";
        else if( info == 3 )
            msg << "Termination criterion of outer while loop not met";
        throw std::logic_error( msg.str().c_str() );
    } 
}

void BidiagDQDS( int n, double* d, double* e )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::BidiagDQDS");
#endif
    int info;
    std::vector<double> work( 4*n );
    LAPACK(dlasq1)( &n, d, e, &work[0], &info );
    if( info != 0 )
    {
        std::ostringstream msg;
        if( info < 0 )
            msg << "Argument " << -info << " had an illegal value";
        else if( info == 1 )
            msg << "A split was marked in a positive value in E";
        else if( info == 2 )
            msg << "Current block of Z not bidiagonalized after 30*k its";
        else if( info == 3 )
            msg << "Termination criterion of outer while loop not met";
        throw std::logic_error( msg.str().c_str() );
    } 
}

//
// Bidiagonal QR algorithm for SVD
//

void BidiagQRAlg
( char uplo, int n, int numColsVTrans, int numRowsU,
  float* d, float* e, float* VTrans, int ldVTrans, float* U, int ldU )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::BidiagQRAlg");
#endif
    if( n==0 )
        return;

    int info;
    float* C=0;
    const int numColsC=0, ldC=1;
    std::vector<float> work( 4*n );
    LAPACK(sbdsqr)
    ( &uplo, &n, &numColsVTrans, &numRowsU, &numColsC, d, e, VTrans, &ldVTrans,
      U, &ldU, C, &ldC, &work[0], &info );
    if( info != 0 )
    {
        std::ostringstream msg;
        if( info < 0 )
            msg << "Argument " << -info << " had illegal value";
        else
            msg << "sbdsqr had " << info << " elements of e not converge";
        throw std::logic_error( msg.str().c_str() );
    }
}

void BidiagQRAlg
( char uplo, int n, int numColsVTrans, int numRowsU, 
  double* d, double* e, double* VTrans, int ldVTrans, double* U, int ldU )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::BidiagQRAlg");
#endif
    if( n==0 )
        return;

    int info;
    double* C=0;
    const int numColsC=0, ldC=1;
    std::vector<double> work( 4*n );
    LAPACK(dbdsqr)
    ( &uplo, &n, &numColsVTrans, &numRowsU, &numColsC, d, e, VTrans, &ldVTrans,
      U, &ldU, C, &ldC, &work[0], &info );
    if( info != 0 )
    {
        std::ostringstream msg;
        if( info < 0 )
            msg << "Argument " << -info << " had illegal value";
        else
            msg << "dbdsqr had " << info << " elements of e not converge";
        throw std::logic_error( msg.str().c_str() );
    }
}

void BidiagQRAlg
( char uplo, int n, int numColsVAdj, int numRowsU, 
  float* d, float* e, scomplex* VAdj, int ldVAdj, scomplex* U, int ldU )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::BidiagQRAlg");
#endif
    if( n==0 )
        return;

    int info;
    scomplex* C=0;
    const int numColsC=0, ldC=1;
    std::vector<float> work( 4*n );
    LAPACK(cbdsqr)
    ( &uplo, &n, &numColsVAdj, &numRowsU, &numColsC, d, e, VAdj, &ldVAdj,
      U, &ldU, C, &ldC, &work[0], &info );
    if( info != 0 )
    {
        std::ostringstream msg;
        if( info < 0 )
            msg << "Argument " << -info << " had illegal value";
        else
            msg << "cbdsqr had " << info << " elements of e not converge";
        throw std::logic_error( msg.str().c_str() );
    }
}

void BidiagQRAlg
( char uplo, int n, int numColsVAdj, int numRowsU, 
  double* d, double* e, dcomplex* VAdj, int ldVAdj, dcomplex* U, int ldU )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::BidiagQRAlg");
#endif
    if( n==0 )
        return;

    int info;
    dcomplex* C=0;
    const int numColsC=0, ldC=1;
    std::vector<double> work( 4*n );
    LAPACK(zbdsqr)
    ( &uplo, &n, &numColsVAdj, &numRowsU, &numColsC, d, e, VAdj, &ldVAdj,
      U, &ldU, C, &ldC, &work[0], &info );
    if( info != 0 )
    {
        std::ostringstream msg;
        if( info < 0 )
            msg << "Argument " << -info << " had illegal value";
        else
            msg << "zbdsqr had " << info << " elements of e not converge";
        throw std::logic_error( msg.str().c_str() );
    }
}

//
// Divide and Conquer SVD
//

void DivideAndConquerSVD
( int m, int n, float* A, int lda, 
  float* s, float* U, int ldu, float* VTrans, int ldvt )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::DivideAndConquerSVD");
#endif
    if( m==0 || n==0 )
        return;

    const char jobz='S';
    int lwork=-1, info;
    float dummyWork;
    const int k = std::min(m,n);
    std::vector<int> iwork(8*k);

    LAPACK(sgesdd)
    ( &jobz, &m, &n, A, &lda, s, U, &ldu, VTrans, &ldvt, &dummyWork, &lwork,
      &iwork[0], &info );

    lwork = dummyWork;
    std::vector<float> work(lwork);
    LAPACK(sgesdd)
    ( &jobz, &m, &n, A, &lda, s, U, &ldu, VTrans, &ldvt, &work[0], &lwork,
      &iwork[0], &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("sgesdd's updating process failed");
    }
}

void DivideAndConquerSVD
( int m, int n, double* A, int lda, 
  double* s, double* U, int ldu, double* VTrans, int ldvt )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::DivideAndConquerSVD");
#endif
    if( m==0 || n==0 )
        return;

    const char jobz='S';
    int lwork=-1, info;
    double dummyWork;
    const int k = std::min(m,n);
    std::vector<int> iwork(8*k);

    LAPACK(dgesdd)
    ( &jobz, &m, &n, A, &lda, s, U, &ldu, VTrans, &ldvt, &dummyWork, &lwork,
      &iwork[0], &info );

    lwork = dummyWork;
    std::vector<double> work(lwork);
    LAPACK(dgesdd)
    ( &jobz, &m, &n, A, &lda, s, U, &ldu, VTrans, &ldvt, &work[0], &lwork,
      &iwork[0], &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("dgesdd's updating process failed");
    }
}

void DivideAndConquerSVD
( int m, int n, scomplex* A, int lda, 
  float* s, scomplex* U, int ldu, scomplex* VAdj, int ldva )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::DivideAndConquerSVD");
#endif
    if( m==0 || n==0 )
        return;

    const char jobz='S';
    int lwork=-1, info;
    const int k = std::min(m,n);
    const int K = std::max(m,n);
    const int lrwork = k*std::max(5*k+7,2*K+2*k+1);
    std::vector<float> rwork(lrwork);
    std::vector<int> iwork(8*k);

    scomplex dummyWork;
    LAPACK(cgesdd)
    ( &jobz, &m, &n, A, &lda, s, U, &ldu, VAdj, &ldva, &dummyWork, &lwork,
      &rwork[0], &iwork[0], &info );

    lwork = dummyWork.real;
    std::vector<scomplex> work(lwork);
    LAPACK(cgesdd)
    ( &jobz, &m, &n, A, &lda, s, U, &ldu, VAdj, &ldva, &work[0], &lwork,
      &rwork[0], &iwork[0], &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("cgesdd's updating process failed");
    }
}

void DivideAndConquerSVD
( int m, int n, dcomplex* A, int lda, 
  double* s, dcomplex* U, int ldu, dcomplex* VAdj, int ldva )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::DivideAndConquerSVD");
#endif
    if( m==0 || n==0 )
        return;

    const char jobz='S';
    int lwork=-1, info;
    dcomplex dummyWork;
    const int k = std::min(m,n);
    const int K = std::max(m,n);
    const int lrwork = k*std::max(5*k+7,2*K+2*k+1);
    std::vector<double> rwork(lrwork);
    std::vector<int> iwork(8*k);

    LAPACK(zgesdd)
    ( &jobz, &m, &n, A, &lda, s, U, &ldu, VAdj, &ldva, &dummyWork, &lwork,
      &rwork[0], &iwork[0], &info );

    lwork = dummyWork.real;
    std::vector<dcomplex> work(lwork);
    LAPACK(zgesdd)
    ( &jobz, &m, &n, A, &lda, s, U, &ldu, VAdj, &ldva, &work[0], &lwork,
      &rwork[0], &iwork[0], &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("zgesdd's updating process failed");
    }
}

//
// QR-algorithm SVD
//

void QRSVD
( int m, int n, float* A, int lda, 
  float* s, float* U, int ldu, float* VTrans, int ldvt )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::QRSVD");
#endif
    if( m==0 || n==0 )
        return;

    const char jobu='S', jobvt='S';
    int lwork=-1, info;
    float dummyWork;

    LAPACK(sgesvd)
    ( &jobu, &jobvt, &m, &n, A, &lda, s, U, &ldu, VTrans, &ldvt, 
      &dummyWork, &lwork, &info );

    lwork = dummyWork;
    std::vector<float> work(lwork);
    LAPACK(sgesvd)
    ( &jobu, &jobvt, &m, &n, A, &lda, s, U, &ldu, VTrans, &ldvt, 
      &work[0], &lwork, &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("sgesvd's updating process failed");
    }
}

void QRSVD
( int m, int n, double* A, int lda, 
  double* s, double* U, int ldu, double* VTrans, int ldvt )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::QRSVD");
#endif
    if( m==0 || n==0 )
        return;

    const char jobu='S', jobvt='S';
    int lwork=-1, info;
    double dummyWork;

    LAPACK(dgesvd)
    ( &jobu, &jobvt, &m, &n, A, &lda, s, U, &ldu, VTrans, &ldvt, 
      &dummyWork, &lwork, &info );

    lwork = dummyWork;
    std::vector<double> work(lwork);
    LAPACK(dgesvd)
    ( &jobu, &jobvt, &m, &n, A, &lda, s, U, &ldu, VTrans, &ldvt, 
      &work[0], &lwork, &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("dgesvd's updating process failed");
    }
}

void QRSVD
( int m, int n, scomplex* A, int lda, 
  float* s, scomplex* U, int ldu, scomplex* VAdj, int ldva )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::QRSVD");
#endif
    if( m==0 || n==0 )
        return;

    const char jobu='S', jobva='S';
    int lwork=-1, info;
    const int k = std::min(m,n);
    std::vector<float> rwork(5*k);

    scomplex dummyWork;
    LAPACK(cgesvd)
    ( &jobu, &jobva, &m, &n, A, &lda, s, U, &ldu, VAdj, &ldva, 
      &dummyWork, &lwork, &rwork[0], &info );

    lwork = dummyWork.real;
    std::vector<scomplex> work(lwork);
    LAPACK(cgesvd)
    ( &jobu, &jobva, &m, &n, A, &lda, s, U, &ldu, VAdj, &ldva, 
      &work[0], &lwork, &rwork[0], &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("cgesvd's updating process failed");
    }
}

void QRSVD
( int m, int n, dcomplex* A, int lda, 
  double* s, dcomplex* U, int ldu, dcomplex* VAdj, int ldva )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::QRSVD");
#endif
    if( m==0 || n==0 )
        return;

    const char jobu='S', jobva='S';
    int lwork=-1, info;
    dcomplex dummyWork;
    const int k = std::min(m,n);
    std::vector<double> rwork(5*k);

    LAPACK(zgesvd)
    ( &jobu, &jobva, &m, &n, A, &lda, s, U, &ldu, VAdj, &ldva, 
      &dummyWork, &lwork, &rwork[0], &info );

    lwork = dummyWork.real;
    std::vector<dcomplex> work(lwork);
    LAPACK(zgesvd)
    ( &jobu, &jobva, &m, &n, A, &lda, s, U, &ldu, VAdj, &ldva, 
      &work[0], &lwork, &rwork[0], &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("zgesvd's updating process failed");
    }
}

//
// Compute singular values (with the QR algorithm)
//

void SVD( int m, int n, float* A, int lda, float* s )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::SVD");
#endif
    if( m==0 || n==0 )
        return;

    const char jobu='N', jobvt='N';
    int fakeLDim=1, lwork=-1, info;
    float dummyWork;

    LAPACK(sgesvd)
    ( &jobu, &jobvt, &m, &n, A, &lda, s, 0, &fakeLDim, 0, &fakeLDim, 
      &dummyWork, &lwork, &info );

    lwork = dummyWork;
    std::vector<float> work(lwork);
    LAPACK(sgesvd)
    ( &jobu, &jobvt, &m, &n, A, &lda, s, 0, &fakeLDim, 0, &fakeLDim, 
      &work[0], &lwork, &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("sgesvd's updating process failed");
    }
}

void SVD( int m, int n, double* A, int lda, double* s )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::SVD");
#endif
    if( m==0 || n==0 )
        return;

    const char jobu='N', jobvt='N';
    int fakeLDim=1, lwork=-1, info;
    double dummyWork;

    LAPACK(dgesvd)
    ( &jobu, &jobvt, &m, &n, A, &lda, s, 0, &fakeLDim, 0, &fakeLDim, 
      &dummyWork, &lwork, &info );

    lwork = dummyWork;
    std::vector<double> work(lwork);
    LAPACK(dgesvd)
    ( &jobu, &jobvt, &m, &n, A, &lda, s, 0, &fakeLDim, 0, &fakeLDim, 
      &work[0], &lwork, &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("dgesvd's updating process failed");
    }
}

void SVD( int m, int n, scomplex* A, int lda, float* s )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::SVD");
#endif
    if( m==0 || n==0 )
        return;

    const char jobu='N', jobva='N';
    int fakeLDim=1, lwork=-1, info;
    scomplex dummyWork;
    const int k = std::min(m,n);
    std::vector<float> rwork(5*k);

    LAPACK(cgesvd)
    ( &jobu, &jobva, &m, &n, A, &lda, s, 0, &fakeLDim, 0, &fakeLDim, 
      &dummyWork, &lwork, &rwork[0], &info );

    lwork = dummyWork.real;
    std::vector<scomplex> work(lwork);
    LAPACK(cgesvd)
    ( &jobu, &jobva, &m, &n, A, &lda, s, 0, &fakeLDim, 0, &fakeLDim, 
      &work[0], &lwork, &rwork[0], &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("cgesvd's updating process failed");
    }
}

void SVD( int m, int n, dcomplex* A, int lda, double* s )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::SVD");
#endif
    if( m==0 || n==0 )
        return;

    const char jobu='N', jobva='N';
    int fakeLDim=1, lwork=-1, info;
    dcomplex dummyWork;
    const int k = std::min(m,n);
    std::vector<double> rwork(5*k);

    LAPACK(zgesvd)
    ( &jobu, &jobva, &m, &n, A, &lda, s, 0, &fakeLDim, 0, &fakeLDim, 
      &dummyWork, &lwork, &rwork[0], &info );

    lwork = dummyWork.real;
    std::vector<dcomplex> work(lwork);
    LAPACK(zgesvd)
    ( &jobu, &jobva, &m, &n, A, &lda, s, 0, &fakeLDim, 0, &fakeLDim, 
      &work[0], &lwork, &rwork[0], &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("zgesvd's updating process failed");
    }
}

// 
// Compute the eigenvalues of an upper Hessenberg matrix
//

void HessenbergEig( int n, float* H, int ldh, scomplex* w )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::HessenbergEig");
#endif
    if( n == 0 )
        return;

    const char job='E', compz='N';
    int ilo=1, ihi=n;
    int fakeLDim=1, lwork=-1, info;
    float dummyWork;
    std::vector<float> wr( n ), wi( n );
    LAPACK(shseqr)
    ( &job, &compz, &n, &ilo, &ihi, H, &ldh, &wr[0], &wi[0], 0, &fakeLDim, 
      &dummyWork, &lwork, &info );

    lwork = dummyWork;
    std::vector<float> work(lwork);
    LAPACK(shseqr)
    ( &job, &compz, &n, &ilo, &ihi, H, &ldh, &wr[0], &wi[0], 0, &fakeLDim, 
      &work[0], &lwork, &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("shseqr's failed to compute all eigenvalues");
    }

    for( int i=0; i<n; ++i )
        w[i] = elem::Complex<float>(wr[i],wi[i]);
}

void HessenbergEig( int n, double* H, int ldh, dcomplex* w )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::HessenbergEig");
#endif
    if( n == 0 )
        return;

    const char job='E', compz='N';
    int ilo=1, ihi=n;
    int fakeLDim=1, lwork=-1, info;
    double dummyWork;
    std::vector<double> wr( n ), wi( n );
    LAPACK(dhseqr)
    ( &job, &compz, &n, &ilo, &ihi, H, &ldh, &wr[0], &wi[0], 0, &fakeLDim,
      &dummyWork, &lwork, &info );

    lwork = dummyWork;
    std::vector<double> work(lwork);
    LAPACK(dhseqr)
    ( &job, &compz, &n, &ilo, &ihi, H, &ldh, &wr[0], &wi[0], 0, &fakeLDim,
      &work[0], &lwork, &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("dhseqr's failed to compute all eigenvalues");
    }
    
    for( int i=0; i<n; ++i )
        w[i] = elem::Complex<double>(wr[i],wi[i]);
}

void HessenbergEig( int n, scomplex* H, int ldh, scomplex* w )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::HessenbergEig");
#endif
    if( n == 0 )
        return;

    const char job='E', compz='N';
    int ilo=1, ihi=n;
    int fakeLDim=1, lwork=-1, info;
    scomplex dummyWork;
    LAPACK(chseqr)
    ( &job, &compz, &n, &ilo, &ihi, H, &ldh, w, 0, &fakeLDim, 
      &dummyWork, &lwork, &info );

    lwork = dummyWork.real;
    std::vector<scomplex> work(lwork);
    LAPACK(chseqr)
    ( &job, &compz, &n, &ilo, &ihi, H, &ldh, w, 0, &fakeLDim, 
      &work[0], &lwork, &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("chseqr's failed to compute all eigenvalues");
    }
}

void HessenbergEig( int n, dcomplex* H, int ldh, dcomplex* w )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::HessenbergEig");
#endif
    if( n == 0 )
        return;

    const char job='E', compz='N';
    int ilo=1, ihi=n;
    int fakeLDim=1, lwork=-1, info;
    dcomplex dummyWork;
    LAPACK(zhseqr)
    ( &job, &compz, &n, &ilo, &ihi, H, &ldh, w, 0, &fakeLDim, 
      &dummyWork, &lwork, &info );

    lwork = dummyWork.real;
    std::vector<dcomplex> work(lwork);
    LAPACK(zhseqr)
    ( &job, &compz, &n, &ilo, &ihi, H, &ldh, w, 0, &fakeLDim, 
      &work[0], &lwork, &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
    {
        throw std::runtime_error("zhseqr's failed to compute all eigenvalues");
    }
}

int HermitianEig
( char job, char range, char uplo, int n, float* A, int lda, 
  float vl, float vu, int il, int iu, float abstol, 
  float* w, float* Z, int ldz )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::HermitianEig");
#endif
    if( n == 0 )
        return 0;

    std::vector<int> isuppz( 2*n );

    int lwork=-1, liwork=-1, m, info;
    int dummyIwork;
    float dummyWork;
    LAPACK(ssyevr)
    ( &job, &range, &uplo, &n, A, &lda, &vl, &vu, &il, &iu, &abstol, &m,
      w, Z, &ldz, &isuppz[0], &dummyWork, &lwork, &dummyIwork, &liwork, &info );

    lwork = dummyWork;
    liwork = dummyIwork;
    std::vector<float> work(lwork);
    std::vector<int> iwork(liwork);
    LAPACK(ssyevr)
    ( &job, &range, &uplo, &n, A, &lda, &vl, &vu, &il, &iu, &abstol, &m,
      w, Z, &ldz, &isuppz[0], &work[0], &lwork, &iwork[0], &liwork, &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
        throw std::runtime_error("ssyevr's failed");
    return m;
}

int HermitianEig
( char job, char range, char uplo, int n, double* A, int lda, 
  double vl, double vu, int il, int iu, double abstol, 
  double* w, double* Z, int ldz )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::HermitianEig");
#endif
    if( n == 0 )
        return 0;

    std::vector<int> isuppz( 2*n );

    int lwork=-1, liwork=-1, m, info;
    int dummyIwork;
    double dummyWork;
    LAPACK(dsyevr)
    ( &job, &range, &uplo, &n, A, &lda, &vl, &vu, &il, &iu, &abstol, &m,
      w, Z, &ldz, &isuppz[0], &dummyWork, &lwork, &dummyIwork, &liwork, &info );

    lwork = dummyWork;
    liwork = dummyIwork;
    std::vector<double> work(lwork);
    std::vector<int> iwork(liwork);
    LAPACK(dsyevr)
    ( &job, &range, &uplo, &n, A, &lda, &vl, &vu, &il, &iu, &abstol, &m,
      w, Z, &ldz, &isuppz[0], &work[0], &lwork, &iwork[0], &liwork, &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
        throw std::runtime_error("dsyevr's failed");
    return m;
}

int HermitianEig
( char job, char range, char uplo, int n, scomplex* A, int lda, 
  float vl, float vu, int il, int iu, float abstol, 
  float* w, scomplex* Z, int ldz )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::HermitianEig");
#endif
    if( n == 0 )
        return 0;

    std::vector<int> isuppz( 2*n );

    int lwork=-1, lrwork=-1, liwork=-1, m, info;
    int dummyIwork;
    float dummyRwork;
    scomplex dummyWork;
    LAPACK(cheevr)
    ( &job, &range, &uplo, &n, A, &lda, &vl, &vu, &il, &iu, &abstol, &m,
      w, Z, &ldz, &isuppz[0], &dummyWork, &lwork, &dummyRwork, &lrwork,
      &dummyIwork, &liwork, &info );

    lwork = dummyWork.real;
    lrwork = dummyRwork;
    liwork = dummyIwork;
    std::vector<scomplex> work(lwork);
    std::vector<float> rwork(lrwork);
    std::vector<int> iwork(liwork);
    LAPACK(cheevr)
    ( &job, &range, &uplo, &n, A, &lda, &vl, &vu, &il, &iu, &abstol, &m,
      w, Z, &ldz, &isuppz[0], &work[0], &lwork, &rwork[0], &lrwork,
      &iwork[0], &liwork, &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
        throw std::runtime_error("cheevr's failed");
    return m;
}

int HermitianEig
( char job, char range, char uplo, int n, dcomplex* A, int lda, 
  double vl, double vu, int il, int iu, double abstol, 
  double* w, dcomplex* Z, int ldz )
{
#ifndef RELEASE
    CallStackEntry entry("lapack::HermitianEig");
#endif
    if( n == 0 )
        return 0;

    std::vector<int> isuppz( 2*n );

    int lwork=-1, lrwork=-1, liwork=-1, m, info;
    int dummyIwork;
    double dummyRwork;
    dcomplex dummyWork;
    LAPACK(zheevr)
    ( &job, &range, &uplo, &n, A, &lda, &vl, &vu, &il, &iu, &abstol, &m,
      w, Z, &ldz, &isuppz[0], &dummyWork, &lwork, &dummyRwork, &lrwork,
      &dummyIwork, &liwork, &info );

    lwork = dummyWork.real;
    lrwork = dummyRwork;
    liwork = dummyIwork;
    std::vector<dcomplex> work(lwork);
    std::vector<double> rwork(lrwork);
    std::vector<int> iwork(liwork);
    LAPACK(zheevr)
    ( &job, &range, &uplo, &n, A, &lda, &vl, &vu, &il, &iu, &abstol, &m,
      w, Z, &ldz, &isuppz[0], &work[0], &lwork, &rwork[0], &lrwork,
      &iwork[0], &liwork, &info );
    if( info < 0 )
    {
        std::ostringstream msg;
        msg << "Argument " << -info << " had illegal value";
        throw std::logic_error( msg.str().c_str() );
    }
    else if( info > 0 )
        throw std::runtime_error("zheevr's failed");
    return m;
}

} // namespace lapack
} // namespace elem
