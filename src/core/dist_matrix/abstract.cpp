/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "elemental-lite.hpp"

namespace elem {

template<typename T,typename Int>
AbstractDistMatrix<T,Int>::AbstractDistMatrix( const elem::Grid& grid )
: viewType_(OWNER),
  height_(0), width_(0), 
  auxMemory_(), 
  matrix_(0,0,true), 
  constrainedColAlignment_(false), 
  constrainedRowAlignment_(false),
  colAlignment_(0), rowAlignment_(0),
  colShift_(0), rowShift_(0),
  grid_(&grid)
{ }

template<typename T,typename Int>
AbstractDistMatrix<T,Int>::~AbstractDistMatrix() 
{ }

#ifndef RELEASE
template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::AssertNotLocked() const
{
    if( Locked() )
        throw std::logic_error
        ("Assertion that matrix not be a locked view failed");
}

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::AssertNotStoringData() const
{
    if( matrix_.MemorySize() > 0 )
        throw std::logic_error
        ("Assertion that matrix not be storing data failed");
}

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::AssertValidEntry
( Int i, Int j ) const
{
    if( i < 0 || i >= Height() || j < 0 || j >= Width() )
    {
        std::ostringstream msg;
        msg << "Entry (" << i << "," << j << ") is out of bounds of "
            << Height() << " x " << Width() << " matrix.";
        throw std::logic_error( msg.str().c_str() );
    }
}

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::AssertValidSubmatrix
( Int i, Int j, Int height, Int width ) const
{
    if( i < 0 || j < 0 )
        throw std::logic_error("Indices of submatrix were negative");
    if( height < 0 || width < 0 )
        throw std::logic_error("Dimensions of submatrix were negative");
    if( (i+height) > Height() || (j+width) > Width() )
    {
        std::ostringstream msg;
        msg << "Submatrix is out of bounds: accessing up to (" << i+height-1
            << "," << j+width-1 << ") of " << Height() << " x "
            << Width() << " matrix.";
        throw std::logic_error( msg.str().c_str() );
    }
}

template<typename T,typename Int> 
void
AbstractDistMatrix<T,Int>::AssertSameGrid( const elem::Grid& grid ) const
{
    if( Grid() != grid )
        throw std::logic_error("Assertion that grids match failed");
}

template<typename T,typename Int> 
void
AbstractDistMatrix<T,Int>::AssertSameSize( int height, int width ) const
{
    if( Height() != height || Width() != width )
        throw std::logic_error
        ("Assertion that matrices be the same size failed");
}

template<typename T,typename Int> 
void
AssertConforming1x2
( const AbstractDistMatrix<T,Int>& AL, 
  const AbstractDistMatrix<T,Int>& AR )
{
    if( AL.Height() != AR.Height() )    
    {
        std::ostringstream msg;
        msg << "1x2 not conformant. Left is " << AL.Height() << " x " 
            << AL.Width() << ", right is " << AR.Height() << " x " 
            << AR.Width();
        throw std::logic_error( msg.str().c_str() );
    }
    if( AL.ColAlignment() != AR.ColAlignment() )
        throw std::logic_error("1x2 is misaligned");
}

template<typename T,typename Int> 
void
AssertConforming2x1
( const AbstractDistMatrix<T,Int>& AT,
  const AbstractDistMatrix<T,Int>& AB )
{
    if( AT.Width() != AB.Width() )
    {
        std::ostringstream msg;        
        msg << "2x1 is not conformant. Top is " << AT.Height() << " x " 
            << AT.Width() << ", bottom is " << AB.Height() << " x " 
            << AB.Width();
        throw std::logic_error( msg.str().c_str() );
    }
    if( AT.RowAlignment() != AB.RowAlignment() )
        throw std::logic_error("2x1 is not aligned");
}

template<typename T,typename Int> 
void
AssertConforming2x2
( const AbstractDistMatrix<T,Int>& ATL, 
  const AbstractDistMatrix<T,Int>& ATR,
  const AbstractDistMatrix<T,Int>& ABL, 
  const AbstractDistMatrix<T,Int>& ABR ) 
{
    if( ATL.Width() != ABL.Width() || ATR.Width() != ABR.Width() ||
        ATL.Height() != ATR.Height() || ABL.Height() != ABR.Height() )
    {
        std::ostringstream msg;
        msg << "2x2 is not conformant: " << std::endl
            << "  TL is " << ATL.Height() << " x " << ATL.Width() << std::endl
            << "  TR is " << ATR.Height() << " x " << ATR.Width() << std::endl
            << "  BL is " << ABL.Height() << " x " << ABL.Width() << std::endl
            << "  BR is " << ABR.Height() << " x " << ABR.Width();
        throw std::logic_error( msg.str().c_str() );
    }
    if( ATL.ColAlignment() != ATR.ColAlignment() ||
        ABL.ColAlignment() != ABR.ColAlignment() ||
        ATL.RowAlignment() != ABL.RowAlignment() ||
        ATR.RowAlignment() != ABR.RowAlignment() )
        throw std::logic_error
        ("2x2 set of matrices must aligned to combine");
}
#endif // RELEASE

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::Align( Int colAlignment, Int rowAlignment )
{ 
#ifndef RELEASE
    CallStackEntry entry("AbstractDistMatrix::Align");    
#endif
    Empty();
    colAlignment_ = colAlignment;
    rowAlignment_ = rowAlignment;
    constrainedColAlignment_ = true;
    constrainedRowAlignment_ = true;
    SetShifts();
}

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::AlignCols( Int colAlignment )
{ 
#ifndef RELEASE
    CallStackEntry entry("AbstractDistMatrix::AlignCols"); 
#endif
    EmptyData();
    colAlignment_ = colAlignment;
    constrainedColAlignment_ = true;
    SetShifts();
}

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::AlignRows( Int rowAlignment )
{ 
#ifndef RELEASE
    CallStackEntry entry("AbstractDistMatrix::AlignRows"); 
#endif
    EmptyData();
    rowAlignment_ = rowAlignment;
    constrainedRowAlignment_ = true;
    SetShifts();
}

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::AlignWith( const elem::DistData<Int>& data )
{ SetGrid( *data.grid ); }

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::AlignWith( const AbstractDistMatrix<T,Int>& A )
{ AlignWith( A.DistData() ); }

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::AlignColsWith( const elem::DistData<Int>& data )
{ 
    EmptyData(); 
    colAlignment_ = 0; 
    constrainedColAlignment_ = false; 
    SetShifts(); 
}

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::AlignColsWith( const AbstractDistMatrix<T,Int>& A )
{ AlignColsWith( A.DistData() ); }

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::AlignRowsWith( const elem::DistData<Int>& data )
{ 
    EmptyData(); 
    rowAlignment_ = 0; 
    constrainedRowAlignment_ = false;
    SetShifts(); 
}

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::AlignRowsWith( const AbstractDistMatrix<T,Int>& A )
{ AlignRowsWith( A.DistData() ); }

template<typename T,typename Int>
bool
AbstractDistMatrix<T,Int>::Viewing() const
{ return !IsOwner( viewType_ ); }

template<typename T,typename Int>
bool
AbstractDistMatrix<T,Int>::Locked() const
{ return IsLocked( viewType_ ); }

template<typename T,typename Int>
Int
AbstractDistMatrix<T,Int>::Height() const
{ return height_; }

template<typename T,typename Int>
Int
AbstractDistMatrix<T,Int>::DiagonalLength( Int offset ) const
{ return elem::DiagonalLength(height_,width_,offset); }

template<typename T,typename Int>
Int
AbstractDistMatrix<T,Int>::Width() const
{ return width_; }

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::FreeAlignments() 
{ 
    constrainedColAlignment_ = false;
    constrainedRowAlignment_ = false;
}
    
template<typename T,typename Int>
bool
AbstractDistMatrix<T,Int>::ConstrainedColAlignment() const
{ return constrainedColAlignment_; }

template<typename T,typename Int>
bool
AbstractDistMatrix<T,Int>::ConstrainedRowAlignment() const
{ return constrainedRowAlignment_; }

template<typename T,typename Int>
Int
AbstractDistMatrix<T,Int>::ColAlignment() const
{ return colAlignment_; }

template<typename T,typename Int>
Int
AbstractDistMatrix<T,Int>::RowAlignment() const
{ return rowAlignment_; }

template<typename T,typename Int>
Int
AbstractDistMatrix<T,Int>::ColShift() const
{ return colShift_; }

template<typename T,typename Int>
Int
AbstractDistMatrix<T,Int>::RowShift() const
{ return rowShift_; }

template<typename T,typename Int>
const elem::Grid&
AbstractDistMatrix<T,Int>::Grid() const
{ return *grid_; }

template<typename T,typename Int>
size_t
AbstractDistMatrix<T,Int>::AllocatedMemory() const
{ return matrix_.MemorySize(); }

template<typename T,typename Int>
Int
AbstractDistMatrix<T,Int>::LocalHeight() const
{ return matrix_.Height(); }

template<typename T,typename Int>
Int
AbstractDistMatrix<T,Int>::LocalWidth() const
{ return matrix_.Width(); }

template<typename T,typename Int>
Int
AbstractDistMatrix<T,Int>::LDim() const
{ return matrix_.LDim(); }

template<typename T,typename Int>
T
AbstractDistMatrix<T,Int>::GetLocal( Int i, Int j ) const
{ return matrix_.Get(i,j); }

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::SetLocal( Int iLoc, Int jLoc, T alpha )
{ matrix_.Set(iLoc,jLoc,alpha); }

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::UpdateLocal( Int iLoc, Int jLoc, T alpha )
{ matrix_.Update(iLoc,jLoc,alpha); }

template<typename T,typename Int>
T*
AbstractDistMatrix<T,Int>::Buffer( Int iLoc, Int jLoc )
{ return matrix_.Buffer(iLoc,jLoc); }

template<typename T,typename Int>
const T*
AbstractDistMatrix<T,Int>::LockedBuffer( Int iLoc, Int jLoc ) const
{ return matrix_.LockedBuffer(iLoc,jLoc); }

template<typename T,typename Int>
elem::Matrix<T,Int>&
AbstractDistMatrix<T,Int>::Matrix()
{ return matrix_; }

template<typename T,typename Int>
const elem::Matrix<T,Int>&
AbstractDistMatrix<T,Int>::LockedMatrix() const
{ return matrix_; }

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::Empty()
{
    matrix_.Empty_();
    viewType_ = OWNER;
    height_ = 0;
    width_ = 0;
    colAlignment_ = 0;
    rowAlignment_ = 0;
    constrainedColAlignment_ = false;
    constrainedRowAlignment_ = false;
}

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::EmptyData()
{
    matrix_.Empty_();
    viewType_ = OWNER;
    height_ = 0;
    width_ = 0;
}

template<typename T,typename Int>
bool
AbstractDistMatrix<T,Int>::Participating() const
{ return grid_->InGrid(); }

//
// Complex-only specializations
//

template<typename T,typename Int>
BASE(T)
AbstractDistMatrix<T,Int>::GetLocalRealPart( Int iLoc, Int jLoc ) const
{ return matrix_.GetRealPart(iLoc,jLoc); }

template<typename T,typename Int>
BASE(T)
AbstractDistMatrix<T,Int>::GetLocalImagPart( Int iLoc, Int jLoc ) const
{ return matrix_.GetImagPart(iLoc,jLoc); }

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::SetLocalRealPart
( Int iLoc, Int jLoc, BASE(T) alpha )
{ matrix_.SetRealPart(iLoc,jLoc,alpha); }

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::SetLocalImagPart
( Int iLoc, Int jLoc, BASE(T) alpha )
{ matrix_.SetImagPart(iLoc,jLoc,alpha); }

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::UpdateLocalRealPart
( Int iLoc, Int jLoc, BASE(T) alpha )
{ matrix_.UpdateRealPart(iLoc,jLoc,alpha); }

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::UpdateLocalImagPart
( Int iLoc, Int jLoc, BASE(T) alpha )
{ matrix_.UpdateImagPart(iLoc,jLoc,alpha); }

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::SetShifts()
{
    if( Participating() )
    {
        colShift_ = Shift(ColRank(),colAlignment_,ColStride());
        rowShift_ = Shift(RowRank(),rowAlignment_,RowStride());
    }
    else
    {
        colShift_ = 0;
        rowShift_ = 0;
    }
}

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::SetColShift()
{
    if( Participating() )
        colShift_ = Shift(ColRank(),colAlignment_,ColStride());
    else
        colShift_ = 0;
}

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::SetRowShift()
{
    if( Participating() )
        rowShift_ = Shift(RowRank(),rowAlignment_,RowStride());
    else
        rowShift_ = 0;
}

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::SetGrid( const elem::Grid& grid )
{
    Empty();
    grid_ = &grid; 
    SetShifts();
}

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::ComplainIfReal() const
{ 
    if( !IsComplex<T>::val )
        throw std::logic_error("Called complex-only routine with real data");
}

template<typename T,typename Int>
BASE(T)
AbstractDistMatrix<T,Int>::GetRealPart( Int i, Int j ) const
{ return RealPart(Get(i,j)); }

template<typename T,typename Int>
BASE(T)
AbstractDistMatrix<T,Int>::GetImagPart( Int i, Int j ) const
{ return ImagPart(Get(i,j)); }

template<typename T,typename Int>
void
AbstractDistMatrix<T,Int>::MakeConsistent()
{
#ifndef RELEASE
    CallStackEntry cse("AbstractDistMatrix::MakeConsistent");
#endif
    const elem::Grid& g = this->Grid();
    const int root = g.VCToViewingMap(0);
    int message[7];
    if( g.ViewingRank() == root )
    {
        message[0] = viewType_;
        message[1] = height_;
        message[2] = width_;
        message[3] = constrainedColAlignment_;
        message[4] = constrainedRowAlignment_;
        message[5] = colAlignment_;
        message[6] = rowAlignment_;
    }
    mpi::Broadcast( message, 7, root, g.ViewingComm() );
    const ViewType newViewType = static_cast<ViewType>(message[0]);
    const int newHeight = message[1]; 
    const int newWidth = message[2];
    const bool newConstrainedCol = message[3];
    const bool newConstrainedRow = message[4];
    const int newColAlignment = message[5];
    const int newRowAlignment = message[6];
    if( !this->Participating() )
    {
        viewType_ = newViewType;
        height_ = newHeight;
        width_ = newWidth;
        constrainedColAlignment_ = newConstrainedCol;
        constrainedRowAlignment_ = newConstrainedRow;
        colAlignment_ = newColAlignment;
        rowAlignment_ = newRowAlignment;
        colShift_ = 0;
        rowShift_ = 0;
    }
#ifndef RELEASE
    else
    {
        if( viewType_ != newViewType )
            throw std::logic_error("Inconsistent ViewType");
        if( height_ != newHeight )
            throw std::logic_error("Inconsistent height");
        if( width_ != newWidth )
            throw std::logic_error("Inconsistent width");
        if( constrainedColAlignment_ != newConstrainedCol || 
            colAlignment_ != newColAlignment )
            throw std::logic_error("Inconsistent column constraint");
        if( constrainedRowAlignment_ != newConstrainedRow ||
            rowAlignment_ != newRowAlignment )
            throw std::logic_error("Inconsistent row constraint");
    }
#endif
}

#define PROTO(T) \
  template class AbstractDistMatrix<T,int>

PROTO(int);
#ifndef DISABLE_FLOAT
PROTO(float);
#endif // ifndef DISABLE_FLOAT
PROTO(double);
#ifndef DISABLE_COMPLEX
#ifndef DISABLE_FLOAT
PROTO(Complex<float>);
#endif // ifndef DISABLE_FLOAT
PROTO(Complex<double>);
#endif // ifndef DISABLE_COMPLEX

#ifndef RELEASE

#define CONFORMING(T) \
  template void AssertConforming1x2( const AbstractDistMatrix<T,int>& AL, const AbstractDistMatrix<T,int>& AR ); \
  template void AssertConforming2x1( const AbstractDistMatrix<T,int>& AT, const AbstractDistMatrix<T,int>& AB ); \
  template void AssertConforming2x2( const AbstractDistMatrix<T,int>& ATL, const AbstractDistMatrix<T,int>& ATR, const AbstractDistMatrix<T,int>& ABL, const AbstractDistMatrix<T,int>& ABR )

CONFORMING(int);
#ifndef DISABLE_FLOAT
CONFORMING(float);
#endif // ifndef DISABLE_FLOAT
CONFORMING(double);
#ifndef DISABLE_COMPLEX
#ifndef DISABLE_FLOAT
CONFORMING(Complex<float>);
#endif // ifndef DISABLE_FLOAT
CONFORMING(Complex<double>);
#endif // ifndef DISABLE_COMPLEX
#endif // ifndef RELEASE

} // namespace elem
