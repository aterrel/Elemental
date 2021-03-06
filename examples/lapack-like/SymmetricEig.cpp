/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
// NOTE: It is possible to simply include "elemental.hpp" instead
#include "elemental-lite.hpp"
#include "elemental/lapack-like/HermitianEig/Sort.hpp"
#include "elemental/lapack-like/Trace.hpp"
using namespace std;
using namespace elem;

// Typedef our real type to 'R' for convenience
typedef double R;

int
main( int argc, char* argv[] )
{
    // Initialize Elemental. The full routine is elem::Initialize.
    // It detects whether or not you have already initialized MPI and 
    // does so if necessary.
    Initialize( argc, argv );

    // Surround the Elemental calls with try/catch statements in order to 
    // safely handle any exceptions that were thrown during execution.
    try 
    {
        const int n = Input("--size","matrix size",100);
        const bool print = Input("--print","print matrices?",false);
        ProcessInput();
        PrintInputReport();

        // Create a 2d process grid from a communicator. In our case, it is
        // MPI_COMM_WORLD. There is another constructor that allows you to 
        // specify the grid dimensions, Grid g( comm, r ), which creates a
        // grid of height r.
        Grid g( mpi::COMM_WORLD );
    
        // Create an n x n real distributed matrix.
        // We distribute the matrix using grid 'g'.
        //
        // There are quite a few available constructors, including ones that 
        // allow you to pass in your own local buffer and to specify the 
        // distribution alignments (i.e., which process row and column owns the
        // top-left element)
        DistMatrix<R> H( n, n, g );

        // Fill the matrix since we did not pass in a buffer. 
        //
        // We will fill entry (i,j) with the value i+j so that 
        // the global matrix is Hermitian. However, only one triangle of the 
        // matrix actually needs to be filled, the symmetry can be implicit.
        //
        const int colShift = H.ColShift(); // first row we own
        const int rowShift = H.RowShift(); // first col we own
        const int colStride = H.ColStride();
        const int rowStride = H.RowStride();
        const int localHeight = H.LocalHeight();
        const int localWidth = H.LocalWidth();
        for( int jLocal=0; jLocal<localWidth; ++jLocal )
        {
            for( int iLocal=0; iLocal<localHeight; ++iLocal )
            {
                // Our process owns the rows colShift:colStride:n,
                //           and the columns rowShift:rowStride:n
                const int i = colShift + iLocal*colStride;
                const int j = rowShift + jLocal*rowStride;
                H.SetLocal( iLocal, jLocal, i+j );
            }
        }
        // Alternatively, we could have sequentially filled the matrix with 
        // for( int j=0; j<A.Width(); ++j )
        //   for( int i=0; i<A.Height(); ++i )
        //     A.Set( i, j, i+j );
        //
        // More convenient interfaces are being investigated.
        //
        if( print )
            Print( H, "H" );

        // Print its trace
        const R trace = Trace( H );
        if( mpi::WorldRank() == 0 )
            std::cout << "Tr(H) = " << trace << std::endl;

        // Call the eigensolver. We first create an empty eigenvector 
        // matrix, X, and an eigenvalue column vector, w[VR,* ]
        DistMatrix<R,VR,STAR> w( g );
        DistMatrix<R> X( g );
        // Optional: set blocksizes and algorithmic choices here. See the 
        //           'Tuning' section of the README for details.
        HermitianEig( LOWER, H, w, X ); // only access lower half of H
        if( print )
        {
            Print( w, "Eigenvalues of H" );
            Print( X, "Eigenvectors of H" );
        }

        // Sort the eigensolution,
        hermitian_eig::Sort( w, X );

        if( print )
        {
            Print( w, "Sorted eigenvalues of H" );
            Print( X, "Sorted eigenvectors of H" );
        }
    }
    catch( exception& e ) { ReportException(e); }

    Finalize();
    return 0;
}
