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

#ifndef FILETRANSFERCHANNEL_HH
#define FILETRANSFERCHANNEL_HH

#include <TelepathyQt/BaseChannel>

#include <QXmppTransferManager.h>

class FileTransferChannel;
class Connection;

typedef Tp::SharedPtr<FileTransferChannel> FileTransferChannelPtr;

class FileTransferChannel : public Tp::BaseChannelFileTransferType
{
    Q_OBJECT
public:
    static FileTransferChannelPtr create(Connection *connection, Tp::BaseChannel *baseChannel, const QString &contentType, const QString &filename, qulonglong size,
                                         uint contentHashType, const QString &contentHash, const QString &description, const QDateTime &date);

    void onFileReceived(QXmppTransferJob *job);

private:
    FileTransferChannel(Connection *connection, Tp::BaseChannel *baseChannel, const QString &contentType, const QString &filename, qulonglong size,
                        uint contentHashType, const QString &contentHash, const QString &description, const QDateTime &date);


    QIODevice* getIODevice(qulonglong offset, Tp::DBusError *error);

private slots:
    void onStateChanged(uint state, uint reason);
    void onQxmppTransferSateChanged(QXmppTransferJob::State state);
    void onTransferError(QXmppTransferJob::Error error);

private:
    Connection *m_connection;
    bool m_outgoingTransfer;
    QBuffer *m_ioDevice;
    QXmppTransferFileInfo m_fileInfo;
    QXmppTransferJob *m_transferJob;
};

#endif // FILETRANSFERCHANNEL_HH
