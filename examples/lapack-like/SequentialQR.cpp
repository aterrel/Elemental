/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
// NOTE: It is possible to simply include "elemental.hpp" instead
#include "elemental-lite.hpp"
#include "elemental/lapack-like/QR.hpp"
#include "elemental/lapack-like/Norm/Frobenius.hpp"
#include "elemental/matrices/Identity.hpp"
#include "elemental/matrices/Uniform.hpp"
using namespace std;
using namespace elem;

typedef double Real;
typedef Complex<Real> C;

int
main( int argc, char* argv[] )
{
    Initialize( argc, argv );

    try 
    {
        const int m = Input("--height","height of matrix",100);
        const int n = Input("--width","width of matrix",100);
        ProcessInput();
        PrintInputReport();

        Matrix<C> A;
        Uniform( A, m, n );
        const Real frobA = FrobeniusNorm( A );

        // Compute the QR decomposition of A, but do not overwrite A
        Matrix<C> Q( A ), R;
        qr::Explicit( Q, R );

        // Check the error in the QR factorization, || A - Q R ||_F / || A ||_F
        Matrix<C> E( A );
        Gemm( NORMAL, NORMAL, C(-1), Q, R, C(1), E );
        const Real frobQR = FrobeniusNorm( E );

        // Check the numerical orthogonality of Q, || I - Q^H Q ||_F / || A ||_F
        const int k = std::min(m,n);
        Identity( E, k, k );
        Herk( LOWER, ADJOINT, C(-1), Q, C(1), E );
        const Real frobOrthog = HermitianFrobeniusNorm( LOWER, E );

        if( mpi::WorldRank() == 0 )
        {
            std::cout << "|| A ||_F = " << frobA << "\n"
                      << "|| A - Q R ||_F / || A ||_F   = "
                      << frobQR/frobA << "\n"
                      << "|| I - Q^H Q ||_F / || A ||_F = "
                      << frobOrthog/frobA << "\n"
                      << std::endl;
        }
    }
    catch( exception& e ) { ReportException(e); }

    Finalize();
    return 0;
}
