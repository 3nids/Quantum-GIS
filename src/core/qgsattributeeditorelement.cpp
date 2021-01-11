/***************************************************************************
  qgsattributeeditorelement.cpp - QgsAttributeEditorElement

 ---------------------
 begin                : 18.8.2016
 copyright            : (C) 2016 by Matthias Kuhn
 email                : matthias@opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qgsattributeeditorelement.h"
#include "qgsrelationmanager.h"
#include "qgsxmlutils.h"


void QgsAttributeEditorContainer::addChildElement( QgsAttributeEditorElement *widget )
{
  mChildren.append( widget );
}

void QgsAttributeEditorContainer::setName( const QString &name )
{
  mName = name;
}

QgsOptionalExpression QgsAttributeEditorContainer::visibilityExpression() const
{
  return mVisibilityExpression;
}

void QgsAttributeEditorContainer::setVisibilityExpression( const QgsOptionalExpression &visibilityExpression )
{
  if ( visibilityExpression == mVisibilityExpression )
    return;

  mVisibilityExpression = visibilityExpression;
}

QColor QgsAttributeEditorContainer::backgroundColor() const
{
  return mBackgroundColor;
}

void QgsAttributeEditorContainer::setBackgroundColor( const QColor &backgroundColor )
{
  mBackgroundColor = backgroundColor;
}

QList<QgsAttributeEditorElement *> QgsAttributeEditorContainer::findElements( QgsAttributeEditorElement::AttributeEditorType type ) const
{
  QList<QgsAttributeEditorElement *> results;

  const auto constMChildren = mChildren;
  for ( QgsAttributeEditorElement *elem : constMChildren )
  {
    if ( elem->type() == type )
    {
      results.append( elem );
    }

    if ( elem->type() == AeTypeContainer )
    {
      QgsAttributeEditorContainer *cont = dynamic_cast<QgsAttributeEditorContainer *>( elem );
      if ( cont )
        results += cont->findElements( type );
    }
  }

  return results;
}

void QgsAttributeEditorContainer::clear()
{
  qDeleteAll( mChildren );
  mChildren.clear();
}

int QgsAttributeEditorContainer::columnCount() const
{
  return mColumnCount;
}

void QgsAttributeEditorContainer::setColumnCount( int columnCount )
{
  mColumnCount = columnCount;
}

QgsAttributeEditorElement *QgsAttributeEditorContainer::clone( QgsAttributeEditorElement *parent ) const
{
  QgsAttributeEditorContainer *element = new QgsAttributeEditorContainer( name(), parent );

  const auto childElements = children();

  for ( QgsAttributeEditorElement *child : childElements )
  {
    element->addChildElement( child->clone( element ) );
  }
  element->mIsGroupBox = mIsGroupBox;
  element->mColumnCount = mColumnCount;
  element->mVisibilityExpression = mVisibilityExpression;

  return element;
}

QVariantMap QgsAttributeEditorContainer::elementConfiguration() const
{
  QVariantMap configuration;
  configuration[QStringLiteral( "columnCount" )] = mColumnCount;
  configuration[QStringLiteral( "groupBox" ) ] = mIsGroupBox;
  configuration[QStringLiteral( "visibilityExpressionEnabled" ) ] = mVisibilityExpression.enabled();
  configuration[QStringLiteral( "visibilityExpression" ) ] = mVisibilityExpression->expression();
  if ( mBackgroundColor.isValid() )
    configuration[QStringLiteral( "backgroundColor" ) ] = mBackgroundColor.name( );

  QVariantList childrenConfigs;
  for ( QgsAttributeEditorElement *child : qgis::as_const( mChildren ) )
  {
    childrenConfigs << child->configuration();
  }
  configuration[QStringLiteral( "children" ) ] = childrenConfigs;
  return configuration;
}

void QgsAttributeEditorContainer::saveConfiguration( QDomElement &elem ) const
{
  elem.setAttribute( QStringLiteral( "columnCount" ), mColumnCount );
  elem.setAttribute( QStringLiteral( "groupBox" ), mIsGroupBox ? 1 : 0 );
  elem.setAttribute( QStringLiteral( "visibilityExpressionEnabled" ), mVisibilityExpression.enabled() ? 1 : 0 );
  elem.setAttribute( QStringLiteral( "visibilityExpression" ), mVisibilityExpression->expression() );
  if ( mBackgroundColor.isValid() )
    elem.setAttribute( QStringLiteral( "backgroundColor" ), mBackgroundColor.name( ) );
  const auto constMChildren = mChildren;
  for ( QgsAttributeEditorElement *child : constMChildren )
  {
    QDomDocument doc = elem.ownerDocument();
    elem.appendChild( child->toDomElement( doc ) );
  }
}

void QgsAttributeEditorContainer::loadElementConfiguration( const QVariantMap &config, const QString &layerId, QgsReadWriteContext &context, const QgsFields &fields )
{
  mBackgroundColor = config.value( QStringLiteral( "backgroundColor" ), QColor() ).value<QColor>();
  int cc = config.value( QStringLiteral( "columnCount" ), 1 ).toInt();
  setColumnCount( cc );

  if ( config.keys().contains( "groupBox" ) )
  {
    bool isGroupBox = config.value( QStringLiteral( "groupBox" ) ).toBool();
    setIsGroupBox( isGroupBox );
  }
  else
  {
    setIsGroupBox( mParent );
  }

  QgsOptionalExpression visibilityExpression;
  visibilityExpression.setEnabled( config.value( QStringLiteral( "visibilityExpressionEnabled" ), false ).toBool() );
  visibilityExpression.setData( QgsExpression( config.value( QStringLiteral( "visibilityExpression" ) ).toString() ) );
  setVisibilityExpression( visibilityExpression );

  const QVariantList children = config.value( QStringLiteral( "children" ) ).toList();
  for ( const QVariant &child : children )
  {
    QgsAttributeEditorElement *elem = QgsAttributeEditorElement::create( child.toMap(), layerId, fields, context, mParent );
    addChildElement( elem );
  }
}

QString QgsAttributeEditorContainer::typeIdentifier() const
{
  return QStringLiteral( "attributeEditorContainer" );
}


QgsAttributeEditorElement *QgsAttributeEditorField::clone( QgsAttributeEditorElement *parent ) const
{
  QgsAttributeEditorField *element = new QgsAttributeEditorField( name(), mIdx, parent );

  return element;
}

bool QgsAttributeEditorRelation::init( QgsRelationManager *relationManager )
{
  mRelation = relationManager->relation( mRelationId );
  return mRelation.isValid();
}

QgsAttributeEditorElement *QgsAttributeEditorRelation::clone( QgsAttributeEditorElement *parent ) const
{
  QgsAttributeEditorRelation *element = new QgsAttributeEditorRelation( mRelationId, parent );
  element->mRelation = mRelation;
  element->mButtons = mButtons;
  element->mForceSuppressFormPopup = mForceSuppressFormPopup;
  element->mNmRelationId = mNmRelationId;
  element->mLabel = mLabel;
  element->mRelationEditorConfig = mRelationEditorConfig;

  return element;
}
QVariantMap QgsAttributeEditorField::elementConfiguration( ) const
{
  QVariantMap configuration;
  configuration[QStringLiteral( "index" )] = mIdx;
  return configuration;
}

void QgsAttributeEditorField::saveConfiguration( QDomElement &elem ) const
{
  elem.setAttribute( QStringLiteral( "index" ), mIdx );
}

void QgsAttributeEditorField::loadElementConfiguration( const QVariantMap &config, const QString &layerId, QgsReadWriteContext &context, const QgsFields &fields )
{
  Q_UNUSED( config )
  Q_UNUSED( layerId )
  Q_UNUSED( context )
  Q_UNUSED( fields )
}

QString QgsAttributeEditorField::typeIdentifier() const
{
  return QStringLiteral( "attributeEditorField" );
}

QDomElement QgsAttributeEditorElement::toDomElement( QDomDocument &doc ) const
{
  QDomElement elem = doc.createElement( typeIdentifier() );
  elem.setAttribute( QStringLiteral( "name" ), mName );
  elem.setAttribute( QStringLiteral( "showLabel" ), mShowLabel );
  saveConfiguration( elem );
  return elem;
}

QVariantMap QgsAttributeEditorElement::configuration( ) const
{
  QVariantMap configuration;
  configuration[ QStringLiteral( "type" )] = typeIdentifier() ;
  configuration[ QStringLiteral( "name" )] = mName ;
  configuration[ QStringLiteral( "showLabel" )] = mShowLabel ;
  configuration[ QStringLiteral( "configuration" )] = elementConfiguration();
  return configuration;
}

QgsAttributeEditorElement *QgsAttributeEditorElement::create( const QVariantMap &configuration,
    const QString &layerId,
    const QgsFields &fields,
    QgsReadWriteContext &context,
    QgsAttributeEditorElement *parent )
{
  QgsAttributeEditorElement *newElement = nullptr;

  QString elementType = configuration["type"].toString();
  QString name = configuration["name"].toString();

  if ( elementType == QLatin1String( "attributeEditorContainer" ) )
  {
    newElement = new QgsAttributeEditorContainer( context.projectTranslator()->translate( QStringLiteral( "project:layers:%1:formcontainers" ).arg( layerId ), name ), parent );
  }
  else if ( elementType == QLatin1String( "attributeEditorField" ) )
  {
    int idx = fields.lookupField( name );
    newElement = new QgsAttributeEditorField( name, idx, parent );
  }
  else if ( elementType == QLatin1String( "attributeEditorRelation" ) )
  {
    // At this time, the relations are not loaded
    // So we only grab the id and delegate the rest to init()
    newElement = new QgsAttributeEditorRelation( QString(), parent );
  }
  else if ( elementType == QLatin1String( "attributeEditorQmlElement" ) )
  {
    newElement = new QgsAttributeEditorQmlElement( name, parent );
  }
  else if ( elementType == QLatin1String( "attributeEditorHtmlElement" ) )
  {
    newElement = new QgsAttributeEditorHtmlElement( name, parent );
  }

  if ( newElement )
  {
    newElement->setShowLabel( configuration.value( QStringLiteral( "showLabel" ), true ).toBool() );
    newElement->loadElementConfiguration( configuration, layerId, context, fields );
  }

  return newElement;
}

bool QgsAttributeEditorElement::showLabel() const
{
  return mShowLabel;
}

void QgsAttributeEditorElement::setShowLabel( bool showLabel )
{
  mShowLabel = showLabel;
}

void QgsAttributeEditorElement::loadElementConfiguration( const QVariantMap &config, const QString &layerId, QgsReadWriteContext &context, const QgsFields &fields )
{
  // TODO QGIS 4: make it pure virtual
  // do not write code here, put anything into static method create
  Q_UNUSED( config )
  Q_UNUSED( layerId )
  Q_UNUSED( context )
  Q_UNUSED( fields )
}

QVariantMap QgsAttributeEditorRelation::relationEditorConfiguration() const
{
  return mRelationEditorConfig;
}

void QgsAttributeEditorRelation::setRelationEditorConfiguration( const QVariantMap &config )
{
  mRelationEditorConfig = config;
}

QVariantMap QgsAttributeEditorRelation::elementConfiguration( ) const
{
  QVariantMap configuration;
  configuration[QStringLiteral( "relation" ) ] = mRelation.id() ;
  configuration[QStringLiteral( "forceSuppressFormPopup" )] = mForceSuppressFormPopup ;
  configuration[QStringLiteral( "nmRelationId" )] = mNmRelationId.toString() ;
  configuration[QStringLiteral( "label" )] = mLabel ;
  configuration[QStringLiteral( "relationWidgetTypeId" )] = mRelationWidgetTypeId ;
  configuration[QStringLiteral( "relationEditorConfiguration" ) ] = mRelationEditorConfig;
  return configuration;
}

void QgsAttributeEditorRelation::saveConfiguration( QDomElement &elem ) const
{
  elem.setAttribute( QStringLiteral( "relation" ), mRelation.id() );
  elem.setAttribute( QStringLiteral( "buttons" ), mRelationEditorConfig.value( QStringLiteral( "buttons" ) ).toString() );
  elem.setAttribute( QStringLiteral( "forceSuppressFormPopup" ), mForceSuppressFormPopup );
  elem.setAttribute( QStringLiteral( "nmRelationId" ), mNmRelationId.toString() );
  elem.setAttribute( QStringLiteral( "label" ), mLabel );
}

QString QgsAttributeEditorRelation::typeIdentifier() const
{
  return QStringLiteral( "attributeEditorRelation" );
}

void QgsAttributeEditorRelation::setForceSuppressFormPopup( bool forceSuppressFormPopup )
{
  mForceSuppressFormPopup = forceSuppressFormPopup;
}

bool QgsAttributeEditorRelation::forceSuppressFormPopup() const
{
  return mForceSuppressFormPopup;
}

void QgsAttributeEditorRelation::setNmRelationId( const QVariant &nmRelationId )
{
  mNmRelationId = nmRelationId;
}

QVariant QgsAttributeEditorRelation::nmRelationId() const
{
  return mNmRelationId;
}

void QgsAttributeEditorRelation::setLabel( const QString &label )
{
  mLabel = label;
}

QString QgsAttributeEditorRelation::label() const
{
  return mLabel;
}

QString QgsAttributeEditorRelation::relationWidgetTypeId() const
{
  return mRelationWidgetTypeId;
}

void QgsAttributeEditorRelation::setRelationWidgetTypeId( const QString &relationWidgetTypeId )
{
  mRelationWidgetTypeId = relationWidgetTypeId;
}

void QgsAttributeEditorRelation::loadElementConfiguration( const QVariantMap &config, const QString &layerId, QgsReadWriteContext &context, const QgsFields &fields )
{
  Q_UNUSED( layerId )
  Q_UNUSED( context )
  Q_UNUSED( fields )
  mRelationId = config.value( QStringLiteral( "relation" ), QStringLiteral( "[None]" ) ).toString();
  mRelationEditorConfig = config.value( QStringLiteral( "relationEditorConfiguration" ) ).toMap();
  mForceSuppressFormPopup = config.value( QStringLiteral( "forceSuppressFormPopup" ) ).toBool();
  mNmRelationId = config.value( QStringLiteral( "nmRelationId" ) ).toString();
  mLabel = config.value( QStringLiteral( "label" ) ).toString();
  mRelationWidgetTypeId = config.value( QStringLiteral( "relationWidgetTypeId" ) ).toString();
}

QgsAttributeEditorElement *QgsAttributeEditorQmlElement::clone( QgsAttributeEditorElement *parent ) const
{
  QgsAttributeEditorQmlElement *element = new QgsAttributeEditorQmlElement( name(), parent );
  element->setQmlCode( mQmlCode );

  return element;
}

QString QgsAttributeEditorQmlElement::qmlCode() const
{
  return mQmlCode;
}

void QgsAttributeEditorQmlElement::setQmlCode( const QString &qmlCode )
{
  mQmlCode = qmlCode;
}

QVariantMap QgsAttributeEditorQmlElement::elementConfiguration( ) const
{
  QVariantMap configuration;
  configuration[QStringLiteral( "qml_code" )] = mQmlCode;
  return configuration;
}

void QgsAttributeEditorQmlElement::saveConfiguration( QDomElement &elem ) const
{
  QDomText codeElem = elem.ownerDocument().createTextNode( mQmlCode );
  elem.appendChild( codeElem );
}

void QgsAttributeEditorQmlElement::loadElementConfiguration( const QVariantMap &config, const QString &layerId, QgsReadWriteContext &context, const QgsFields &fields )
{
  Q_UNUSED( config )
  Q_UNUSED( layerId )
  Q_UNUSED( context )
  Q_UNUSED( fields )
  mQmlCode = config.value( QStringLiteral( "qml_code" ) ).toString();
}

QString QgsAttributeEditorQmlElement::typeIdentifier() const
{
  return QStringLiteral( "attributeEditorQmlElement" );
}

QgsAttributeEditorElement *QgsAttributeEditorHtmlElement::clone( QgsAttributeEditorElement *parent ) const
{
  QgsAttributeEditorHtmlElement *element = new QgsAttributeEditorHtmlElement( name(), parent );
  element->setHtmlCode( mHtmlCode );

  return element;
}

QString QgsAttributeEditorHtmlElement::htmlCode() const
{
  return mHtmlCode;
}

void QgsAttributeEditorHtmlElement::setHtmlCode( const QString &htmlCode )
{
  mHtmlCode = htmlCode;
}

QVariantMap QgsAttributeEditorHtmlElement::elementConfiguration( ) const
{
  QVariantMap configuration;
  configuration[QStringLiteral( "html_code" )] = mHtmlCode;
  return configuration;
}

void QgsAttributeEditorHtmlElement::saveConfiguration( QDomElement &elem ) const
{
  QDomText codeElem = elem.ownerDocument().createTextNode( mHtmlCode );
  elem.appendChild( codeElem );
}

void QgsAttributeEditorHtmlElement::loadElementConfiguration( const QVariantMap &config, const QString &layerId, QgsReadWriteContext &context, const QgsFields &fields )
{
  Q_UNUSED( config )
  Q_UNUSED( layerId )
  Q_UNUSED( context )
  Q_UNUSED( fields )
  mHtmlCode = config.value( QStringLiteral( "html_code" ) ).toString();
}

QString QgsAttributeEditorHtmlElement::typeIdentifier() const
{
  return QStringLiteral( "attributeEditorHtmlElement" );
}
