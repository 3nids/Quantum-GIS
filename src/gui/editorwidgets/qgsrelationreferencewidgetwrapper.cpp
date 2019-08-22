/***************************************************************************
    qgsrelationreferencewidgetwrapper.cpp
     --------------------------------------
    Date                 : 20.4.2013
    Copyright            : (C) 2013 Matthias Kuhn
    Email                : matthias at opengis dot ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "qgsrelationreferencewidgetwrapper.h"
#include "qgsproject.h"
#include "qgsrelationmanager.h"
#include "qgsrelationreferencewidget.h"

QgsRelationReferenceWidgetWrapper::QgsRelationReferenceWidgetWrapper( QgsVectorLayer *vl, int fieldIdx, QWidget *editor, QgsMapCanvas *canvas, QgsMessageBar *messageBar, QWidget *parent )
  : QgsEditorWidgetWrapper( vl, fieldIdx, editor, parent )
  , mCanvas( canvas )
  , mMessageBar( messageBar )
  , mIndeterminateState( false )
{
}

QWidget *QgsRelationReferenceWidgetWrapper::createWidget( QWidget *parent )
{
  QgsRelationReferenceWidget *w = new QgsRelationReferenceWidget( parent );
  return w;
}

void QgsRelationReferenceWidgetWrapper::initWidget( QWidget *editor )
{
  QgsRelationReferenceWidget *w = qobject_cast<QgsRelationReferenceWidget *>( editor );
  if ( !w )
  {
    w = new QgsRelationReferenceWidget( editor );
  }

  mWidget = w;

  const QgsAttributeEditorContext *ctx = &context();

  mWidget->setEditorContext( *ctx, mCanvas, mMessageBar );

  const QVariant relationName = config( QStringLiteral( "Relation" ) );
  QgsRelation relation; // invalid relation by default
  if ( relationName.isValid() )
    relation = QgsProject::instance()->relationManager()->relation( relationName.toString() );
  else if ( ! layer()->referencingRelations( fieldIdx() ).isEmpty() )
    relation = layer()->referencingRelations( fieldIdx() )[0];

  bool allowNull = config( QStringLiteral( "AllowNULL" ) ).toBool();
  bool showForm = config( QStringLiteral( "ShowForm" ), false ).toBool();
  bool mapIdent = config( QStringLiteral( "MapIdentification" ), false ).toBool();
  bool readOnlyWidget = config( QStringLiteral( "ReadOnly" ), false ).toBool();
  bool orderByValue = config( QStringLiteral( "OrderByValue" ), false ).toBool();
  bool showOpenFormButton = config( QStringLiteral( "ShowOpenFormButton" ), true ).toBool();
  QStringList filterFields = config( QStringLiteral( "FilterFields" ) ).toStringList();
  bool chainedFilters = config( QStringLiteral( "ChainFilters" ) ).toBool();
  QString displayExpression = config( QStringLiteral( "DisplayExpression" ) ).toString();


  // for composite relation additional fields are set
  // and display expression, if not defined, will be set to concatenate all referenced fields
  QgsAttributeList additionalFieldIdxs;
  QStringList fieldsForDisplayExpression;
  if ( relation.isValid() && relation.isComposite() )
  {
    chainedFilters = true;
    for ( int idx = 0; idx < relation.fieldPairs().count(); idx++ )
    {
      QString fieldName = relation.fieldPairs().at( idx ).referencingField();
      fieldsForDisplayExpression << QString( " \"%1\" " ).arg( fieldName );
      if ( fieldName  == field().name() )
        continue;

      additionalFieldIdxs << idx;
    }
  }

  if ( displayExpression.isEmpty() )
  {
    if ( !fieldsForDisplayExpression.isEmpty() )
      displayExpression = fieldsForDisplayExpression.join( " || ' ' || " );
    else
      displayExpression = relation.referencedLayer()->displayExpression();
  }

  setAdditionalFields( additionalFieldIdxs );

  mWidget->setEmbedForm( showForm );
  mWidget->setReadOnlySelector( readOnlyWidget );
  mWidget->setAllowMapIdentification( mapIdent );
  mWidget->setOrderByValue( orderByValue );
  mWidget->setOpenFormButtonVisible( showOpenFormButton );
  mWidget->setDisplayExpression( displayExpression );

  if ( !filterFields.isEmpty() )
  {
    mWidget->setFilterFields( filterFields );
    mWidget->setChainFilters( chainedFilters );
  }
  mWidget->setAllowAddFeatures( config( QStringLiteral( "AllowAddFeatures" ), false ).toBool() );

  // If this widget is already embedded by the same relation, reduce functionality
  do
  {
    if ( ctx->relation().id() == relation.id() )
    {
      mWidget->setEmbedForm( false );
      mWidget->setReadOnlySelector( true );
      mWidget->setAllowMapIdentification( false );
      mWidget->setOpenFormButtonVisible( false );
      break;
    }
    ctx = ctx->parentContext();
  }
  while ( ctx );

  mWidget->setRelation( relation, allowNull );

  connect( mWidget, &QgsRelationReferenceWidget::foreignKeysChanged, this, &QgsRelationReferenceWidgetWrapper::foreignKeysChanged );
}

QVariant QgsRelationReferenceWidgetWrapper::value() const
{
  if ( !mWidget )
    return QVariant( field().type() );

  QVariant v = mWidget->foreignKey();

  if ( v.isNull() )
  {
    return QVariant( field().type() );
  }
  else
  {
    return v;
  }
}

bool QgsRelationReferenceWidgetWrapper::valid() const
{
  return mWidget;
}

void QgsRelationReferenceWidgetWrapper::showIndeterminateState()
{
  if ( mWidget )
  {
    mWidget->showIndeterminateState();
  }
  mIndeterminateState = true;
}

QVariantMap QgsRelationReferenceWidgetWrapper::additionalFieldValues() const
{
  // TODO
}

void QgsRelationReferenceWidgetWrapper::updateValues( const QVariant &val, const QVariantMap &additionalValues )
{
  if ( !mWidget || ( !mIndeterminateState && val == value() && val.isNull() == value().isNull() ) )
    return;

  mIndeterminateState = false;

  // append the main value to additional values to set foreign keys on relation reference widget
  QVariantMap foreignKeyValues = additionalValues;
  foreignKeyValues.insert( field().name(), val );
  mWidget->setForeignKeys( foreignKeyValues );
}

void QgsRelationReferenceWidgetWrapper::setEnabled( bool enabled )
{
  if ( !mWidget )
    return;

  mWidget->setRelationEditable( enabled );
}

void QgsRelationReferenceWidgetWrapper::foreignKeysChanged( const QVariantList &foreignKeys )
{
  if ( !foreignKeys.isEmpty() )
  {
    emit valuesChanged( QVariant( field().type() ) );
  }
  else
  {
    // TODO
    //emit valuesChanged( foreignKeys, additionalValues );
  }
}

void QgsRelationReferenceWidgetWrapper::updateConstraintWidgetStatus()
{
  if ( mWidget )
  {
    if ( !constraintResultVisible() )
    {
      widget()->setStyleSheet( QString() );
    }
    else
    {
      switch ( constraintResult() )
      {
        case ConstraintResultPass:
          mWidget->setStyleSheet( QString() );
          break;

        case ConstraintResultFailHard:
          mWidget->setStyleSheet( QStringLiteral( ".QComboBox { background-color: #dd7777; }" ) );
          break;

        case ConstraintResultFailSoft:
          mWidget->setStyleSheet( QStringLiteral( ".QComboBox { background-color: #ffd85d; }" ) );
          break;
      }
    }
  }
}
