#ifndef PARTICIPANTMODEL_H
#define PARTICIPANTMODEL_H

#include <QAbstractTableModel>
#include <QTimer>

#include "rtp/ParticipantDatabase.h"

class ParticipantModel : public QAbstractListModel, private ohmcomm::rtp::ParticipantListener
{
    Q_OBJECT

public:
    explicit ParticipantModel(QObject *parent = 0);
    
    ~ParticipantModel();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    
    bool insertRows(int row, int count, const QModelIndex& parent) override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;
    
    void onRemoteConnected(const unsigned int ssrc, const std::string& address, const unsigned short port) override;

Q_SIGNALS:
    void onRemoteAdded(const unsigned int ssrc) override;
    void remoteConnected(const unsigned int ssrc);
    void onRemoteRemoved(const unsigned int ssrc) override;
    
    void participantSelected(uint32_t ssrc);

public Q_SLOTS:
    void fireParticipantSelected(const QModelIndex& current, const QModelIndex& previous);
    
private:
    
    QModelIndex toModelIndex(const unsigned int ssrc) const;
    unsigned int toSSRC(const QModelIndex& index) const;
    
private Q_SLOTS:
    void handleNewParticipant(const unsigned int ssrc);
    void handleRemoteConnected(const unsigned int ssrc);
    void handleRemoveParticipant(const unsigned int ssrc);
};

#endif // PARTICIPANTMODEL_H