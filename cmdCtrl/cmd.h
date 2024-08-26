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
                            Done,
                            Timeout,

                            Max,

                            /* helpers. */
                            begin = 0,
                            end = Max };

class cmdObj {
    public:
        virtual void send( void ) {};
        virtual void reply( void ) {};
        virtual void timeout( void ) {};
};

class Cmd {

    public:

        //using pCallback = void (Cmd::*)(void);

        Cmd(    CmdType   cmdType      = CmdType::noCmd,
                CmdType   cmdReplyType = CmdType::noCmd,
                PrioLevel cmdPrioLevel = PrioLevel::low,
                uint32_t  cmdRetryNr   = cmdDefaultRetryNr,
                uint32_t  cmdTimeoutMs = cmdDefaultTimeoutMs,
                uint32_t  cmdPeriodMs  = cmdDefaultPeriodMs,
                uint32_t  cmdDelayMs   = cmdDefaultDelayMs   ) :

                type      { cmdType },
                replyType { cmdReplyType },
                priority  { cmdPrioLevel },
                retryNr   { cmdRetryNr },
                timeoutMs { cmdTimeoutMs },
                periodMs  { cmdPeriodMs },
                delayMs   { cmdDelayMs } {}

        Cmd(    cmdObj*   pObject, 
                CmdType   cmdType      = CmdType::noCmd,
                CmdType   cmdReplyType = CmdType::noCmd,
                PrioLevel cmdPrioLevel = PrioLevel::low,
                uint32_t  cmdRetryNr   = cmdDefaultRetryNr,
                uint32_t  cmdTimeoutMs = cmdDefaultTimeoutMs,
                uint32_t  cmdPeriodMs  = cmdDefaultPeriodMs,
                uint32_t  cmdDelayMs   = cmdDefaultDelayMs   ) :
                
                Cmd {   cmdType, 
                        cmdReplyType, 
                        cmdPrioLevel, 
                        cmdRetryNr, 
                        cmdTimeoutMs, cmdPeriodMs, cmdDelayMs  }
                { 
                    if( pObject != nullptr )
                    {
                        pObj = pObject; 
                    }      
                }
        
        void setObj( cmdObj* pObject )
        {
            if( pObject != nullptr )
            {
                pObj = pObject; 
            }     
            else
            {
                pObj = &obj;
            }
        }
        
        void set(   cmdObj*   pObject, 
                    CmdType   cmdType      = CmdType::noCmd,
                    CmdType   cmdReplyType = CmdType::noCmd,
                    PrioLevel cmdPrioLevel = PrioLevel::low,
                    uint32_t  cmdRetryNr   = cmdDefaultRetryNr,
                    uint32_t  cmdTimeoutMs = cmdDefaultTimeoutMs,
                    uint32_t  cmdPeriodMs  = cmdDefaultPeriodMs,
                    uint32_t  cmdDelayMs   = cmdDefaultDelayMs   )
        {
            setObj( pObject );
            type      = cmdType;
            replyType = cmdReplyType;
            priority  = cmdPrioLevel;
            retryNr   = cmdRetryNr;
            timeoutMs = cmdTimeoutMs;
            periodMs  = cmdPeriodMs;
            delayMs   = cmdDelayMs;
        }

        static bool priorityGreaterEqual( const Cmd &lCmd, const Cmd &rCmd ) { return lCmd.priority >= rCmd.priority; } const

        static bool prioritySmallerEqual( const Cmd &lCmd, const Cmd &rCmd ) { return lCmd.priority <= rCmd.priority; } const

        static bool priorityEqual( const Cmd &lCmd, const Cmd &rCmd ) { return lCmd.priority == rCmd.priority; } const
                                
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

        void replied( void ) { isReplied = true; }

        CmdType  type;

        CmdType  replyType;        /* Command type expected as reply. */

        uint32_t  token{ 0 };      /* Each issued command has an unique token assigned to it. Replies must have the same token used by the issued command they refer to. */

    private:

        cmdObj obj;

        cmdObj* pObj { &obj };


        PrioLevel priority;


        uint32_t  retryNr;

        uint32_t  retry;


        uint32_t  timeoutMs;

        uint32_t  timeoutTimerMs;


        uint32_t  periodMs;

        uint32_t  periodTimerMs;


        uint32_t  delayMs;

        uint32_t  delayTimerMs;

        CmdState  state{ CmdState::Idle };

        bool      suspend{ false };     /* Used to suspend (periodic) command execution. */

        bool      isReplied{ false };

        uint32_t  getTimerMs( void ) const;

};

#endif /* CMDCTRL_CMD_H_ */

