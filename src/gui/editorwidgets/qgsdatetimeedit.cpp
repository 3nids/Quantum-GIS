/***************************************************************************
    qgsdatetimeedit.cpp
     --------------------------------------
    Date                 : 08.2014
    Copyright            : (C) 2014 Denis Rouzaud
    Email                : denis.rouzaud@gmail.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QCalendarWidget>
#include <QLineEdit>
#include <QMouseEvent>
#include <QSettings>
#include <QStyle>
#include <QPushButton>
#include <QStyleOptionSpinBox>


#include "qgsdatetimeedit.h"

#include "qgsapplication.h"
#include "qgslogger.h"

QgsDateTimeEdit::QgsDateTimeEdit( QWidget *parent )
  : QDateTimeEdit( parent )
{
  setAllowNull( mAllowNull );
}

QDateTime QgsDateTimeEdit::dateTime() const
{
  if ( mAllowNull && mIsNull )
  {
    return QDateTime();
  }
  else
  {
    return QDateTimeEdit::dateTime();
  }
}

QDate QgsDateTimeEdit::date() const
{
  if ( mAllowNull && mIsNull )
  {
    return QDate();
  }
  else
  {
    return QDateTimeEdit::date();
  }
}

QTime QgsDateTimeEdit::time() const
{
  if ( mAllowNull && mIsNull )
  {
    return QTime();
  }
  else
  {
    return QDateTimeEdit::time();
  }
}

void QgsDateTimeEdit::setDateTime( const QDateTime &dateTime )
{
  if ( mAllowNull && !dateTime.isValid() )
  {
    setNull( true );
  }
  else
  {
    setNull( false );
    QDateTimeEdit::setDateTime( dateTime );
  }
}

void QgsDateTimeEdit::setDate( const QDate &date )
{
  if ( mAllowNull && !date.isValid() )
  {
    setNull( true );
  }
  else
  {
    setNull( false );
    QDateTimeEdit::setDate( date );
  }
}

void QgsDateTimeEdit::setTime( const QTime &time )
{
  if ( mAllowNull && !time.isValid() )
  {
    setNull( true );
  }
  else
  {
    setNull( false );
    QDateTimeEdit::setTime( time );
  }
}

bool QgsDateTimeEdit::allowNull() const
{
  return mAllowNull;
}

void QgsDateTimeEdit::setAllowNull( bool enable )
{
  mAllowNull = enable;

  if ( enable && !mClearButton )
  {
    mClearButton = new QPushButton( this );
    mClearButton->setFlat( true );
    mClearButton->setIcon( QIcon( ":/images/edit-clear-locationbar-rtl.png" ) );
    mClearButton->setFocusPolicy( Qt::NoFocus );
    mClearButton->setFixedSize( 17, mClearButton->sizeHint().height() - 6 );
    connect( mClearButton, SIGNAL( clicked() ), this, SLOT( clear() ) );
    mClearButton->setVisible( !mIsNull );
  }
  else if ( mClearButton )
  {
    disconnect( mClearButton, SIGNAL( clicked() ), this, SLOT( clear() ) );
    delete mClearButton;
    mClearButton = 0;
  }

  update();
}

QSize QgsDateTimeEdit::sizeHint() const
{
  const QSize sz = QDateTimeEdit::sizeHint();
  if ( !mClearButton )
    return sz;
  return QSize( sz.width() + mClearButton->width() + 3, sz.height() );
}

QSize QgsDateTimeEdit::minimumSizeHint() const
{
  const QSize sz = QDateTimeEdit::minimumSizeHint();
  if ( !mClearButton )
    return sz;
  return QSize( sz.width() + mClearButton->width() + 3, sz.height() );
}

void QgsDateTimeEdit::showEvent( QShowEvent *event )
{
  QDateTimeEdit::showEvent( event );
  setNull( mIsNull ); // force empty string back in
}

void QgsDateTimeEdit::resizeEvent( QResizeEvent *event )
{
  if ( mClearButton )
  {
    QStyleOptionSpinBox opt;
    initStyleOption( &opt );
    opt.subControls = QStyle::SC_SpinBoxUp;

    int left = style()->subControlRect( QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxUp, this ).left() - mClearButton->width() - 3;
    mClearButton->move( left, ( height() - mClearButton->height() ) / 2 );
  }

  QDateTimeEdit::resizeEvent( event );
}

void QgsDateTimeEdit::paintEvent( QPaintEvent *event )
{
  QDateTimeEdit::paintEvent( event );
}

void QgsDateTimeEdit::keyPressEvent( QKeyEvent *event )
{
  if ( mAllowNull &&
       ( event->key() >= Qt::Key_0 ) &&
       ( event->key() <= Qt::Key_9 ) &&
       mIsNull )
  {
    setDateTime( QDateTime::currentDateTime() );
  }
  if ( event->key() == Qt::Key_Tab && mAllowNull && mIsNull )
  {
    QAbstractSpinBox::keyPressEvent( event );
    return;
  }
  if ( event->key() == Qt::Key_Backspace && mAllowNull )
  {
    QLineEdit *edit = qFindChild<QLineEdit *>( this, "qt_spinbox_lineedit" );
    if ( edit->selectedText() == edit->text() )
    {
      setDateTime( QDateTime() );
      event->accept();
      return;
    }
  }

  QDateTimeEdit::keyPressEvent( event );
}

void QgsDateTimeEdit::mousePressEvent( QMouseEvent *event )
{
  bool saveNull = mIsNull;
  QDateTimeEdit::mousePressEvent( event );
  if ( mAllowNull && saveNull && calendarWidget()->isVisible() )
  {
    setDateTime( QDateTime::currentDateTime() );
  }
}

bool QgsDateTimeEdit::focusNextPrevChild( bool next )
{
  if ( mAllowNull && mIsNull )
  {
    return QAbstractSpinBox::focusNextPrevChild( next );
  }
  else
  {
    return QDateTimeEdit::focusNextPrevChild( next );
  }
}

QValidator::State QgsDateTimeEdit::validate( QString &input, int &pos ) const
{
  if ( mAllowNull && mIsNull )
  {
    return QValidator::Acceptable;
  }
  return QDateTimeEdit::validate( input, pos );
}

void QgsDateTimeEdit::clear()
{
  setNull( true );
}

void QgsDateTimeEdit::setEmpty()
{
//  mNullLabel->setVisible( false );
  lineEdit()->setVisible( false );
  mClearButton->setVisible( mAllowNull );
  mIsEmpty = true;
}


void QgsDateTimeEdit::setNull( bool n )
{
  mIsNull = n;
  if ( mIsNull )
  {
    QLineEdit *edit = qFindChild<QLineEdit *>( this, "qt_spinbox_lineedit" );
    if ( !edit->text().isEmpty() )
    {
      edit->clear();
    }
  }
  if ( mAllowNull )
  {
    mClearButton->setVisible( !mIsNull );
  }
}



/*
QgsDateTimeEdit::QgsDateTimeEdit( QWidget *parent )
  : QDateTimeEdit( parent )
{
  mClearButton = new QToolButton( this );
  mClearButton->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mIconClearText.svg" ) ) );
  mClearButton->setCursor( Qt::ArrowCursor );
  mClearButton->setStyleSheet( QStringLiteral( "position: absolute; border: none; padding: 0px;" ) );
  mClearButton->hide();
  connect( mClearButton, &QAbstractButton::clicked, this, &QgsDateTimeEdit::clear );

//  mNullLabel = new QLineEdit( QgsApplication::nullRepresentation(), this );
//  mNullLabel->setReadOnly( true );
//  mNullLabel->setStyleSheet( QStringLiteral( "position: absolute; border: none; font-style: italic; color: grey;" ) );
//  mNullLabel->hide();

  setStyleSheet( QStringLiteral( ".QWidget, QLineEdit, QToolButton { padding-right: %1px; }" ).arg( mClearButton->sizeHint().width() + spinButtonWidth() + frameWidth() + 1 ) );

  QSize msz = minimumSizeHint();
  setMinimumSize( std::max( msz.width(), mClearButton->sizeHint().height() + frameWidth() * 2 + 2 ),
                  std::max( msz.height(), mClearButton->sizeHint().height() + frameWidth() * 2 + 2 ) );

  connect( this, &QDateTimeEdit::dateTimeChanged, this, &QgsDateTimeEdit::changed );

  // set this by defaut to properly connect the calendar widget
  setCalendarPopup( true );
  // when clearing the widget, date of the QDateTimeEdit will be set to minimum date
  // hence when the calendar popups, on selection changed if it set to the minimum date,
  // the page of the current date will be shown
  connect( calendarWidget(), &QCalendarWidget::selectionChanged, this, &QgsDateTimeEdit::calendarSelectionChanged );

  // init with current time so mIsNull is properly initialized
  QDateTimeEdit::setDateTime( QDateTime::currentDateTime() );

  setMinimumEditDateTime();
}

void QgsDateTimeEdit::setAllowNull( bool allowNull )
{
  mAllowNull = allowNull;

//  mNullLabel->setVisible( ( mAllowNull && mIsNull ) && !mIsEmpty );
  mClearButton->setVisible( mAllowNull && ( !mIsNull || mIsEmpty ) );
  lineEdit()->setVisible( ( !mAllowNull || !mIsNull ) && !mIsEmpty );
}


void QgsDateTimeEdit::clear()
{
    QDateTimeEdit::blockSignals( true );
    QDateTimeEdit::setDateTime( minimumDateTime() );
    QDateTimeEdit::blockSignals( false );
  changed( QDateTime() );
  emit dateTimeChanged( QDateTime() );
}

void QgsDateTimeEdit::setEmpty()
{
//  mNullLabel->setVisible( false );
  lineEdit()->setVisible( false );
  mClearButton->setVisible( mAllowNull );
  mIsEmpty = true;
}

void QgsDateTimeEdit::mousePressEvent( QMouseEvent *event )
{
  QRect lerect = rect().adjusted( 0, 0, -spinButtonWidth(), 0 );
  if ( mAllowNull && mIsNull && lerect.contains( event->pos() ) )
    return;


  QDateTimeEdit::mousePressEvent( event );
}

void QgsDateTimeEdit::changed( const QDateTime &dateTime )
{
  mIsEmpty = false;
  mIsNull = dateTime.isNull();
  setSpecialValueText(mIsNull ? QgsApplication::nullRepresentation() : QString() );
//  mNullLabel->setVisible( mAllowNull && mIsNull );
  mClearButton->setVisible( mAllowNull && !mIsNull );
  lineEdit()->setVisible( !mAllowNull || !mIsNull );
}

void QgsDateTimeEdit::calendarSelectionChanged()
{
  if ( mAllowNull && calendarWidget() && calendarWidget()->selectedDate() == minimumDate() )
  {
    calendarWidget()->setCurrentPage( QDate::currentDate().year(), QDate::currentDate().month() );
  }
}

int QgsDateTimeEdit::spinButtonWidth() const
{
  return calendarPopup() ? 25 : 18;
}

int QgsDateTimeEdit::frameWidth() const
{
  return style()->pixelMetric( QStyle::PM_DefaultFrameWidth );
}

void QgsDateTimeEdit::setDateTime( const QDateTime &dateTime )
{
  mIsEmpty = false;

  // set an undefined date
  if ( !dateTime.isValid() || dateTime.isNull() )
  {
    clear();
  }
  else
  {
    QDateTimeEdit::setDateTime( dateTime );
    mIsNull = false;
    changed( dateTime );
  }
}

QDateTime QgsDateTimeEdit::dateTime() const
{
  if ( mAllowNull && mIsNull )
  {
    return QDateTime();
  }
  else
  {
    return QDateTimeEdit::dateTime();
  }
}

void QgsDateTimeEdit::resizeEvent( QResizeEvent *event )
{
  QDateTimeEdit::resizeEvent( event );

  QSize sz = mClearButton->sizeHint();


  mClearButton->move( rect().right() - frameWidth() - spinButtonWidth() - sz.width(),
                      ( rect().bottom() + 1 - sz.height() ) / 2 );

//  mNullLabel->move( 0, 0 );
//  mNullLabel->setMinimumSize( rect().adjusted( 0, 0, -spinButtonWidth(), 0 ).size() );
//  mNullLabel->setMaximumSize( rect().adjusted( 0, 0, -spinButtonWidth(), 0 ).size() );
}
*/

