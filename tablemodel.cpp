#include "tablemodel.h"

TableModel::TableModel(QObject *parent) :
    QStandardItemModel(parent)
{
//    loss_data = new float*[48];
//    for(int i =0; i < 48; i++){
//        loss_data[i] = new float[200];
//    }
}

//int TableModel::rowCount(const QModelIndex &parent) const{
//    return 200;
//}

//int TableModel::columnCount(const QModelIndex &parent) const{
//    return 48;
//}

//QVariant TableModel::data(const QModelIndex &index, int role) const{
//    if (Qt::DisplayRole == role)
//    {
//        return loss_data[index.row()][index.column()];//返回num二维数组中的值
//    }
//    return QVariant::Invalid;
//}

//Qt::ItemFlags TableModel::flags(const QModelIndex &index) const{
//    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
//    flags |= Qt::ItemIsEditable;
//    return flags;
//}

//bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role){
//    if (index.isValid() && role == Qt::EditRole){
//        loss_data[index.row()][index.column()] = value.toFloat();
//        emit dataChanged(index, index);
//        return true;
//    }
//    return false;
//}
