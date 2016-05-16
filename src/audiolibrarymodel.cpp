#include "audiolibrarymodel.h"

#include "audio/AudioHandlerFactory.h"

AudioLibraryModel::AudioLibraryModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int AudioLibraryModel::rowCount(const QModelIndex &parent) const
{
    return ohmcomm::AudioHandlerFactory::getAudioHandlerNames().size();
}

QVariant AudioLibraryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    const std::string libraryName = ohmcomm::AudioHandlerFactory::getAudioHandlerNames()[index.row()];
    
    if(role == Qt::DisplayRole)
    {
        return QVariant(QString(libraryName.data()));
    }
    return QVariant();
}
