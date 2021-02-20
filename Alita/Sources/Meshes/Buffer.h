//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_BUFFER_H
#define ALITA_BUFFER_H

#include "DataBuffer.h"

NS_RX_BEGIN

class Buffer
{
public:
    Buffer(DataBuffer& data, std::uint32_t stride);

protected:
    DataBuffer data_;
    std::uint32_t stride_ = 0;
};

NS_RX_END

#endif //ALITA_BUFFER_H
