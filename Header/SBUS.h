#ifndef SBUS_h
#define SBUS_h

#include <iostream>

namespace SBUS
{

    class SBUS
    {
    public:
        SBUS(std::string tty);
        ~SBUS();
        int begin();
        void setEndPoints(uint8_t channel,uint16_t min,uint16_t max);
        void write(uint16_t* channels);

    private:
        std::string _tty;
        int _fd;
        const uint32_t _sbusBaud = 100000;
        static const uint8_t _numChannels = 16;
        bool _useWriteCoeff[_numChannels];
        float **_readCoeff, **_writeCoeff;
        uint8_t _parserState;
        const uint16_t _defaultMin = 193; // ?
        const uint16_t _defaultMax = 1794; // ?
        uint16_t _sbusMin[_numChannels];
        uint16_t _sbusMax[_numChannels];
        float _sbusScale[_numChannels];
        float _sbusBias[_numChannels];
        void scaleBias(uint8_t channel);
        const uint8_t _sbusHeader = 0x0F;
        const uint8_t _sbusFooter = 0x00;
        const uint8_t _sbus2Footer = 0x04;
        const uint8_t _sbus2Mask = 0x0F;
    };
} // namespace SBUS

#endif