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

#include "iochannel.hh"

IOChannel::IOChannel(QObject *parent) : QIODevice(parent)
{
}

qint64 IOChannel::size() const
{
    return QIODevice::size() + m_data.size();
}

bool IOChannel::isSequential() const
{
    return true;
}

qint64 IOChannel::readData(char *data, qint64 maxSize)
{
    auto size = qMin<qint64>(m_data.size(), maxSize);
    memcpy(data, m_data.constData(), size);
    m_data.remove(0, size);
    return size;
}

qint64 IOChannel::writeData(const char *data, qint64 maxSize)
{
    m_data.append(data, maxSize);
    emit bytesWritten(maxSize);
    emit readyRead();
    return maxSize;
}
