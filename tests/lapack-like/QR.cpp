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
#include "elemental/lapack-like/Norm/Frobenius.hpp"
#include "elemental/lapack-like/Norm/Infinity.hpp"
#include "elemental/lapack-like/Norm/One.hpp"
#include "elemental/lapack-like/QR.hpp"
#include "elemental/matrices/Identity.hpp"
#include "elemental/matrices/Uniform.hpp"
using namespace std;
using namespace elem;

template<typename F>
void TestCorrectness
( const DistMatrix<F>& A,
  const DistMatrix<F,MD,STAR>& t,
        DistMatrix<F>& AOrig )
{
    typedef BASE(F) R;
    const Grid& g = A.Grid();
    const int m = A.Height();
    const int n = A.Width();
    const int minDim = std::min(m,n);

    if( g.Rank() == 0 )
        cout << "  Testing orthogonality of Q..." << endl;

    // Form Z := Q^H Q as an approximation to identity
    DistMatrix<F> Z(g);
    Identity( Z, m, n );
    ApplyPackedReflectors
    ( LEFT, LOWER, VERTICAL, BACKWARD, UNCONJUGATED, 0, A, t, Z );
    ApplyPackedReflectors
    ( LEFT, LOWER, VERTICAL, FORWARD, CONJUGATED, 0, A, t, Z );
    
    DistMatrix<F> ZUpper(g);
    View( ZUpper, Z, 0, 0, minDim, minDim );

    // Form Identity
    DistMatrix<F> X(g);
    Identity( X, minDim, minDim );

    // Form X := I - Q^H Q
    Axpy( F(-1), ZUpper, X );

    R oneNormOfError = OneNorm( X );
    R infNormOfError = InfinityNorm( X );
    R frobNormOfError = FrobeniusNorm( X );
    if( g.Rank() == 0 )
    {
        cout << "    ||Q^H Q - I||_1  = " << oneNormOfError << "\n"
             << "    ||Q^H Q - I||_oo = " << infNormOfError << "\n"
             << "    ||Q^H Q - I||_F  = " << frobNormOfError << endl;
    }

    if( g.Rank() == 0 )
        cout << "  Testing if A = QR..." << endl;

    // Form Q R
    DistMatrix<F> U( A );
    MakeTriangular( UPPER, U );
    ApplyPackedReflectors
    ( LEFT, LOWER, VERTICAL, BACKWARD, UNCONJUGATED, 0, A, t, U );

    // Form Q R - A
    Axpy( F(-1), AOrig, U );
    
    const R oneNormOfA = OneNorm( AOrig );
    const R infNormOfA = InfinityNorm( AOrig );
    const R frobNormOfA = FrobeniusNorm( AOrig );
    oneNormOfError = OneNorm( U );
    infNormOfError = InfinityNorm( U );
    frobNormOfError = FrobeniusNorm( U );
    if( g.Rank() == 0 )
    {
        cout << "    ||A||_1       = " << oneNormOfA << "\n"
             << "    ||A||_oo      = " << infNormOfA << "\n"
             << "    ||A||_F       = " << frobNormOfA << "\n"
             << "    ||A - QR||_1  = " << oneNormOfError << "\n"
             << "    ||A - QR||_oo = " << infNormOfError << "\n"
             << "    ||A - QR||_F  = " << frobNormOfError << endl;
    }
}

template<typename F>
void TestQR( bool testCorrectness, bool print, int m, int n, const Grid& g )
{
    DistMatrix<F> A(g), AOrig(g);
    DistMatrix<F,MD,STAR> t(g);

    Uniform( A, m, n );
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
        cout << "  Starting QR factorization...";
        cout.flush();
    }
    mpi::Barrier( g.Comm() );
    const double startTime = mpi::Time();
    QR( A, t );
    mpi::Barrier( g.Comm() );
    const double runTime = mpi::Time() - startTime;
    const double mD = double(m);
    const double nD = double(n);
    const double realGFlops = (8.*mD*nD*nD - 8./3.*nD*nD*nD)/(1.e9*runTime);
    const double gFlops = ( IsComplex<F>::val ? 4*realGFlops : realGFlops );
    if( g.Rank() == 0 )
    {
        cout << "DONE. " << endl
             << "  Time = " << runTime << " seconds. GFlops = " 
             << gFlops << endl;
    }
    if( print )
        Print( A, "A after factorization" );
    if( testCorrectness )
        TestCorrectness( A, t, AOrig );
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
        const int m = Input("--height","height of matrix",100);
        const int n = Input("--width","width of matrix",100);
        const int nb = Input("--nb","algorithmic blocksize",96);
        const bool testCorrectness = Input
            ("--correctness","test correctness?",true);
        const bool print = Input("--print","print matrices?",false);
        ProcessInput();
        PrintInputReport();

        if( r == 0 )
            r = Grid::FindFactor( commSize );
        const Grid g( comm, r );
        SetBlocksize( nb );
        ComplainIfDebug();
        if( commRank == 0 )
            cout << "Will test QR" << endl;

        if( commRank == 0 )
        {
            cout << "---------------------\n"
                 << "Testing with doubles:\n"
                 << "---------------------" << endl;
        }
        TestQR<double>( testCorrectness, print, m, n, g );

        if( commRank == 0 )
        {
            cout << "--------------------------------------\n"
                 << "Testing with double-precision complex:\n"
                 << "--------------------------------------" << endl;
        }
        TestQR<Complex<double> >( testCorrectness, print, m, n, g );
    }
    catch( exception& e ) { ReportException(e); }

    Finalize();
    return 0;
}
