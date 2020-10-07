#include <string.h> // string function definitions
#include <unistd.h> // UNIX standard function definitions
#include <fcntl.h>  // File control definitions
#include <errno.h>  // Error number definitions
#include <inttypes.h>
#include <asm/ioctls.h>
#include <asm/termbits.h>
#include <sys/ioctl.h>

#include "../Header/SBUS.h"

using namespace std;

namespace SBUS
{

    SBUS::SBUS(string tty)
    {
        _tty = tty;

        for (int i = 0; i < _numChannels; i++)
            _useWriteCoeff[i] = false;
    }

    SBUS::~SBUS()
    {
        close(_fd);

        if (_readCoeff)
        {
            for (uint8_t i = 0; i < _numChannels; i++)
            {
                if (_readCoeff[i])
                {
                    delete[] _readCoeff[i];
                }
            }
            delete[] _readCoeff;
        }
        if (_writeCoeff)
        {
            for (uint8_t i = 0; i < _numChannels; i++)
            {
                if (_writeCoeff[i])
                {
                    delete[] _writeCoeff[i];
                }
            }
            delete[] _writeCoeff;
        }
    }

    /* starts the serial communication */
    int SBUS::begin()
    {
        // initialize parsing state
        _parserState = 0;
        // initialize default scale factors and biases
        for (uint8_t i = 0; i < _numChannels; i++)
        {
            setEndPoints(i, _defaultMin, _defaultMax);
        }

// begin the serial port for SBUS
        // open file descriptor
        _fd = open(_tty.c_str(), O_RDWR | O_NOCTTY);

        if (_fd < 0)
        {
            cerr << "Error " << errno << " opening " << _tty << ": " << strerror(errno) << endl;
            return -1;
        }

        // set custom baudrate
        struct termios2 tio;
        int r = ioctl(_fd, TCGETS2, &tio);
        if (r)
        {
            cerr << "TCGETS2" << endl;
            return -1;
        }

        tio.c_cflag &= ~CBAUD;
        tio.c_cflag |= BOTHER;
        tio.c_cflag |= CSTOPB; // 2 stop bits
        tio.c_cflag |= PARENB; // enable parity bit, even by default
        tio.c_ispeed = tio.c_ospeed = _sbusBaud;

        r = ioctl(_fd, TCSETS2, &tio);
        if (r)
        {
            cerr << "TCSETS2" << endl;
            return -1;
        }
        return 0;
    }
    void SBUS::setEndPoints(uint8_t channel, uint16_t min, uint16_t max)
    {
        _sbusMin[channel] = min;
        _sbusMax[channel] = max;
        scaleBias(channel);
    }
    void SBUS::scaleBias(uint8_t channel)
    {
        _sbusScale[channel] = 2.0f / ((float)_sbusMax[channel] - (float)_sbusMin[channel]);
        _sbusBias[channel] = -1.0f * ((float)_sbusMin[channel] + ((float)_sbusMax[channel] - (float)_sbusMin[channel]) / 2.0f) * _sbusScale[channel];
    }
    void SBUS::write(uint16_t *channels)
    {
        static uint8_t packet[25];
        /* assemble the SBUS packet */
        // SBUS header
        packet[0] = _sbusHeader;
        // 16 channels of 11 bit data
        if (channels)
        {
            packet[1] = (uint8_t)((channels[0] & 0x07FF));
            packet[2] = (uint8_t)((channels[0] & 0x07FF) >> 8 | (channels[1] & 0x07FF) << 3);
            packet[3] = (uint8_t)((channels[1] & 0x07FF) >> 5 | (channels[2] & 0x07FF) << 6);
            packet[4] = (uint8_t)((channels[2] & 0x07FF) >> 2);
            packet[5] = (uint8_t)((channels[2] & 0x07FF) >> 10 | (channels[3] & 0x07FF) << 1);
            packet[6] = (uint8_t)((channels[3] & 0x07FF) >> 7 | (channels[4] & 0x07FF) << 4);
            packet[7] = (uint8_t)((channels[4] & 0x07FF) >> 4 | (channels[5] & 0x07FF) << 7);
            packet[8] = (uint8_t)((channels[5] & 0x07FF) >> 1);
            packet[9] = (uint8_t)((channels[5] & 0x07FF) >> 9 | (channels[6] & 0x07FF) << 2);
            packet[10] = (uint8_t)((channels[6] & 0x07FF) >> 6 | (channels[7] & 0x07FF) << 5);
            packet[11] = (uint8_t)((channels[7] & 0x07FF) >> 3);
            packet[12] = (uint8_t)((channels[8] & 0x07FF));
            packet[13] = (uint8_t)((channels[8] & 0x07FF) >> 8 | (channels[9] & 0x07FF) << 3);
            packet[14] = (uint8_t)((channels[9] & 0x07FF) >> 5 | (channels[10] & 0x07FF) << 6);
            packet[15] = (uint8_t)((channels[10] & 0x07FF) >> 2);
            packet[16] = (uint8_t)((channels[10] & 0x07FF) >> 10 | (channels[11] & 0x07FF) << 1);
            packet[17] = (uint8_t)((channels[11] & 0x07FF) >> 7 | (channels[12] & 0x07FF) << 4);
            packet[18] = (uint8_t)((channels[12] & 0x07FF) >> 4 | (channels[13] & 0x07FF) << 7);
            packet[19] = (uint8_t)((channels[13] & 0x07FF) >> 1);
            packet[20] = (uint8_t)((channels[13] & 0x07FF) >> 9 | (channels[14] & 0x07FF) << 2);
            packet[21] = (uint8_t)((channels[14] & 0x07FF) >> 6 | (channels[15] & 0x07FF) << 5);
            packet[22] = (uint8_t)((channels[15] & 0x07FF) >> 3);
        }
        // flags
        packet[23] = 0x00;
        // footer
        packet[24] = _sbusFooter;

        ::write(_fd, packet, sizeof(packet));
    }

} // namespace SBUS