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

#ifndef IOCHANNEL_HH
#define IOCHANNEL_HH

#include <QIODevice>

class IOChannel : public QIODevice
{
    Q_OBJECT
public:
    IOChannel(QObject *parent = 0);
    virtual bool isSequential() const override;
    virtual qint64 size() const override;

private:
    virtual qint64 readData(char *data, qint64 maxSize) override;
    virtual qint64 writeData(const char *data, qint64 maxSize) override;

private:
    QByteArray m_data;
};

#endif // IOCHANNEL_HH