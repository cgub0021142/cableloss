#include "tabledelegate.h"

TableDelegate::TableDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QWidget *TableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const{
    QLineEdit *line = new QLineEdit(parent);
    return line;
}

void TableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const{
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    QString str = index.model()->data(index,Qt::DisplayRole).toString();//获取model中的数据，角色为DisplayRole
    qDebug() << str;
    lineEdit->setText(str);
}

void TableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const{
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    model->setData(index,lineEdit->text(),Qt::EditRole);
}
