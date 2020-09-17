#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QFlags>

#define ROW 50
#define COL 40

class TableModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit TableModel(QObject *parent = 0);

//    int rowCount(const QModelIndex &parent) const;
//    int columnCount(const QModelIndex &parent) const;
//    QVariant data(const QModelIndex &index, int role) const;

//    Qt::ItemFlags flags(const QModelIndex &index) const;
//    bool setData(const QModelIndex &index, const QVariant &value, int role);
    //float **loss_data;
    float loss_data[48][200];
signals:

public slots:

};

#endif // TABLEMODEL_H
