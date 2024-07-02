/**
 * \file            cmd.h
 * \brief           Command smallest item.
 * \author          giancarlo.marcolin@gmail.com
 */

#ifndef CMDCTRL_CMD_H_
#define CMDCTRL_CMD_H_

enum class CmdState : int { Idle,
                            Sent,
                            WaitForReply,
                            Timeout,

                            Max,

                            /* helpers. */
                            begin = 0,
                            end = Max };

class Cmd {

public:

    using pCallback = void (Cmd::*)(uint8_t*);

    Cmd(    CmdType   cmdType      = CmdType::noCmd,
            CmdType   cmdReplyType = CmdType::noCmd,
            PrioLevel cmdPrioLevel = PrioLevel::low,
            uint32_t  cmdRetryNr   = cmdDefaultRetryNr,
            uint32_t  cmdTimeoutMs = cmdDefaultTimeoutMs,
            uint32_t  cmdPeriodMs  = cmdDefaultPeriodMs,
            uint32_t  cmdDelayMs   = cmdDefaultDelayMs,
            pCallback done         = nullptr,
            pCallback reply        = nullptr,
            pCallback timeout      = nullptr    ) :

            type      { cmdType },
            replyType { cmdReplyType },
            priority  { cmdPrioLevel },
            retryNr   { cmdRetryNr },
            timeoutMs { cmdTimeoutMs },
            periodMs  { cmdPeriodMs },
            delayMs   { cmdDelayMs },
            token     { ++nextToken },
            done_Cb   { done },
            reply_Cb  { reply },
            timeout_Cb{ timeout }  {}

    Cmd& operator=(const Cmd& other)
    {
        if( this != &other )
        {
//            Cmd tmp( other ); /* RAII. */
//            tmp.swap(*this);
        }

        return *this;
    }
    
//    void swap( Cmd& s) noexcept
//    {
//        std::swap(this, s);
//    }

    void SetCallbacks(  pCallback xCmdDone_Cb = nullptr,
                        pCallback xReply_Cb   = nullptr,
                        pCallback xTimeout_Cb = nullptr )
                    {   done_Cb    = xCmdDone_Cb;
                        reply_Cb   = xReply_Cb;
                        timeout_Cb = xTimeout_Cb;
                    }


private:

    CmdType   type;

    CmdType   replyType;      /* Which type of command is expected as reply. */


    PrioLevel priority;

    uint32_t  retryNr;

    uint32_t  timeoutMs;

    uint32_t  periodMs;

    uint32_t  delayMs;


    pCallback done_Cb;

    pCallback reply_Cb;

    pCallback timeout_Cb;


    CmdState  state = CmdState::Idle;

    uint32_t nextToken = 0;

    uint32_t  token = 0;    /* Each issued command has an unique token assigned to it. It is replicated in the reply as well. */

    bool suspend = false;   /* Used to suspend (periodic) command execution. */

};

#endif /* CMDCTRL_CMD_H_ */

