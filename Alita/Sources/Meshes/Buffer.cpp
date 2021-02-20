//
// Created by realxie on 2019-10-29.
//

#include "Buffer.h"

NS_RX_BEGIN

Buffer::Buffer(DataBuffer &data, std::uint32_t stride)
    : data_(data), stride_(stride)
{
}

NS_RX_END