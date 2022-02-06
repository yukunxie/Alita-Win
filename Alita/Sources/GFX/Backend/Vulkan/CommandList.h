//
// Created by realxie on 2020-03-01.
//

#ifndef PROJ_ANDROID_COMMANDLIST_H
#define PROJ_ANDROID_COMMANDLIST_H

#include "DeferredRenderCommands.h"

NS_GFX_BEGIN

class VKCommandBuffer;

class CommandListIterator;

class CommandList
{
protected:
    struct DeferredCmdWrapper
    {
        DeferredCmdWrapper* next = nullptr;
        DeferredCmdBase* cmd = nullptr;
    };

public:
    CommandList()
        : byteBuffers_(1024 * 256, 0)
    {}
    
    ~CommandList()
    {
        Reset();
    }
    
    template<typename CommandName, typename ...ArgTypes>
    void RecordCommand(ArgTypes ... args)
    {
        if (offset_ + sizeof(CommandName) + sizeof(DeferredCmdWrapper) + 8 > byteBuffers_.size())
        {
            byteBuffers_.resize(
                byteBuffers_.size() * 2 + sizeof(CommandName) + sizeof(DeferredCmdWrapper));
        }
        
        auto commandListNode = new(byteBuffers_.data() + offset_) DeferredCmdWrapper();
        offset_ += sizeof(DeferredCmdWrapper);
        AlignOffset();
        
        commandListNode->cmd = new(byteBuffers_.data() + offset_) CommandName(args...);
        offset_ += sizeof(CommandName);
        AlignOffset();
        
        if (lastCommandNode_ == nullptr)
        {
            lastCommandNode_ = firstCommandNode_ = commandListNode;
        }
        else
        {
            lastCommandNode_->next = commandListNode;
            lastCommandNode_ = commandListNode;
        }
    
        cmdCount_++;
    }

    void AddCmd(std::function<void(const CommandBufferPtr&)>&& func)
    {
        gfxCmds_.push_back(std::move(func));
    }

    void AddCmd(const std::function<void(const CommandBufferPtr&)>& func)
    {
        gfxCmds_.push_back(func);
    }
    
    void Reset();
    
    std::uint32_t GetCmdCount()
    {return cmdCount_;}

protected:
    
    void AlignOffset()
    {
        // 8 bits alignment
        offset_ = (offset_ + 8 - 1) & (~(8 - 1));
    }

protected:
    std::uint32_t cmdCount_ = 0;
    std::uint32_t offset_ = 0;
    std::vector<std::uint8_t> byteBuffers_;
    
    DeferredCmdWrapper* firstCommandNode_ = nullptr;
    DeferredCmdWrapper* lastCommandNode_ = nullptr;

    std::vector<std::function<void(const CommandBufferPtr&)>> gfxCmds_;
    
    friend class CommandListIterator;
};

// TODO realxie
class CommandListIterator
{
public:
    explicit CommandListIterator(const CommandList* commandList)
    {
        pWrapperHeader_ = commandList->firstCommandNode_;
    }
    
    FORCE_INLINE bool HasCommandLeft()
    {
        return nullptr != pWrapperHeader_;
    }
    
    FORCE_INLINE DeferredCmdBase* NextCommand()
    {
        auto cmd = pWrapperHeader_->cmd;
        pWrapperHeader_ = pWrapperHeader_->next;
        return cmd;
    }

protected:
    CommandList::DeferredCmdWrapper* pWrapperHeader_ = nullptr;
};

NS_GFX_END

#endif //PROJ_ANDROID_COMMANDLIST_H
