#include "participantmodel.h"

#include <QListView>

#include "rtp/RTCPData.h"

ParticipantModel::ParticipantModel(QObject *parent): QAbstractListModel(parent)
{
    ohmcomm::rtp::ParticipantDatabase::registerListener(*this);
    
    connect(this, SIGNAL(onRemoteAdded(unsigned int)), this, SLOT(handleNewParticipant(unsigned int)));
    connect(this, SIGNAL(onRemoteRemoved(unsigned int)), this, SLOT(handleRemoveParticipant(unsigned int)));
    connect(this, SIGNAL(remoteConnected(unsigned int)), this, SLOT(handleNewParticipant(unsigned int)));
}

ParticipantModel::~ParticipantModel()
{
    ohmcomm::rtp::ParticipantDatabase::unregisterListener(*this);
}

int ParticipantModel::rowCount(const QModelIndex &parent) const
{
    return ohmcomm::rtp::ParticipantDatabase::getNumberOfRemotes();
}

QVariant ParticipantModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    const auto& remote = ohmcomm::rtp::ParticipantDatabase::getAllRemoteParticipants().find(toSSRC(index));

    if(role == Qt::DisplayRole)
    {
        if((*remote).second.rtcpData.get() != nullptr)
        {
            return QVariant(QString((*(*remote).second.rtcpData)[ohmcomm::rtp::RTCP_SOURCE_NAME].data()));
        }
        return QVariant(QString("%1").arg((*remote).first, 16));
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
    return true;
}

void ParticipantModel::onRemoteConnected(const unsigned int ssrc, const std::string& address, const unsigned short port)
{
    remoteConnected(ssrc);
}

QModelIndex ParticipantModel::toModelIndex(const unsigned int ssrc) const
{
    const auto& remotes = ohmcomm::rtp::ParticipantDatabase::getAllRemoteParticipants();
    unsigned int ind = 0;
    auto it = remotes.begin();
    while(it != remotes.end())
    {
        if((*it).first == ssrc)
            return index(ind);
        ++ind;
    }
    return QModelIndex();
}

unsigned int ParticipantModel::toSSRC(const QModelIndex& index) const
{
    const auto& remotes = ohmcomm::rtp::ParticipantDatabase::getAllRemoteParticipants();
    int ind = 0;
    auto it = remotes.begin();
    while(it != remotes.end())
    {
        if(ind == index.row())
            return (*it).first;
        ++ind;
    }
    return UINT_MAX;
}

void ParticipantModel::fireParticipantSelected(const QModelIndex& current, const QModelIndex& previous)
{
    participantSelected(toSSRC(current));
}

void ParticipantModel::handleNewParticipant(const unsigned int ssrc)
{
    insertRow(0);
}

void ParticipantModel::handleRemoteConnected(const unsigned int ssrc)
{
    dataChanged(toModelIndex(ssrc), toModelIndex(ssrc));
}

void ParticipantModel::handleRemoveParticipant(const unsigned int ssrc)
{
    removeRow(toModelIndex(ssrc).row());
}
