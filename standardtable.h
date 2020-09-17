#ifndef STANDARDTABLE_H
#define STANDARDTABLE_H

#include <QStandardItemModel>
#include <QMap>
#include <QString>
#include <QList>
#include <QVector>

class StandardTable : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit StandardTable(int row, int col, QObject *parent = 0);
//    QVariant data(const QModelIndex &index, int role) const;
    QMap<QString, float> *loss_data;

signals:

public slots:

};

#endif // STANDARDTABLE_H
