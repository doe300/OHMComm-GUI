#ifndef FORMATSMODEL_H
#define FORMATSMODEL_H

#include <QAbstractListModel>

class FormatsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit FormatsModel(QObject *parent = 0);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

#endif // FORMATSMODEL_H