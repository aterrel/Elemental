/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef LAPACK_NORM_FROBENIUS_HPP
#define LAPACK_NORM_FROBENIUS_HPP

namespace elem {

template<typename F> 
inline BASE(F)
FrobeniusNorm( const Matrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("FrobeniusNorm");
#endif
    typedef BASE(F) R;
    R scale = 0;
    R scaledSquare = 1;
    const int width = A.Width();
    const int height = A.Height();
    for( int j=0; j<width; ++j )
    {
        for( int i=0; i<height; ++i )
        {
            const R alphaAbs = Abs(A.Get(i,j));
            if( alphaAbs != 0 )
            {
                if( alphaAbs <= scale )
                {
                    const R relScale = alphaAbs/scale;
                    scaledSquare += relScale*relScale;
                }
                else
                {
                    const R relScale = scale/alphaAbs;
                    scaledSquare = scaledSquare*relScale*relScale + 1;
                    scale = alphaAbs;
                }
            }
        }
    }
    return scale*Sqrt(scaledSquare);
}

template<typename F>
inline BASE(F)
HermitianFrobeniusNorm( UpperOrLower uplo, const Matrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("HermitianFrobeniusNorm");
#endif
    if( A.Height() != A.Width() )
        throw std::logic_error("Hermitian matrices must be square.");

    typedef BASE(F) R;
    R scale = 0;
    R scaledSquare = 1;
    const int height = A.Height();
    const int width = A.Width();
    if( uplo == UPPER )
    {
        for( int j=0; j<width; ++j )
        {
            for( int i=0; i<j; ++i )
            {
                const R alphaAbs = Abs(A.Get(i,j));
                if( alphaAbs != 0 )
                {
                    if( alphaAbs <= scale )
                    {
                        const R relScale = alphaAbs/scale;
                        scaledSquare += 2*relScale*relScale;
                    }
                    else
                    {
                        const R relScale = scale/alphaAbs;
                        scaledSquare = scaledSquare*relScale*relScale + 2;
                        scale = alphaAbs;
                    }
                }
            }
            const R alphaAbs = Abs(A.Get(j,j));
            if( alphaAbs != 0 )
            {
                if( alphaAbs <= scale )
                {
                    const R relScale = alphaAbs/scale;
                    scaledSquare += relScale*relScale;
                }
                else
                {
                    const R relScale = scale/alphaAbs;
                    scaledSquare = scaledSquare*relScale*relScale + 1;
                    scale = alphaAbs;
                }
            }
        }
    }
    else
    {
        for( int j=0; j<width; ++j )
        {
            for( int i=j+1; i<height; ++i )
            {
                const R alphaAbs = Abs(A.Get(i,j));
                if( alphaAbs != 0 )
                {
                    if( alphaAbs <= scale )
                    {
                        const R relScale = alphaAbs/scale;
                        scaledSquare += 2*relScale*relScale;
                    }
                    else
                    {
                        const R relScale = scale/alphaAbs;
                        scaledSquare = scaledSquare*relScale*relScale + 2;
                        scale = alphaAbs;
                    }
                }
            }
            const R alphaAbs = Abs(A.Get(j,j));
            if( alphaAbs != 0 )
            {
                if( alphaAbs <= scale )
                {
                    const R relScale = alphaAbs/scale;
                    scaledSquare += relScale*relScale;
                }
                else
                {
                    const R relScale = scale/alphaAbs;
                    scaledSquare = scaledSquare*relScale*relScale + 1;
                    scale = alphaAbs;
                }
            }
        }
    }
    return scale*Sqrt(scaledSquare);
}

template<typename F>
inline BASE(F)
SymmetricFrobeniusNorm( UpperOrLower uplo, const Matrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("SymmetricFrobeniusNorm");
#endif
    return HermitianFrobeniusNorm( uplo, A );
}

template<typename F,Distribution U,Distribution V> 
inline BASE(F)
FrobeniusNorm( const DistMatrix<F,U,V>& A )
{
#ifndef RELEASE
    CallStackEntry entry("FrobeniusNorm");
#endif
    typedef BASE(F) R;
    R localScale = 0;
    R localScaledSquare = 1;
    const int localHeight = A.LocalHeight();
    const int localWidth = A.LocalWidth();
    for( int jLoc=0; jLoc<localWidth; ++jLoc )
    {
        for( int iLoc=0; iLoc<localHeight; ++iLoc )
        {
            const R alphaAbs = Abs(A.GetLocal(iLoc,jLoc));
            if( alphaAbs != 0 )
            {
                if( alphaAbs <= localScale )
                {
                    const R relScale = alphaAbs/localScale;
                    localScaledSquare += relScale*relScale;
                }
                else
                {
                    const R relScale = localScale/alphaAbs;
                    localScaledSquare = localScaledSquare*relScale*relScale + 1;
                    localScale = alphaAbs; 
                }
            }
        }
    }

    // Find the maximum relative scale
    R scale;
    mpi::Comm comm = ReduceComm<U,V>( A.Grid() );
    mpi::AllReduce( &localScale, &scale, 1, mpi::MAX, comm );

    R norm = 0;
    if( scale != 0 )
    {
        // Equilibrate our local scaled sum to the maximum scale
        R relScale = localScale/scale;
        localScaledSquare *= relScale*relScale;

        // The scaled square is now simply the sum of the local contributions
        R scaledSquare;
        mpi::AllReduce( &localScaledSquare, &scaledSquare, 1, mpi::SUM, comm );
        norm = scale*Sqrt(scaledSquare);
    }
    return norm;
}

template<typename F>
inline BASE(F)
HermitianFrobeniusNorm
( UpperOrLower uplo, const DistMatrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("HermitianFrobeniusNorm");
#endif
    if( A.Height() != A.Width() )
        throw std::logic_error("Hermitian matrices must be square.");

    const int r = A.Grid().Height();
    const int c = A.Grid().Width();
    const int colShift = A.ColShift();
    const int rowShift = A.RowShift();

    typedef BASE(F) R;
    R localScale = 0;
    R localScaledSquare = 1;
    const int localWidth = A.LocalWidth();
    if( uplo == UPPER )
    {
        for( int jLoc=0; jLoc<localWidth; ++jLoc )
        {
            int j = rowShift + jLoc*c;
            int numUpperRows = Length(j+1,colShift,r);
            for( int iLoc=0; iLoc<numUpperRows; ++iLoc )
            {
                int i = colShift + iLoc*r;
                const R alphaAbs = Abs(A.GetLocal(iLoc,jLoc));
                if( alphaAbs != 0 )
                {
                    if( alphaAbs <= localScale )
                    {
                        const R relScale = alphaAbs/localScale;
                        if( i != j )
                            localScaledSquare += 2*relScale*relScale;
                        else
                            localScaledSquare += relScale*relScale;
                    }
                    else
                    {
                        const R relScale = localScale/alphaAbs;
                        if( i != j )
                            localScaledSquare =
                                localScaledSquare*relScale*relScale + 2;
                        else
                            localScaledSquare =
                                localScaledSquare*relScale*relScale + 1;
                        localScale = alphaAbs;
                    }
                }
            }
        }
    }
    else
    {
        for( int jLoc=0; jLoc<localWidth; ++jLoc )
        {
            int j = rowShift + jLoc*c;
            int numStrictlyUpperRows = Length(j,colShift,r);
            for( int iLoc=numStrictlyUpperRows;
                 iLoc<A.LocalHeight(); ++iLoc )
            {
                int i = colShift + iLoc*r;
                const R alphaAbs = Abs(A.GetLocal(iLoc,jLoc));
                if( alphaAbs != 0 )
                {
                    if( alphaAbs <= localScale )
                    {
                        const R relScale = alphaAbs/localScale;
                        if( i != j )
                            localScaledSquare += 2*relScale*relScale;
                        else
                            localScaledSquare += relScale*relScale;
                    }
                    else
                    {
                        const R relScale = localScale/alphaAbs;
                        if( i != j )
                            localScaledSquare =
                                localScaledSquare*relScale*relScale + 2;
                        else
                            localScaledSquare =
                                localScaledSquare*relScale*relScale + 1;
                        localScale = alphaAbs;
                    }
                }
            }
        }
    }

    // Find the maximum relative scale
    R scale;
    mpi::AllReduce( &localScale, &scale, 1, mpi::MAX, A.Grid().VCComm() );

    R norm = 0;
    if( scale != 0 )
    {
        // Equilibrate our local scaled sum to the maximum scale
        R relScale = localScale/scale;
        localScaledSquare *= relScale*relScale;

        // The scaled square is now simply the sum of the local contributions
        R scaledSquare;
        mpi::AllReduce
        ( &localScaledSquare, &scaledSquare, 1, mpi::SUM, A.Grid().VCComm() );

        norm = scale*Sqrt(scaledSquare);
    }
    return norm;
}

template<typename F,Distribution U,Distribution V>
inline BASE(F)
SymmetricFrobeniusNorm( UpperOrLower uplo, const DistMatrix<F,U,V>& A )
{
#ifndef RELEASE
    CallStackEntry entry("SymmetricFrobeniusNorm");
#endif
    return HermitianFrobeniusNorm( uplo, A );
}

} // namespace elem

#endif // ifndef LAPACK_NORM_FROBENIUS_HPP
