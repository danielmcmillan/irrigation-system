#ifndef _CONTROL_PROCESSOR_STATE_H
#define _CONTROL_PROCESSOR_STATE_H

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
