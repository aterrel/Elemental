/*
   Copyright (c) 2009-2012, Jack Poulson
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
#include "elemental.hpp"
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
        MpiArgs args( argc, argv, comm );
        const int k = args.Optional("--size",100,"problem size");
        args.Process();

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
                    std::cout << "Testing DiscreteFourier " 
                              << qrString << std::endl;
                n = k;
                DiscreteFourier( n, A ); 
                break;
            case HILBERT:     
                if( commRank == 0 )
                    std::cout << "Testing Hilbert " << qrString << std::endl;
                n = k;
                Hilbert( n, A ); 
                break;
            case IDENTITY:    
                if( commRank == 0 )
                    std::cout << "Testing Identity " << qrString << std::endl;
                n = k;
                Identity( n, n, A ); 
                break;
            case ONES:        
                if( commRank == 0 )
                    std::cout << "Testing Ones " << qrString << std::endl;
                n = k;
                Ones( n, n, A ); 
                break;
            case ONE_TWO_ONE: 
                if( commRank == 0 )
                    std::cout << "Testing OneTwoOne " << qrString << std::endl;
                n = k;
                OneTwoOne( n, A ); 
                break;
            case UNIFORM:     
                if( commRank == 0 )
                    std::cout << "Testing Uniform " << qrString << std::endl;
                n = k;
                Uniform( n, n, A ); 
                break;
            case WILKINSON:   
                if( commRank == 0 )
                    std::cout << "Testing Wilkinson " << qrString << std::endl;
                Wilkinson( k, A ); 
                n = 2*k+1;
                break;
            case ZEROS:       
                if( commRank == 0 )
                    std::cout << "Testing Zeros " << qrString << std::endl;
                n = k;
                Zeros( n, n, A ); 
                break;
            };

            // Make a copy of A and then perform the SVD
            U = A;
            SVD( U, s, V, useQR );

            const R twoNormOfA = Norm( s, MAX_NORM );

            const R maxNormOfA = Norm( A, MAX_NORM );
            const R oneNormOfA = Norm( A, ONE_NORM );
            const R infNormOfA = Norm( A, INFINITY_NORM );
            const R frobNormOfA = Norm( A, FROBENIUS_NORM );
            const R lowerBound = TwoNormLowerBound( A );
            const R upperBound = TwoNormUpperBound( A );

            DiagonalScale( RIGHT, NORMAL, s, U );
            Gemm( NORMAL, ADJOINT, C(-1), U, V, C(1), A );
            const R maxNormOfE = Norm( A, MAX_NORM );
            const R oneNormOfE = Norm( A, ONE_NORM );
            const R infNormOfE = Norm( A, INFINITY_NORM );
            const R frobNormOfE = Norm( A, FROBENIUS_NORM );
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
    catch( ArgException& e )
    {
        // There is nothing to do
    }
    catch( exception& e )
    {
        ostringstream os;
        os << "Process " << commRank << " caught exception with message: "
           << e.what() << endl;
        cerr << os.str();
#ifndef RELEASE
        DumpCallStack();
#endif
    }

    Finalize();
    return 0;
}