/***************************************************************************
  qgsfilterfeaturepicker.h - QgsFilterFeaturePicker

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

#ifndef QGSFILTERFEATUREPICKER_H
#define QGSFILTERFEATUREPICKER_H

#include <QWidget>
#include <QComboBox>

#include "qgis_gui.h"


class QHBoxLayout;

class QgsFeatureListComboBox;
class QgsVectorLayer;
class QgsFeatureRequest;


/**
 * \ingroup gui
 * This offers a combobox with autocompleter that allows selecting features from a layer with a series of comobobox to filter features.
 *
 * \see QgsFeatureListComboBox, QgsChainedFilterFeaturePicker
 * \since QGIS 3.10
 */
class GUI_EXPORT QgsFilterFeaturePicker : public QWidget
{
    Q_OBJECT

  public:
    explicit QgsFilterFeaturePicker( QWidget *parent = nullptr );

    Q_PROPERTY( QgsVectorLayer *sourceLayer READ sourceLayer WRITE setSourceLayer NOTIFY sourceLayerChanged )
    Q_PROPERTY( QStringList filterFields READ filterFields WRITE setFilterFields NOTIFY filterFieldsChanged )
    Q_PROPERTY( QString displayExpression READ displayExpression WRITE setDisplayExpression NOTIFY displayExpressionChanged )
    Q_PROPERTY( QString filterExpression READ filterExpression WRITE setFilterExpression NOTIFY filterExpressionChanged )
    Q_PROPERTY( QVariant identifierValue READ identifierValue WRITE setIdentifierValue NOTIFY identifierValueChanged )
    Q_PROPERTY( QString identifierField READ identifierField WRITE setIdentifierField NOTIFY identifierFieldChanged )
    Q_PROPERTY( bool allowNull READ allowNull WRITE setAllowNull NOTIFY allowNullChanged )

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
     * The display expression will be used to display features as well as
     * the value to match the typed text against.
     */
    QString displayExpression() const;

    /**
     * The display expression will be used to display features as well as
     * the value to match the typed text against.
     */
    void setDisplayExpression( const QString &displayExpression );

    /**
     * An additional expression to further restrict the available features.
     * This can be used to integrate additional spatial or other constraints.
     */
    QString filterExpression() const;

    /**
     * Returns the current index of the NULL value, or -1 if NULL values are
     * not allowed.
     */
    int nullIndex() const;

    /**
     * An additional expression to further restrict the available features.
     * This can be used to integrate additional spatial or other constraints.
     */
    void setFilterExpression( const QString &filterExpression );

    /**
     * The identifier value of the currently selected feature. A value from the
     * identifierField.
     */
    QVariant identifierValue() const;

    /**
     * The identifier value of the currently selected feature. A value from the
     * identifierField.
     */
    void setIdentifierValue( const QVariant &identifierValue );

    /**
     * Shorthand for getting a feature request to query the currently selected
     * feature.
     */
    QgsFeatureRequest currentFeatureRequest() const;

    /**
     * Determines if a NULL value should be available in the list.
     */
    bool allowNull() const;

    /**
     * Determines if a NULL value should be available in the list.
     */
    void setAllowNull( bool allowNull );

    /**
     * Field name that will be used to uniquely identify the current feature.
     * Normally the primary key of the layer.
     */
    QString identifierField() const;

    /**
     * Field name that will be used to uniquely identify the current feature.
     * Normally the primary key of the layer.
     */
    void setIdentifierField( const QString &identifierField );

  signals:

    /**
      * Filters fields have changed
      */
    void filterFieldsChanged();

    /**
     * The underlying model has been updated.
     */
    void modelUpdated();

    /**
     * The layer from which features should be listed.
     */
    void sourceLayerChanged();

    /**
     * The display expression will be used to display features as well as
     * the the value to match the typed text against.
     */
    void displayExpressionChanged();

    /**
     * An additional expression to further restrict the available features.
     * This can be used to integrate additional spatial or other constraints.
     */
    void filterExpressionChanged();

    /**
     * The identifier value of the currently selected feature. A value from the
     * identifierField.
     */
    void identifierValueChanged();

    /**
     * Field name that will be used to uniquely identify the current feature.
     * Normally the primary key of the layer.
     */
    void identifierFieldChanged();

    /**
     * Determines if a NULL value should be available in the list.
     */
    void allowNullChanged();

  private slots:
    void filtersChanged();

  private:
    QgsFeatureListComboBox *mFeatureListComboBox;
    QHBoxLayout *mFiltersLayout;
    QMap<QString, QComboBox *> mFilterFieldsComboBoxes;
};

#endif // QGSFILTERFEATUREPICKER_H
