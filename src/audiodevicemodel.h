#ifndef AUDIODEVICEMODEL_H
#define AUDIODEVICEMODEL_H

#include <memory>

#include <QAbstractListModel>

#include "audio/AudioHandlerFactory.h"

class AudioDeviceModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit AudioDeviceModel(const bool inputDevice, QObject *parent = 0);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public Q_SLOTS:
    void onUpdateLibrary(const QString& libraryName);
    
private:
    const bool inputDevice;
    std::unique_ptr<ohmcomm::AudioHandler> currentHandler;
};

#endif // AUDIODEVICEMODEL_H