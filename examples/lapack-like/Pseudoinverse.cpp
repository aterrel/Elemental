/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
// NOTE: It is possible to simply include "elemental.hpp" instead
#include "elemental-lite.hpp"
#include "elemental/lapack-like/Norm/Frobenius.hpp"
#include "elemental/lapack-like/Pseudoinverse.hpp"
#include "elemental/matrices/Uniform.hpp"
using namespace std;
using namespace elem;

// Typedef our real and complex types to 'R' and 'C' for convenience
typedef double R;
typedef Complex<R> C;

int
main( int argc, char* argv[] )
{
    Initialize( argc, argv );

    try 
    {
        const int m = Input("--height","height of matrix",100);
        const int n = Input("--width","width of matrix",100);
        const bool print = Input("--print","print matrices?",false);
        ProcessInput();
        PrintInputReport();

        Grid g( mpi::COMM_WORLD );
        DistMatrix<C> A( g );
        Uniform( A, m, n );

        // Compute the pseudoinverseof A (but do not overwrite A)
        DistMatrix<C> pinvA( A );
        Pseudoinverse( pinvA );

        if( print )
        {
            Print( A, "A" );
            Print( pinvA, "pinv(A)" );
        }

        const R frobOfA = FrobeniusNorm( A );
        const R frobOfPinvA = FrobeniusNorm( pinvA );

        if( mpi::WorldRank() == 0 )
        {
            cout << "||   A   ||_F =  " << frobOfA << "\n"
                 << "||pinv(A)||_F =  " << frobOfPinvA << "\n"
                 << endl;
        }
    }
    catch( exception& e ) { ReportException(e); }

    Finalize();
    return 0;
}
