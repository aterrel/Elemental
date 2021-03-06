/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
// NOTE: It is possible to simply include "elemental.hpp" instead
#include "elemental-lite.hpp"
#include "elemental/blas-like/level1/MakeTriangular.hpp"
#include "elemental/lapack-like/ApplyPackedReflectors.hpp"
#include "elemental/lapack-like/Norm/Infinity.hpp"
#include "elemental/lapack-like/Norm/Frobenius.hpp"
#include "elemental/matrices/HermitianUniformSpectrum.hpp"
using namespace std;
using namespace elem;

template<typename F> 
void TestCorrectness
( bool print,
  UpperOrLower uplo, 
  const DistMatrix<F>& A, 
  const DistMatrix<F,STAR,STAR>& t,
        DistMatrix<F>& AOrig )
{
    typedef BASE(F) R;
    const Grid& g = A.Grid();
    const int m = AOrig.Height();

    int subdiagonal = ( uplo==LOWER ? -1 : +1 );

    if( g.Rank() == 0 )
        cout << "Testing error..." << endl;

    // Grab the diagonal and subdiagonal of the symmetric tridiagonal matrix
    DistMatrix<R,MD,STAR> d(g);
    DistMatrix<R,MD,STAR> e(g);
    A.GetRealPartOfDiagonal( d );
    A.GetRealPartOfDiagonal( e, subdiagonal );
     
    // Grab a full copy of e so that we may fill the opposite subdiagonal 
    DistMatrix<R,STAR,STAR> e_STAR_STAR(g);
    DistMatrix<R,MD,STAR> eOpposite(g);
    e_STAR_STAR = e;
    eOpposite.AlignWithDiagonal( A.DistData(), -subdiagonal );
    eOpposite = e_STAR_STAR;
    
    // Zero B and then fill its tridiagonal
    DistMatrix<F> B(g);
    B.AlignWith( A );
    Zeros( B, m, m );
    B.SetRealPartOfDiagonal( d );
    B.SetRealPartOfDiagonal( e, subdiagonal );
    B.SetRealPartOfDiagonal( eOpposite, -subdiagonal );
    if( print )
        Print( B, "Tridiagonal" );

    // Reverse the accumulated Householder transforms, ignoring symmetry
    if( uplo == LOWER )
    {
        ApplyPackedReflectors
        ( LEFT, LOWER, VERTICAL, BACKWARD, 
          UNCONJUGATED, subdiagonal, A, t, B );
        if( print )
            Print( B, "Partially rotated tridiagonal" );
        ApplyPackedReflectors
        ( RIGHT, LOWER, VERTICAL, BACKWARD, 
          CONJUGATED, subdiagonal, A, t, B );
    }
    else
    {
        ApplyPackedReflectors
        ( LEFT, UPPER, VERTICAL, FORWARD, 
          UNCONJUGATED, subdiagonal, A, t, B );
        if( print )
            Print( B, "Partially rotated tridiagonal" );
        ApplyPackedReflectors
        ( RIGHT, UPPER, VERTICAL, FORWARD, 
          CONJUGATED, subdiagonal, A, t, B );
    }
    if( print )
        Print( B, "Rotated tridiagonal" );

    // Compare the appropriate triangle of AOrig and B
    MakeTriangular( uplo, AOrig );
    MakeTriangular( uplo, B );
    Axpy( F(-1), AOrig, B );
    if( print )
        Print( B, "Error in rotated tridiagonal" );

    const R infNormOfAOrig = HermitianInfinityNorm( uplo, AOrig );
    const R frobNormOfAOrig = HermitianFrobeniusNorm( uplo, AOrig );
    const R infNormOfError = HermitianInfinityNorm( uplo, B );
    const R frobNormOfError = HermitianFrobeniusNorm( uplo, B );
    if( g.Rank() == 0 )
    {
        cout << "    ||AOrig||_1 = ||AOrig||_oo = " << infNormOfAOrig << "\n"
             << "    ||AOrig||_F                = " << frobNormOfAOrig << "\n"
             << "    ||AOrig - Q^H A Q||_oo     = " << infNormOfError << "\n"
             << "    ||AOrig - Q^H A Q||_F      = " << frobNormOfError << endl;
    }
}

