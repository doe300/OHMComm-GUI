#ifndef PARTICIPANTMODEL_H
#define PARTICIPANTMODEL_H

#include <QAbstractTableModel>

#include "rtp/ParticipantDatabase.h"

class ParticipantModel : public QAbstractTableModel, private ohmcomm::rtp::ParticipantListener
{
    Q_OBJECT

public:
    explicit ParticipantModel(QObject *parent = 0);
    
    ~ParticipantModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    
    bool insertRows(int row, int count, const QModelIndex& parent) override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;


    void onRemoteAdded(const unsigned int ssrc) override;
    void onRemoteConnected(const unsigned int ssrc, const std::string& address, const unsigned short port) override;
    void onRemoteRemoved(const unsigned int ssrc) override;
    
Q_SIGNALS:
    void participantSelected(uint32_t ssrc);

public Q_SLOTS:
    void fireParticipantSelected(const QModelIndex& current, const QModelIndex& previous);
    
private:
    
    QModelIndex toModelIndex(const unsigned int ssrc) const;
    unsigned int toSSRC(const QModelIndex& index) const;
};

#endif // PARTICIPANTMODEL_H