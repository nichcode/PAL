
#ifndef PAL_EVENT_QUEUE_H
#define PAL_EVENT_QUEUE_H

#include "PAL/PAL_events.h"
#include "PAL_utils.h"

#include <deque>
#include <queue>

class PAL_EventQueue
{
public:
    PAL_BOOL poll(PAL_Event* event)
    {
        if (m_Queue.empty()) { return PAL_FALSE; }
        *event = m_Queue.front();
        m_Queue.pop();
        return PAL_TRUE;
    }

    void push(PAL_Event& event) 
    {
        m_Queue.push(event);
    }

    bool empty() 
    {
        return m_Queue.empty();
    }

private:
    std::queue<PAL_Event> m_Queue;
};

static PAL_EventQueue s_EventQueue;

#endif // PAL_EVENT_QUEUE_H