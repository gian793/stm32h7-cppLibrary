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
            done_Cb   { done },
            reply_Cb  { reply },
            timeout_Cb{ timeout }  {}

    static bool priorityGreaterEqual( const Cmd &lCmd, const Cmd &rCmd ) { return lCmd.priority >= rCmd.priority; } const

    static bool prioritySmallerEqual( const Cmd &lCmd, const Cmd &rCmd ) { return lCmd.priority <= rCmd.priority; } const

    static bool priorityEqual( const Cmd &lCmd, const Cmd &rCmd ) { return lCmd.priority == rCmd.priority; } const

    uint32_t getToken( void ) { return token; } const

    void setToken( uint32_t tokenValue ) { token = tokenValue; }

    void setCallbacks(  pCallback xCmdDone_Cb = nullptr,
                        pCallback xReply_Cb   = nullptr,
                        pCallback xTimeout_Cb = nullptr )
                    {   
                        done_Cb    = xCmdDone_Cb;
                        reply_Cb   = xReply_Cb;
                        timeout_Cb = xTimeout_Cb;
                    }
                            
    void init( uint32_t newToken )  { state = CmdState::Idle; retry = 0; timeoutTimerMs = getTimerMs(); periodTimerMs = getTimerMs(); delayTimerMs = getTimerMs(); token = newToken; }
        
    void reset( void ) { type = CmdType::noCmd; }

    CmdState execute( void );

    CmdType  type;

    CmdType  replyType;        /* Which type of command is expected as reply. */

private:

    PrioLevel priority;


    uint32_t  retryNr;

    uint32_t  retry;


    uint32_t  timeoutMs;

    uint32_t  timeoutTimerMs;


    uint32_t  periodMs;

    uint32_t  periodTimerMs;


    uint32_t  delayMs;

    uint32_t  delayTimerMs;


    pCallback done_Cb;

    pCallback reply_Cb;

    pCallback timeout_Cb;


    CmdState  state{CmdState::Idle};

    uint32_t  token{0};        /* Each issued command has an unique token assigned to it. Replies must have the same token used by the command request issued. */

    bool      suspend{false};  /* Used to suspend (periodic) command execution. */

    uint32_t  getTimerMs( void ) const;

    /*
    CmdType   type{CmdType::noCmd};

    CmdType   replyType{CmdType::noCmd};      
    

    PrioLevel priority{PrioLevel::low};

    uint32_t  retryNr{cmdDefaultRetryNr};

    uint32_t  timeoutMs{cmdDefaultTimeoutMs};

    uint32_t  periodMs{cmdDefaultPeriodMs};

    uint32_t  delayMs{cmdDefaultDelayMs};


    pCallback done_Cb{nullptr};

    pCallback reply_Cb{nullptr};

    pCallback timeout_Cb{nullptr};


    CmdState  state{CmdState::Idle};

    uint32_t  token{0};        

    bool suspend{false};       */

};

#endif /* CMDCTRL_CMD_H_ */

