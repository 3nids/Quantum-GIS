#ifndef QGSSVGPARAMETER_H
#define QGSSVGPARAMETER_H

#include "qgis_core.h"

#include <QList>
#include <QString>

/**
\ingroup core
A simple class to hold informations for SVG dynamic parameters
\since QGIS 3.18
*/
class CORE_EXPORT QgsSvgParameter
{
  public:
    QgsSvgParameter() = default;

    QgsSvgParameter( const QString &name, const QString &value )
      : mValid( !name.isEmpty() ), mName( name ), mValue( value ) {}

    QString name() const {return mName;}
    void setName( const QString &name ) {mName = name;}
    QString value() const {return mValue;}
    void setValue( const QString &value ) {mValue = value;}

  private:
    bool mValid = false;
    QString mName;
    QString mValue;
};

typedef QList<QgsSvgParameter> QgsSvgParameters;

#endif // QGSSVGPARAMETER_H
