/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/gui/editorwidgets/qgsdatetimeedit.h                              *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/





class QgsDateTimeEdit : QDateTimeEdit
{
%Docstring
 The QgsDateTimeEdit class is a QDateTimeEdit with the capability of setting/reading null date/times.
%End

%TypeHeaderCode
#include "qgsdatetimeedit.h"
%End
  public:
    explicit QgsDateTimeEdit( QWidget *parent /TransferThis/ = 0 );
%Docstring
Constructor for QgsDateTimeEdit
%End

    QDateTime dateTime() const;
%Docstring
 dateTime returns the date time which can eventually be a null date/time

.. note::

   since QDateTimeEdit.dateTime() is not virtual, dateTime must be called for QgsDateTimeEdit.
%End
    QDate date() const;
    QTime time() const;

    bool allowNull() const;
%Docstring
Determines if the widget allows setting null date/time.
%End
    void setAllowNull( bool enable );

    virtual QSize sizeHint() const;

    virtual QSize minimumSizeHint() const;


  protected:
    virtual void showEvent( QShowEvent *event );

    virtual void resizeEvent( QResizeEvent *event );

    virtual void paintEvent( QPaintEvent *event );

    virtual void keyPressEvent( QKeyEvent *event );

    virtual void mousePressEvent( QMouseEvent *event );

    virtual bool focusNextPrevChild( bool next );

    virtual QValidator::State validate( QString &input, int &pos ) const;


  public slots:

    void setDateTime( const QDateTime &dateTime );
%Docstring
 setDateTime set the date time in the widget and handles null date times.

.. note::

   since QDateTimeEdit.setDateTime() is not virtual, setDateTime must be called for QgsDateTimeEdit.
%End
    void setDate( const QDate &date );
    void setTime( const QTime &time );

    virtual void clear();

%Docstring
Set the current date as NULL

.. note::

   if the widget is not configured to accept NULL dates, this will have no effect
%End

    void setEmpty();
%Docstring
Resets the widget to show no value (ie, an "unknown" state).

.. versionadded:: 2.16
%End

};

/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/gui/editorwidgets/qgsdatetimeedit.h                              *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
