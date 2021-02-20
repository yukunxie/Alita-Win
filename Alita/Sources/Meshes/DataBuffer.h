//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_DATABUFFER_H
#define ALITA_DATABUFFER_H

#include "../Base/Macros.h"

#include <cstdint>

NS_RX_BEGIN

/*
 * data buffer with uint8_t;
 * */
struct DataBuffer
{
    std::uint8_t* data = nullptr;
    std::uint32_t capacity = 0;
};

NS_RX_END


#endif //ALITA_DATABUFFER_H
