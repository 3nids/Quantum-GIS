#ifndef QGSOPTIONSDIALOGHIGHLIGHTWIDGETSIMPL_H
#define QGSOPTIONSDIALOGHIGHLIGHTWIDGETSIMPL_H

#include <QObject>
#include <QMap>
#include <QBrush>


#include "qgis_gui.h"
#include "qgis_sip.h"
#include "qgsoptionsdialoghighlightwidget.h"

class QLabel;
class QCheckBox;
class QAbstractButton;
class QGroupBox;
class QTreeView;
class QTreeWidgetItem;


class GUI_EXPORT QgsOptionsDialogHighlightInvalid : public QgsOptionsDialogHighlightWidget
{
    Q_OBJECT
  public:
    QgsOptionsDialogHighlightInvalid() : QgsOptionsDialogHighlightWidget() {}
  private:
    bool searchText( const QString &searchText ) {Q_UNUSED( searchText ); return false;}
    bool highlightText( const QString &searchText ) {Q_UNUSED( searchText ); return false;}
    void reset() {}
};
class GUI_EXPORT QgsOptionsDialogHighlightLabel : public QgsOptionsDialogHighlightWidget
{
    Q_OBJECT
  public:
    QgsOptionsDialogHighlightLabel( QLabel *label );
  private:
    bool searchText( const QString &text ) override;
    bool highlightText( const QString &text ) override;
    void reset() override;
    QPointer<QLabel> mLabel;
    QString mStyleSheet = QStringLiteral( /*!search!*/"QLabel { background-color: yellow; color: blue;}/*!search!*/" );
};
class GUI_EXPORT QgsOptionsDialogHighlightCheckBox : public QgsOptionsDialogHighlightWidget
{
    Q_OBJECT
  public:
    QgsOptionsDialogHighlightCheckBox( QCheckBox *checkBox );
  private:
    bool searchText( const QString &text ) override;
    bool highlightText( const QString &text ) override;
    void reset() override;
    QPointer<QCheckBox> mCheckBox;
    QString mStyleSheet = QStringLiteral( "/*!search!*/QCheckBox { background-color: yellow; color: blue;}/*!search!*/" );
};
class GUI_EXPORT QgsOptionsDialogHighlightButton : public QgsOptionsDialogHighlightWidget
{
    Q_OBJECT
  public:
    QgsOptionsDialogHighlightButton( QAbstractButton *button );
  private:
    bool searchText( const QString &text ) override;
    bool highlightText( const QString &text ) override;
    void reset() override;
    QPointer<QAbstractButton> mButton;
    QString mStyleSheet = QStringLiteral( "/*!search!*/QAbstractButton { background-color: yellow; color: blue;}/*!search!*/" );
};
class GUI_EXPORT QgsOptionsDialogHighlightGroupBox : public QgsOptionsDialogHighlightWidget
{
    Q_OBJECT
  public:
    QgsOptionsDialogHighlightGroupBox( QGroupBox *groupBox );
  private:
    bool searchText( const QString &text ) override;
    bool highlightText( const QString &text ) override;
    void reset() override;
    QPointer<QGroupBox> mGroupBox;
    QString mStyleSheet = QStringLiteral( "/*!search!*/QGroupBox::title { background-color: yellow; color: blue;}/*!search!*/" );
};
class GUI_EXPORT QgsOptionsDialogHighlightTree : public QgsOptionsDialogHighlightWidget
{
    Q_OBJECT
  public:
    QgsOptionsDialogHighlightTree( QTreeView *treeView );
  private:
    bool searchText( const QString &text ) override;
    bool highlightText( const QString &text ) override;
    void reset() override;
    QPointer<QTreeView> mTreeView;
    // a map to save the tree state (backouground, font, expanded) before highlighting items
    QMap<QTreeWidgetItem *, QPair<QBrush, QBrush>> mTreeInitialStyle = QMap<QTreeWidgetItem *, QPair<QBrush, QBrush>>();
    QMap<QTreeWidgetItem *, bool> mTreeInitialExpand = QMap<QTreeWidgetItem *, bool>();
};
#endif // QGSOPTIONSDIALOGHIGHLIGHTWIDGETSIMPL_H
