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

NS_RHI_BEGIN

// class SampledTextureView : public BindingResource
// {
// public:
//     SampledTextureView() = delete;
//
//     SampledTextureView(Sampler* sampler, TextureView* imageView)
//         : BindingResource(sampler->GetGPUDevice(), RHIObjectType::SampledTextureView), sampler_(sampler),
//           imageView_(imageView)
//     {
//         RHI_SAFE_RETAIN(sampler_);
//         RHI_SAFE_RETAIN(imageView_);
//     }
//
//
//     virtual void Dispose() override
//     {
//         RHI_SAFE_RELEASE(sampler_);
//         RHI_SAFE_RELEASE(imageView_);
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

NS_RHI_END

#endif //RHI_SAMPLEDTEXTUREVIEW_H
