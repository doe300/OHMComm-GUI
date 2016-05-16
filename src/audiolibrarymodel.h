#ifndef AUDIOLIBRARYMODEL_H
#define AUDIOLIBRARYMODEL_H

#include <QAbstractListModel>

class AudioLibraryModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit AudioLibraryModel(QObject *parent = 0);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

#endif // AUDIOLIBRARYMODEL_H