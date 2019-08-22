/***************************************************************************
  qgsfilterfeaturepicker.cpp - QgsFilterFeaturePicker

 ---------------------
 begin                : 20.08.2019
 copyright            : (C) 2019 by Denis Rouzaud
 email                : denis@opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QMap>
#include <QHBoxLayout>

#include "qgsapplication.h"
#include "qgsfilterfeaturepicker.h"
#include "qgsfeaturelistcombobox.h"
#include "qgsvectorlayer.h"
#include "qgslogger.h"


static int IGNORED_FILTER = std::numeric_limits<int>::min();

QgsFilterFeaturePicker::QgsFilterFeaturePicker( QWidget *parent )
  : QWidget( parent )
{
  QHBoxLayout *layout = new QHBoxLayout();

  mFiltersLayout = new QHBoxLayout();
  layout->addLayout( mFiltersLayout );

  mFeatureListComboBox = new QgsFeatureListComboBox( this );
  layout->addWidget( mFeatureListComboBox );

  setLayout( layout );

  // map signals
  connect( mFeatureListComboBox, &QgsFeatureListComboBox::modelUpdated, this, &QgsFilterFeaturePicker::modelUpdated );
  connect( mFeatureListComboBox, &QgsFeatureListComboBox::sourceLayerChanged, this, &QgsFilterFeaturePicker::sourceLayerChanged );
  connect( mFeatureListComboBox, &QgsFeatureListComboBox::displayExpressionChanged, this, &QgsFilterFeaturePicker::displayExpressionChanged );
  connect( mFeatureListComboBox, &QgsFeatureListComboBox::filterExpressionChanged, this, &QgsFilterFeaturePicker::filterExpressionChanged );
  connect( mFeatureListComboBox, &QgsFeatureListComboBox::identifierValueChanged, this, &QgsFilterFeaturePicker::identifierValueChanged );
  connect( mFeatureListComboBox, &QgsFeatureListComboBox::identifierFieldChanged, this, &QgsFilterFeaturePicker::identifierFieldChanged );
  connect( mFeatureListComboBox, &QgsFeatureListComboBox::allowNullChanged, this, &QgsFilterFeaturePicker::allowNullChanged );
}

QgsVectorLayer *QgsFilterFeaturePicker::sourceLayer() const
{
  return mFeatureListComboBox->sourceLayer();
}

void QgsFilterFeaturePicker::setSourceLayer( QgsVectorLayer *sourceLayer )
{
  mFeatureListComboBox->setSourceLayer( sourceLayer );
}

QStringList QgsFilterFeaturePicker::filterFields() const
{
  return mFilterFieldsComboBoxes.keys();
}

void QgsFilterFeaturePicker::setFilterFields( const QStringList &filterFields )
{
  qDeleteAll( mFilterFieldsComboBoxes );
  mFilterFieldsComboBoxes.clear();

  QgsVectorLayer *layer = sourceLayer();
  if ( !layer )
    return;

  for ( const QString &fieldName : filterFields )
  {
    int idx = layer->fields().lookupField( fieldName );

    if ( idx == -1 )
      continue;

    QComboBox *cb = new QComboBox();
    cb->setProperty( "Field", fieldName );
    cb->setProperty( "FieldAlias", layer->attributeDisplayName( idx ) );
    mFilterFieldsComboBoxes.insert( fieldName, cb );
    QVariantList uniqueValues = layer->uniqueValues( idx ).toList();
    cb->addItem( QString( "[%1]" ).arg( layer->attributeDisplayName( idx ) ), IGNORED_FILTER );
    QVariant nullValue = QgsApplication::nullRepresentation();
    cb->addItem( nullValue.toString(), QVariant( layer->fields().at( idx ).type() ) );

    std::sort( uniqueValues.begin(), uniqueValues.end(), qgsVariantLessThan );
    const auto constUniqueValues = uniqueValues;
    for ( const QVariant &v : constUniqueValues )
    {
      cb->addItem( v.toString(), v );
    }

    connect( cb, &QComboBox::currentTextChanged, this, &QgsFilterFeaturePicker::filtersChanged );
  }

  emit filterFieldsChanged();
}

QString QgsFilterFeaturePicker::displayExpression() const
{
  return mFeatureListComboBox->displayExpression();
}

void QgsFilterFeaturePicker::setDisplayExpression( const QString &displayExpression )
{
  mFeatureListComboBox->setDisplayExpression( displayExpression );
}

QString QgsFilterFeaturePicker::filterExpression() const
{
  return mFeatureListComboBox->filterExpression();
}

int QgsFilterFeaturePicker::nullIndex() const
{
  return mFeatureListComboBox->nullIndex();
}

void QgsFilterFeaturePicker::setFilterExpression( const QString &filterExpression )
{
  mFeatureListComboBox->setFilterExpression( filterExpression );
}

QVariant QgsFilterFeaturePicker::identifierValue() const
{
  return mFeatureListComboBox->identifierValue();
}

void QgsFilterFeaturePicker::setIdentifierValue( const QVariant &identifierValue )
{
  mFeatureListComboBox->setIdentifierValues( identifierValue );
}

QgsFeatureRequest QgsFilterFeaturePicker::currentFeatureRequest() const
{
  return mFeatureListComboBox->currentFeatureRequest();
}

bool QgsFilterFeaturePicker::allowNull() const
{
  return mFeatureListComboBox->allowNull();
}

void QgsFilterFeaturePicker::setAllowNull( bool allowNull )
{
  mFeatureListComboBox->setAllowNull( allowNull );
}

QString QgsFilterFeaturePicker::identifierField() const
{
  return mFeatureListComboBox->identifierField();
}

void QgsFilterFeaturePicker::setIdentifierField( const QString &identifierField )
{
  mFeatureListComboBox->setIdentifierField( identifierField );
}

void QgsFilterFeaturePicker::filtersChanged()
{
  QStringList conditions;
  QMap<QString, QComboBox *>::const_iterator it = mFilterFieldsComboBoxes.constBegin();
  for ( ; it != mFilterFieldsComboBoxes.constEnd(); ++it )
  {
    if ( it.value()->currentData().toInt() == IGNORED_FILTER )
      continue;
    conditions << QgsExpression::createFieldEqualityExpression( it.key(), it.value()->currentData() );
  }
  QgsDebugMsg( QStringLiteral( "Filter conditions: '%1'" ).arg( conditions.join( " AND " ) ) );
  mFeatureListComboBox->setFilterExpression( conditions.join( QStringLiteral( " AND " ) ) );
}
