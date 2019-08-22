/***************************************************************************
  qgschainedfilterfeaturepicker.cpp - QgsChainedFilterFeaturePicker

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

#include <QHBoxLayout>

#include "qgschainedfilterfeaturepicker.h"
#include "qgsvectorlayer.h"
#include "qgsapplication.h"

static int NOTSET_FILTER = std::numeric_limits<int>::min();

QgsChainedFilterFeaturePicker::QgsChainedFilterFeaturePicker( QWidget *parent )
  : QWidget( parent )
{
  mLayout = new QHBoxLayout();
  setLayout( mLayout );
}

QgsVectorLayer *QgsChainedFilterFeaturePicker::sourceLayer() const
{
  return mLayer;
}

void QgsChainedFilterFeaturePicker::setSourceLayer( QgsVectorLayer *sourceLayer )
{
  if ( sourceLayer == mLayer )
    return;

  mLayer = sourceLayer();

  emit sourceLayerChanged();
}

QStringList QgsChainedFilterFeaturePicker::filterFields() const
{
  // TODO
}

void QgsChainedFilterFeaturePicker::setFilterFields( const QStringList &filterFields )
{
  qDeleteAll( mComboBoxes );
  mComboBoxes.clear();
  mFilterFields.clear():

    if ( !mLayer || !mLayer->isValid() )
      return;

  for ( const QString &fieldName : filterFields )
  {
    int idx = mLayer->fields().lookupField( fieldName );

    if ( idx == -1 )
      continue;

    QComboBox *cb = new QComboBox();
    cb->setProperty( "Field", fieldName );
    cb->setProperty( "FieldAlias", mLayer->attributeDisplayName( idx ) );

    QVariantList uniqueValues = mLayer->uniqueValues( idx ).toList();
    cb->addItem( QString( "[%1]" ).arg( mLayer->attributeDisplayName( idx ) ), NOTSET_FILTER );
    QVariant nullValue = QgsApplication::nullRepresentation(); // Does null values are listed with unique?
    cb->addItem( nullValue.toString(), QVariant( mLayer->fields().at( idx ).type() ) );

    std::sort( uniqueValues.begin(), uniqueValues.end(), qgsVariantLessThan );
    const auto constUniqueValues = uniqueValues;
    for ( const QVariant &v : constUniqueValues )
    {
      cb->addItem( v.toString(), v );
    }

    connect( cb, &QComboBox::currentTextChanged, this, &QgsFilterFeaturePicker::filtersChanged );

    mComboBoxes << cb;
    mFilterFields << fieldName;
  }

  emit filterFieldsChanged();
}

