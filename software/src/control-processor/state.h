#ifndef _CONTROL_PROCESSOR_STATE_H
#define _CONTROL_PROCESSOR_STATE_H

/**
0 Unconfigured
1 Initializing
2 Ready

              init: ->0
0 && begin command: ->1
1 && begin success: ->2
     reset command: ->0
*/

enum class ControlProcessorStatus
{
    Unconfigured,
    Initializing,
    Ready
};

struct ControlProcessorState
{
    ControlProcessorStatus status = ControlProcessorStatus::Unconfigured;
};

#endif
