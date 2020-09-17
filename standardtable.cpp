#include "standardtable.h"
#include <QDebug>
StandardTable::StandardTable( int row, int col, QObject *parent) :
    QStandardItemModel(row, col, parent)
{
}

//QVariant StandardTable::data(const QModelIndex &index, int role) const{
//    if(index.row()==0){

//        return this->itemData(index).value();
//    }
//    if (Qt::DisplayRole == role){
//        int band = this->headerData(index.row(),Qt::Vertical).toInt();
//        QString ant = this->headerData(index.column(), Qt::Horizontal).toString();
//        qDebug()<< index.column()<<" "<<index.row()<<" "<<ant+"/"+QString::number(band)
//                <<" "<<loss_data[ant+band];
//        QVariant val = loss_data[ant+"/"+QString::number(band)];
//        if(val.toInt() == 0)
//            return QVariant::Invalid;
//        return loss_data[ant+"/"+QString::number(band)] ;//返回num二维数组中的值
//    }
//return QVariant::Invalid;
//}
