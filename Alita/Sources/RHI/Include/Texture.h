//
// Created by realxie on 2019-10-07.
//

#ifndef ALITA_TEXTURE_H
#define ALITA_TEXTURE_H

#include "Macros.h"
#include "RHIObjectBase.h"
#include "Descriptors.h"

NS_RHI_BEGIN

class TextureView;

class Texture : public RHIObjectBase
{
public:
    virtual ~Texture()
    {
    }
    
    virtual TextureFormat GetFormat() const = 0;
    
    virtual TextureView* CreateView() = 0;
};

NS_RHI_END

#endif //ALITA_TEXTURE_H
