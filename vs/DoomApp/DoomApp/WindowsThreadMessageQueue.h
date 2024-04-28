#pragma once
#include "SmartHandle.h"

// WindowsThreadMessageQueue will process all messages for all windows where this window created
class WindowsThreadMessageQueue {
public:
    WindowsThreadMessageQueue();

    void ProcessQueuedMessages();
    void WaitAndProcessQueuedMessages();

private:
    bool queueReceivedQuit = false;
    UniqueEvent wakeUpQueueEvent;
};
