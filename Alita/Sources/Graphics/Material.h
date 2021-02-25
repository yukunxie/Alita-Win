//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_MATERIAL_H
#define ALITA_MATERIAL_H

#include "Base/ObjectBase.h"
#include "Base/FileSystem.h"
#include "Effect.h"

#include "rapidjson/document.h"

#include <list>

NS_RX_BEGIN

class Material : public ObjectBase
{
public:
    Material(const std::string& configFilename = "");

    virtual void Build() {}

protected:
    RHI::BindGroupLayout* CreateBindGroupLayout(const rapidjson::Document& doc);

protected:
    Effect* effect_ = nullptr;

    // RHI
    RHI::BindGroupLayout* rhiBindGroupLayout_ = nullptr;
};

NS_RX_END


#endif //ALITA_MATERIAL_H
