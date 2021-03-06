/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
// NOTE: It is possible to simply include "elemental.hpp" instead
#include "elemental-lite.hpp"
#include "elemental/blas-like/level1/DiagonalScale.hpp"
#include "elemental/lapack-like/Norm/Frobenius.hpp"
#include "elemental/lapack-like/Norm/Infinity.hpp"
#include "elemental/lapack-like/Norm/Max.hpp"
#include "elemental/lapack-like/Norm/One.hpp"
#include "elemental/lapack-like/Norm/TwoLowerBound.hpp"
#include "elemental/lapack-like/Norm/TwoUpperBound.hpp"
#include "elemental/lapack-like/SVD.hpp"
#include "elemental/matrices.hpp"
using namespace std;
using namespace elem;

// Typedef our real and complex types to 'R' and 'C' for convenience
typedef double R;
typedef Complex<R> C;

int
main( int argc, char* argv[] )
{
    Initialize( argc, argv );

    mpi::Comm comm = mpi::COMM_WORLD;
    const int commRank = mpi::CommRank( comm );

    enum TestType { FOURIER=0, HILBERT=1, IDENTITY=2, ONES=3, ONE_TWO_ONE=4,
                    UNIFORM=5, WILKINSON=6, ZEROS=7 }; 

    try 
    {
        const int k = Input("--size","problem size",100);
        ProcessInput();
        PrintInputReport();

        Matrix<C> A, U, V;
        Matrix<R> s;

        for( int test=0; test<16; ++test )
        {
            int n;
            const TestType testType = TestType(test/2);
            const bool useQR = test % 2;
            const std::string qrString = ( useQR ? "with QR:" : "with D&C:" );
            switch( testType )
            {
            case FOURIER:     
                if( commRank == 0 ) 
                    std::cout << "Testing Fourier " 
                              << qrString << std::endl;
                n = k;
                Fourier( A, n ); 
                break;
            case HILBERT:     
                if( commRank == 0 )
                    std::cout << "Testing Hilbert " << qrString << std::endl;
                n = k;
                Hilbert( A, n ); 
                break;
            case IDENTITY:    
                if( commRank == 0 )
                    std::cout << "Testing Identity " << qrString << std::endl;
                n = k;
                Identity( A, n, n ); 
                break;
            case ONES:        
                if( commRank == 0 )
                    std::cout << "Testing Ones " << qrString << std::endl;
                n = k;
                Ones( A, n, n ); 
                break;
            case ONE_TWO_ONE: 
                if( commRank == 0 )
                    std::cout << "Testing OneTwoOne " << qrString << std::endl;
                n = k;
                OneTwoOne( A, n ); 
                break;
            case UNIFORM:     
                if( commRank == 0 )
                    std::cout << "Testing Uniform " << qrString << std::endl;
                n = k;
                Uniform( A, n, n ); 
                break;
            case WILKINSON:   
                if( commRank == 0 )
                    std::cout << "Testing Wilkinson " << qrString << std::endl;
                Wilkinson( A, k ); 
                n = 2*k+1;
                break;
            case ZEROS:       
                if( commRank == 0 )
                    std::cout << "Testing Zeros " << qrString << std::endl;
                n = k;
                Zeros( A, n, n ); 
                break;
            };

            // Make a copy of A and then perform the SVD
            U = A;
            SVD( U, s, V, useQR );

            const R twoNormOfA = MaxNorm( s );
            const R maxNormOfA = MaxNorm( A );
            const R oneNormOfA = OneNorm( A );
            const R infNormOfA = InfinityNorm( A );
            const R frobNormOfA = FrobeniusNorm( A );
            const R lowerBound = TwoNormLowerBound( A );
            const R upperBound = TwoNormUpperBound( A );

            DiagonalScale( RIGHT, NORMAL, s, U );
            Gemm( NORMAL, ADJOINT, C(-1), U, V, C(1), A );
            const R maxNormOfE = MaxNorm( A );
            const R oneNormOfE = OneNorm( A );
            const R infNormOfE = InfinityNorm( A );
            const R frobNormOfE = FrobeniusNorm( A );
            const R epsilon = lapack::MachineEpsilon<R>();
            const R scaledResidual = frobNormOfE/(n*epsilon*twoNormOfA);

            if( commRank == 0 )
            {
                cout << "||A||_max   = " << maxNormOfA << "\n"
                     << "||A||_1     = " << oneNormOfA << "\n"
                     << "||A||_oo    = " << infNormOfA << "\n"
                     << "||A||_F     = " << frobNormOfA << "\n"
                     << "\n"
                     << "lower bound = " << lowerBound << "\n"
                     << "||A||_2     = " << twoNormOfA << "\n"
                     << "upper bound = " << upperBound << "\n"
                     << "\n"
                     << "||A - U Sigma V^H||_max = " << maxNormOfE << "\n"
                     << "||A - U Sigma V^H||_1   = " << oneNormOfE << "\n"
                     << "||A - U Sigma V^H||_oo  = " << infNormOfE << "\n"
                     << "||A - U Sigma V^H||_F   = " << frobNormOfE << "\n"
                     << "||A - U Sigma V_H||_F / (n eps ||A||_2) = " 
                     << scaledResidual << "\n" << endl;
            }
        }
    }
    catch( exception& e ) { ReportException(e); }

    Finalize();
    return 0;
}
