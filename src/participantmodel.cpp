#include "participantmodel.h"

ParticipantModel::ParticipantModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    ohmcomm::rtp::ParticipantDatabase::registerListener(*this);
}

ParticipantModel::~ParticipantModel()
{
    ohmcomm::rtp::ParticipantDatabase::unregisterListener(*this);
}

QVariant ParticipantModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

int ParticipantModel::rowCount(const QModelIndex &parent) const
{
    return ohmcomm::rtp::ParticipantDatabase::getNumberOfRemotes();
}

int ParticipantModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant ParticipantModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        
    }
    if(role == Qt::ToolTipRole)
    {
        
    }
    return QVariant();
}

bool ParticipantModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if (!parent.isValid()) {
        // We need to append a row to the bottom of the view.
        int rows = rowCount() - 1;

        beginInsertRows(parent, rows, rows);
        // Do nothing, but just inform view(s) that the internal data has changed
        // and these rows should be added.
        endInsertRows();
        return true;
    }
    return QAbstractItemModel::insertRows(row, count, parent);
}

bool ParticipantModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count);
    endRemoveRows();
}


void ParticipantModel::onRemoteAdded(const unsigned int ssrc)
{
    insertRow(0);
}

void ParticipantModel::onRemoteConnected(const unsigned int ssrc, const std::string& address, const unsigned short port)
{
    dataChanged(toModelIndex(ssrc), toModelIndex(ssrc));
}

void ParticipantModel::onRemoteRemoved(const unsigned int ssrc)
{
    removeRow(toModelIndex(ssrc).row());
}

QModelIndex ParticipantModel::toModelIndex(const unsigned int ssrc) const
{
    
}

unsigned int ParticipantModel::toSSRC(const QModelIndex& index) const
{
    
}
