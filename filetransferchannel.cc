/*
    Copyright (C) 2015 Niels Ole Salscheider <niels_ole@salscheider-online.de>

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "filetransferchannel.hh"
#include "common.hh"
#include "connection.hh"

FileTransferChannel::FileTransferChannel(Connection *connection, Tp::BaseChannel *baseChannel, const QString &contentType, const QString &filename, qulonglong size,
                                         uint contentHashType, const QString &contentHash, const QString &description, const QDateTime &date)
    : Tp::BaseChannelFileTransferType(contentType, filename, size, contentHashType, contentHash, description, date),
      m_connection(connection), m_outgoingTransfer(baseChannel->requested())
{
    DBG;

    m_fileInfo.setDate(date);
    m_fileInfo.setHash(contentHash.toLatin1());
    m_fileInfo.setName(filename);
    m_fileInfo.setDescription(description);
    m_fileInfo.setSize(size);

    m_ioDevice = new QBuffer;
    /* Open the QBuffer - QXmpp requires this */
    m_ioDevice->open(QIODevice::ReadWrite);

    setGetIODeviceCallback(Tp::memFun(this, &FileTransferChannel::getIODevice));
    bool success;
    success = connect(this, SIGNAL(stateChanged(uint, uint)), this, SLOT(onStateChanged(uint, uint)));
    Q_ASSERT(success);

    if (m_outgoingTransfer) {
        QXmppTransferManager *transferManager = m_connection->qxmppClient()->findExtension<QXmppTransferManager>();
        Q_ASSERT(transferManager);
        m_transferJob = transferManager->sendFile(baseChannel->targetID() + m_connection->lastResourceForJid(baseChannel->targetID(), true), m_ioDevice, m_fileInfo);
        Q_ASSERT(m_transferJob->error() == QXmppTransferJob::NoError);
        success = connect(m_transferJob, SIGNAL(stateChanged(QXmppTransferJob::State)), this, SLOT(onQxmppTransferSateChanged(QXmppTransferJob::State)));
        Q_ASSERT(success);
        success = connect(m_transferJob, SIGNAL(error(QXmppTransferJob::Error)), this, SLOT(onTransferError(QXmppTransferJob::Error)));
        Q_ASSERT(success);
    }
}

FileTransferChannelPtr FileTransferChannel::create(Connection *connection, Tp::BaseChannel *baseChannel, const QString &contentType, const QString &filename, qulonglong size,
                                                   uint contentHashType, const QString &contentHash, const QString &description, const QDateTime &date)
{
    return FileTransferChannelPtr(new FileTransferChannel(connection, baseChannel, contentType, filename, size, contentHashType, contentHash, description, date));
}

QIODevice* FileTransferChannel::getIODevice(qulonglong offset, Tp::DBusError* error)
{
    DBG;

    if (m_outgoingTransfer) {
        setInitialOffset(0); /* The initial offset is always 0 for QXmpp */
    }

    return m_ioDevice;
}

void FileTransferChannel::onFileReceived(QXmppTransferJob* job)
{
    DBG;

    m_transferJob = job;
    bool success;
    success = connect(m_transferJob, SIGNAL(stateChanged(QXmppTransferJob::State)), this, SLOT(onQxmppTransferSateChanged(QXmppTransferJob::State)));
    Q_ASSERT(success);
    success = connect(m_transferJob, SIGNAL(error(QXmppTransferJob::Error)), this, SLOT(onTransferError(QXmppTransferJob::Error)));
    Q_ASSERT(success);
}

/* The state of the CM changes */
void FileTransferChannel::onStateChanged(uint state, uint reason)
{
    DBG << state;
    switch (state) {
    case Tp::FileTransferStateNone:
    case Tp::FileTransferStatePending:
        break;
    case Tp::FileTransferStateAccepted:
        if (!m_outgoingTransfer) {
            setInitialOffset(0); /* The initial offset is always 0 for QXmpp */
            m_transferJob->accept(m_ioDevice);
        }
        break;
    case Tp::FileTransferStateOpen:
    case Tp::FileTransferStateCompleted:
    case Tp::FileTransferStateCancelled:
        break;
    default:
        Q_ASSERT(0);
    }
}

/* QXmpp's state changes */
void FileTransferChannel::onQxmppTransferSateChanged(QXmppTransferJob::State state)
{
    DBG << state;

    switch (state) {
    case QXmppTransferJob::TransferState:
        if (m_outgoingTransfer) {
            remoteAcceptFile(0); /* The initial offset is always 0 for QXmpp */
        }
    case QXmppTransferJob::StartState:
    case QXmppTransferJob::FinishedState:
        /* Nothing to do */
        break;
    default:
        Q_ASSERT(0);
    }
}

void FileTransferChannel::onTransferError(QXmppTransferJob::Error error)
{
    /* TODO */
    qDebug() << "TRANSFER ERROR: " << error;
}
