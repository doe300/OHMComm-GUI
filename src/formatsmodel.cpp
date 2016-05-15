#include "formatsmodel.h"

#include "sip/SupportedFormats.h"

FormatsModel::FormatsModel(QObject *parent) : QAbstractListModel(parent)
{
}

int FormatsModel::rowCount(const QModelIndex &parent) const
{
    return ohmcomm::sip::SupportedFormats::getFormats().size();
}

QVariant FormatsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    const ohmcomm::sip::SupportedFormat& format = ohmcomm::sip::SupportedFormats::getFormats()[index.row()];
    
    if(role == Qt::DisplayRole)
    {
        //display name
        return QVariant(format.encoding.data());
    }
    if(role == Qt::ToolTipRole)
    {
        //tool-tip
        return QVariant(QString("%1 channels / %2 Hz").arg(format.numChannels).arg(format.sampleRate));
    }
    return QVariant();
}
