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

//template <typename T>
class cmdObj {
public:
    virtual void send( void ) {};
    virtual void reply( void ) {};
    virtual void timeout( void ) {};
};

class Cmd {

public:

    using pCallback = void (Cmd::*)(void);

    Cmd(    CmdType   cmdType      = CmdType::noCmd,
            CmdType   cmdReplyType = CmdType::noCmd,
            PrioLevel cmdPrioLevel = PrioLevel::low,
            uint32_t  cmdRetryNr   = cmdDefaultRetryNr,
            uint32_t  cmdTimeoutMs = cmdDefaultTimeoutMs,
            uint32_t  cmdPeriodMs  = cmdDefaultPeriodMs,
            uint32_t  cmdDelayMs   = cmdDefaultDelayMs,
            pCallback send         = nullptr,
            pCallback reply        = nullptr,
            pCallback timeout      = nullptr,
            cmdObj*   pObject      = nullptr    ) :

            type      { cmdType },
            replyType { cmdReplyType },
            priority  { cmdPrioLevel },
            retryNr   { cmdRetryNr },
            timeoutMs { cmdTimeoutMs },
            periodMs  { cmdPeriodMs },
            delayMs   { cmdDelayMs },
            send_Cb   { send },
            reply_Cb  { reply },
            timeout_Cb{ timeout },
            pObj      { pObject }  {}

    static bool priorityGreaterEqual( const Cmd &lCmd, const Cmd &rCmd ) { return lCmd.priority >= rCmd.priority; } const

    static bool prioritySmallerEqual( const Cmd &lCmd, const Cmd &rCmd ) { return lCmd.priority <= rCmd.priority; } const

    static bool priorityEqual( const Cmd &lCmd, const Cmd &rCmd ) { return lCmd.priority == rCmd.priority; } const

    uint32_t getToken( void ) { return token; } const

    void setToken( uint32_t tokenValue ) { token = tokenValue; }

    void setCallbacks(  pCallback xCmdSend_Cb = nullptr,
                        pCallback xReply_Cb    = nullptr,
                        pCallback xTimeout_Cb  = nullptr )
                    {   
                        send_Cb    = xCmdSend_Cb;
                        reply_Cb   = xReply_Cb;
                        timeout_Cb = xTimeout_Cb;
                    }
                            
    void init( uint32_t newToken )  {   token = newToken; 
                                        state = CmdState::Idle; 
                                        retry = 0; 
                                        timeoutTimerMs = getTimerMs(); 
                                        periodTimerMs  = getTimerMs(); 
                                        delayTimerMs   = getTimerMs();  }

    void init( uint32_t newToken, cmdObj* pObject )  {  init( newToken );
                                                        pObj = pObject;  }
                                        
    void reset( void ) { type = CmdType::noCmd; }

    CmdState execute( void );

    CmdType  type;

    CmdType  replyType;        /* Command type expected as reply. */

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


    pCallback send_Cb;

    pCallback reply_Cb;

    pCallback timeout_Cb;

    static cmdObj obj;

    cmdObj* pObj { &obj };

    CmdState  state{ CmdState::Idle };

    uint32_t  token{ 0 };           /* Each issued command has an unique token assigned to it. Replies must have the same token used by the command request issued. */

    bool      suspend{ false };     /* Used to suspend (periodic) command execution. */

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

