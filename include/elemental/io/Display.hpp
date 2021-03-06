/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef IO_DISPLAY_HPP
#define IO_DISPLAY_HPP

#ifdef HAVE_QT5
#include "elemental/io/display_window_decl.hpp"
#include "elemental/io/complex_display_window_decl.hpp"
#include "elemental/io/display_widget_impl.hpp"
#include <QApplication>
#endif

namespace elem {

inline void
ProcessEvents( int numMsecs )
{
#ifdef HAVE_QT5
    QCoreApplication::instance()->processEvents
    ( QEventLoop::AllEvents, numMsecs );
#endif
}

template<typename T>
inline void
Display( const Matrix<T>& A, std::string title="Default" )
{
#ifndef RELEASE
    CallStackEntry entry("Display");
#endif
#ifdef HAVE_QT5
    // Convert A to double-precision since Qt's MOC does not support templates
    const int m = A.Height();
    const int n = A.Width();
    Matrix<double>* ADouble = new Matrix<double>( m, n );
    for( int j=0; j<n; ++j )
        for( int i=0; i<m; ++i )
            ADouble->Set( i, j, double(A.Get(i,j)) );

    QString qTitle = QString::fromStdString( title );
    DisplayWindow* displayWindow = new DisplayWindow;
    displayWindow->Display( ADouble, qTitle );
    displayWindow->show();

    // Spend at most 200 milliseconds rendering
    ProcessEvents( 200 );
#else
    Print( A, title );
#endif
}

template<typename T>
inline void
Display( const Matrix<Complex<T> >& A, std::string title="Default" )
{
#ifndef RELEASE
    CallStackEntry entry("Display");
#endif
#ifdef HAVE_QT5
    // Convert A to double-precision since Qt's MOC does not support templates
    const int m = A.Height();
    const int n = A.Width();
    Matrix<Complex<double> >* ADouble = new Matrix<Complex<double> >( m, n );
    for( int j=0; j<n; ++j )
    {
        for( int i=0; i<m; ++i )
        {
            const Complex<T> alpha = A.Get(i,j);
            const Complex<double> alphaDouble = 
                Complex<double>(alpha.real,alpha.imag); 
            ADouble->Set( i, j, alphaDouble );
        }
    }

    QString qTitle = QString::fromStdString( title );
    ComplexDisplayWindow* displayWindow = new ComplexDisplayWindow;
    displayWindow->Display( ADouble, qTitle );
    displayWindow->show();

    // Spend at most 200 milliseconds rendering
    ProcessEvents( 200 );
#else
    Print( A, title );
#endif
}

template<typename T,Distribution U,Distribution V>
inline void
Display( const DistMatrix<T,U,V>& A, std::string title="Default" )
{
#ifndef RELEASE
    CallStackEntry entry("Display");
#endif
#ifdef HAVE_QT5
    DistMatrix<T,CIRC,CIRC> A_CIRC_CIRC( A );
    if( A.Grid().Rank() == A_CIRC_CIRC.Root() )
        Display( A_CIRC_CIRC.Matrix(), title );
#else
    Print( A, title );
#endif
}

// If already in [* ,* ] or [o ,o ] distributions, no copy is needed
template<typename T>
inline void
Display( const DistMatrix<T,STAR,STAR>& A, std::string title="Default" )
{
#ifndef RELEASE
    CallStackEntry entry("Display");
#endif
#ifdef HAVE_QT5
    if( A.Grid().Rank() == 0 )
        Display( A.LockedMatrix(), title );
#else
    Print( A, title );
#endif
}
template<typename T>
inline void
Display( const DistMatrix<T,CIRC,CIRC>& A, std::string title="Default" )
{
#ifndef RELEASE
    CallStackEntry entry("Display");
#endif
#ifdef HAVE_QT5
    if( A.Grid().Rank() == A.Root() )
        Display( A.LockedMatrix(), title );
#else
    Print( A, title );
#endif
}

} // namespace elem

#endif // ifndef IO_DISPLAY_HPP
