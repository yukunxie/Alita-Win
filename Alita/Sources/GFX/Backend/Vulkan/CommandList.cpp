//
// Created by realxie on 2020-03-01.
//

#include "CommandList.h"

NS_GFX_BEGIN

void CommandList::Reset()
{
    // // No need to call dtor of deferred commands,
    // // So, we must assure no any command may alloc memory from heap.
    // CommandListIterator iterator(this);
    // while (iterator.HasCommandLeft())
    // {
    //     auto cmd = iterator.NextCommand();
    //     cmd->~DeferredCmdBase();
    // }
    
    cmdCount_ = 0;
    offset_ = 0;
    firstCommandNode_ = nullptr;
    lastCommandNode_ = nullptr;

    gfxCmds_.clear();
}

NS_GFX_END
