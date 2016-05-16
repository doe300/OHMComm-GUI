#include "audiodevicemodel.h"

AudioDeviceModel::AudioDeviceModel(const bool inputDevice, QObject *parent)
    : QAbstractListModel(parent), inputDevice(inputDevice), currentHandler(ohmcomm::AudioHandlerFactory::getAudioHandler(ohmcomm::AudioHandlerFactory::getDefaultAudioHandlerName()))
{
}

int AudioDeviceModel::rowCount(const QModelIndex &parent) const
{
    unsigned char count = 0;
    for(const ohmcomm::AudioDevice& dev : currentHandler->getAudioDevices())
    {
        if(inputDevice && dev.isInputDevice())
            ++count;
        if(!inputDevice && dev.isOutputDevice())
            ++count;
    }
    return count;
}

QVariant AudioDeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    unsigned char numDevice = 0;
    for(const ohmcomm::AudioDevice& dev : currentHandler->getAudioDevices())
    {
        if(inputDevice && dev.isInputDevice())
        {
            if(numDevice == index.row())
            {
                break;
            }
            ++numDevice;
        }
        if(!inputDevice && dev.isOutputDevice())
        {
            if(numDevice == index.row())
            {
                break;
            }
            ++numDevice;
        }
    }
    const ohmcomm::AudioDevice& device = currentHandler->getAudioDevices()[numDevice];
    if(role == Qt::DisplayRole)
    {
        return QVariant(QString(device.name.data()));
    }
    if(role == Qt::ToolTipRole)
    {
        return QVariant(QString("%1 channels %2").arg((inputDevice ? device.inputChannels : device.outputChannels)).arg(((inputDevice ? device.defaultInputDevice : device.defaultOutputDevice)) ? "(default)" : ""));
    }
    return QVariant();
}

void AudioDeviceModel::onUpdateLibrary(const QString& libraryName)
{
    currentHandler = std::move(ohmcomm::AudioHandlerFactory::getAudioHandler(libraryName.toStdString()));
    dataChanged(index(0), index(rowCount() - 1));
}