template<typename F>
void TestHermitianTridiag
( bool testCorrectness, bool print,
  UpperOrLower uplo, int m, const Grid& g )
{
    DistMatrix<F> A(g), AOrig(g);
    DistMatrix<F,STAR,STAR> t(g);

    HermitianUniformSpectrum( A, m, -10, 10 );
    if( testCorrectness )
    {
        if( g.Rank() == 0 )
        {
            cout << "  Making copy of original matrix...";
            cout.flush();
        }
        AOrig = A;
        if( g.Rank() == 0 )
            cout << "DONE" << endl;
    }
    if( print )
        Print( A, "A" );

    if( g.Rank() == 0 )
    {
        cout << "  Starting tridiagonalization...";
        cout.flush();
    }
    mpi::Barrier( g.Comm() );
    const double startTime = mpi::Time();
    HermitianTridiag( uplo, A, t );
    mpi::Barrier( g.Comm() );
    const double runTime = mpi::Time() - startTime;
    const double realGFlops = 16./3.*Pow(double(m),3.)/(1.e9*runTime);
    const double gFlops = ( IsComplex<F>::val ? 4*realGFlops : realGFlops );
    if( g.Rank() == 0 )
    {
        cout << "DONE. " << endl
             << "  Time = " << runTime << " seconds. GFlops = " 
             << gFlops << endl;
    }
    if( print )
    {
        Print( A, "A after HermitianTridiag" );
        Print( t, "t after HermitianTridiag" );
    }
    if( testCorrectness )
        TestCorrectness( print, uplo, A, t, AOrig );
}

int 
main( int argc, char* argv[] )
{
    Initialize( argc, argv );
    mpi::Comm comm = mpi::COMM_WORLD;
    const int commRank = mpi::CommRank( comm );
    const int commSize = mpi::CommSize( comm );

    try
    {
        int r = Input("--gridHeight","height of process grid",0);
        const char uploChar = Input("--uplo","upper or lower storage: L/U",'L');
        const int m = Input("--height","height of matrix",100);
        const int nb = Input("--nb","algorithmic blocksize",96);
        const int nbLocal = Input("--nbLocal","local blocksize",32);
        const bool testCorrectness = Input
            ("--correctness","test correctness?",true);
        const bool print = Input("--print","print matrices?",false);
        ProcessInput();
        PrintInputReport();

        if( r == 0 )
            r = Grid::FindFactor( commSize );
        const Grid g( comm, r );
        const UpperOrLower uplo = CharToUpperOrLower( uploChar );
        SetBlocksize( nb );
        SetLocalSymvBlocksize<double>( nbLocal );
        SetLocalSymvBlocksize<Complex<double> >( nbLocal );
        ComplainIfDebug();
        if( commRank == 0 )
            cout << "Will test HermitianTridiag" << uploChar << endl;

        if( commRank == 0 )
        {
            cout << "----------------------------------\n"
                 << "Double-precision normal algorithm:\n"
                 << "----------------------------------" << endl;
        }
        SetHermitianTridiagApproach( HERMITIAN_TRIDIAG_NORMAL );
        TestHermitianTridiag<double>( testCorrectness, print, uplo, m, g );

        if( commRank == 0 )
        {
            cout << "--------------------------------------------------\n"
                 << "Double-precision square algorithm, row-major grid:\n"
                 << "--------------------------------------------------" 
                 << endl;
        }
        SetHermitianTridiagApproach( HERMITIAN_TRIDIAG_SQUARE );
        SetHermitianTridiagGridOrder( ROW_MAJOR );
        TestHermitianTridiag<double>( testCorrectness, print, uplo, m, g );

        if( commRank == 0 )
        {
            cout << "--------------------------------------------------\n"
                 << "Double-precision square algorithm, col-major grid:\n"
                 << "--------------------------------------------------" 
                 << endl;
        }
        SetHermitianTridiagApproach( HERMITIAN_TRIDIAG_SQUARE );
        SetHermitianTridiagGridOrder( COLUMN_MAJOR );
        TestHermitianTridiag<double>( testCorrectness, print, uplo, m, g );

        if( commRank == 0 )
        {
            cout << "------------------------------------------\n"
                 << "Double-precision complex normal algorithm:\n"
                 << "------------------------------------------" << endl;
        }
        SetHermitianTridiagApproach( HERMITIAN_TRIDIAG_NORMAL );
        TestHermitianTridiag<Complex<double> >
        ( testCorrectness, print, uplo, m, g );

        if( commRank == 0 )
        {
            cout << "-------------------------------------------\n"
                 << "Double-precision complex square algorithm, \n"
                 << "row-major grid:\n"
                 << "-------------------------------------------" 
                 << endl;
        }
        SetHermitianTridiagApproach( HERMITIAN_TRIDIAG_SQUARE );
        SetHermitianTridiagGridOrder( ROW_MAJOR );
        TestHermitianTridiag<Complex<double> >
        ( testCorrectness, print, uplo, m, g );

        if( commRank == 0 )
        {
            cout << "-------------------------------------------\n"
                 << "Double-precision complex square algorithm, \n"
                 << "col-major grid:\n"
                 << "-------------------------------------------" 
                 << endl;
        }
        SetHermitianTridiagApproach( HERMITIAN_TRIDIAG_SQUARE );
        SetHermitianTridiagGridOrder( COLUMN_MAJOR );
        TestHermitianTridiag<Complex<double> >
        ( testCorrectness, print, uplo, m, g );
    }
    catch( exception& e ) { ReportException(e); }

    Finalize();
    return 0;
}
