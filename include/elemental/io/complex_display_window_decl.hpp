/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef IO_COMPLEXDISPLAYWINDOW_DECL_HPP
#define IO_COMPLEXDISPLAYWINDOW_DECL_HPP

// Not currently guarded due to CMake MOC handling requiring extra flags
//#ifdef HAVE_QT5

#include <QPushButton>
#include <QScrollArea>
#include <QWidget>

#include "elemental-lite.hpp"
#include "elemental/io/display_widget_decl.hpp"

namespace elem {

// Unfortunately Q_OBJECT does not support templates...
class ComplexDisplayWindow : public QWidget
{
    Q_OBJECT

public:
    ComplexDisplayWindow( QWidget* parent=0 );    
    ~ComplexDisplayWindow();

    void Display
    ( const Matrix<Complex<double> >* A, 
      QString title=QString("Default title") );
    void Display
    ( const Matrix<Complex<double> >* A, 
      double minRealVal, double maxRealVal,
      double minImagVal, double maxImagVal,
      QString title=QString("Default title") );

private:
    QScrollArea *realScroll_, *imagScroll_;
    DisplayWidget<Complex<double> > *realDisplay_, *imagDisplay_;
    const Matrix<Complex<double> > *matrix_;

public slots:
    void SaveReal();
    void SaveImag();
    void SetScale( bool global );
};

} // namespace elem

//#endif // ifdef HAVE_QT5

#endif // ifndef IO_COMPLEXDISPLAYWINDOW_DECL_HPP
