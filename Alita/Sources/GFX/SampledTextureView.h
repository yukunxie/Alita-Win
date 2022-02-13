//
// Created by realxie on 2019-12-31.
//

#ifndef RHI_SAMPLEDTEXTUREVIEW_H
#define RHI_SAMPLEDTEXTUREVIEW_H

#include "Macros.h"
#include "BindingResource.h"
#include "Descriptors.h"
#include "Sampler.h"
#include "TextureView.h"

NS_GFX_BEGIN

// class SampledTextureView : public BindingResource
// {
// public:
//     SampledTextureView() = delete;
//
//     SampledTextureView(Sampler* sampler, TextureView* imageView)
//         : BindingResource(sampler->GetGPUDevice(), RHIObjectType::SampledTextureView), sampler_(sampler),
//           imageView_(imageView)
//     {
//     }
//
//
//     virtual void Dispose() override
//     {
//         sampler_ = nullptr;
//         imageView_ = nullptr;
//     }
//
//     virtual ~SampledTextureView()
//     {
//         Dispose();
//     }
//
//     Sampler* GetSampler()
//     { return sampler_; }
//
//     TextureView* GetTextureView()
//     { return imageView_; }
//
// protected:
//     Sampler* sampler_ = nullptr;
//     TextureView* imageView_ = nullptr;
// };

NS_GFX_END

#endif //RHI_SAMPLEDTEXTUREVIEW_H
