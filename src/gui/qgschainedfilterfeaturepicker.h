/***************************************************************************
  qgschainedfilterfeaturepicker.h - QgsChainedFilterFeaturePicker

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


#ifndef QGSCHAINEDFILTERFEATUREPICKER_H
#define QGSCHAINEDFILTERFEATUREPICKER_H

#include <QWidget>
#include <QComboBox>

#include "qgis_gui.h"

class QHBoxLayout;

class QgsVectorLayer;


/**
 * \ingroup gui
 * This offers a series of comboboxes on field to choose a feature using chained fitler fields.
 *
 *
 * \see QgsFilterFeaturePicker for non-chained filters feature picker
 * \since QGIS 3.10
 */
class GUI_EXPORT QgsChainedFilterFeaturePicker : public QWidget
{
    Q_OBJECT

    Q_PROPERTY( QgsVectorLayer *sourceLayer READ sourceLayer WRITE setSourceLayer NOTIFY sourceLayerChanged )
    Q_PROPERTY( QStringList filterFields READ filterFields WRITE setFilterFields NOTIFY filterFieldsChanged )
    Q_PROPERTY( bool allowNull READ allowNull WRITE setAllowNull NOTIFY allowNullChanged )

  public:
    explicit QgsChainedFilterFeaturePicker( QWidget *parent = nullptr );

    /**
     * The layer from which features should be listed.
     */
    QgsVectorLayer *sourceLayer() const;

    /**
     * The layer from which features should be listed.
     */
    void setSourceLayer( QgsVectorLayer *sourceLayer );

    /**
     * Returns the fields names used as filters
     */
    QStringList filterFields() const;

    /**
     * Returns the fields names used as filters
     */
    void setFilterFields( const QStringList &filterFields );

    /**
     * Determines if a NULL value should be available in the list.
     */
    bool allowNull() const;

    /**
     * Determines if a NULL value should be available in the list.
     */
    void setAllowNull( bool allowNull );

  signals:

    /**
      * Filters fields have changed
      */
    void filterFieldsChanged();

    /**
     * The layer from which features should be listed.
     */
    void sourceLayerChanged();

    /**
     * Determines if a NULL value should be available in the list.
     */
    void allowNullChanged();


  private:
    QgsVectorLayer *mLayer = nullptr;
    QHBoxLayout *mLayout;
    QList<QString *> mFilterFields;
    QList<QComboBox *> mComboBoxes;
};

#endif // QGSCHAINEDFILTERFEATUREPICKER_H
