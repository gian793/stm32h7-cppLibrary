/**
 * \file            cmd.h
 * \brief           Command smallest item.
 * \author          giancarlo.marcolin@gmail.com
 */

#ifndef CMDCTRL_CMD_H_
#define CMDCTRL_CMD_H_

#include <type_traits>

/*---------------------------------------------------------------------------*/

enum class CmdState: int {  Idle = 0,
                            Sent,
                            WaitForReply,
                            Done,
                            Timeout,

                            Max,

                            /* helpers. */
                            begin = Idle,
                            end = Max };

/*---------------------------------------------------------------------------*/

enum class CmdOption: int { None            = 0,
                            RepeatOnReply   = 1,
                            RepeatOnTimeout = 2,
                            RepeatForever   = 4,   

                            Max,

                            /* helpers. */
                            begin = 0,
                            end = Max };

inline bool operator &(CmdOption lhs, CmdOption rhs) {
    return static_cast<bool>( static_cast<std::underlying_type_t<CmdOption>>(lhs) & static_cast<std::underlying_type_t<CmdOption>>(rhs) );
}

inline bool operator |(CmdOption lhs, CmdOption rhs) {
    return static_cast<bool>( static_cast<std::underlying_type_t<CmdOption>>(lhs) | static_cast<std::underlying_type_t<CmdOption>>(rhs) );
}

inline CmdOption operator +(CmdOption lhs, CmdOption rhs) {
    return static_cast<CmdOption>( static_cast<std::underlying_type_t<CmdOption>>(lhs) + static_cast<std::underlying_type_t<CmdOption>>(rhs) );
}

/*---------------------------------------------------------------------------*/

class CmdObj {
    public:
        virtual void send( void ) {};
        virtual void reply( void ) {};
        virtual void timeout( void ) {};
};

/*---------------------------------------------------------------------------*/

class Cmd {

    public:

        //using pCallback = void (Cmd::*)(void);

        explicit Cmd(   CmdType   cmdType      = CmdType::noCmd,
                        CmdType   cmdReplyType = CmdType::noCmd,
                        PrioLevel cmdPrioLevel = PrioLevel::low,
                        uint32_t  cmdRetryNr   = cmdDefaultRetryNr,
                        uint32_t  cmdTimeoutMs = cmdDefaultTimeoutMs,
                        uint32_t  cmdPeriodMs  = cmdDefaultPeriodMs,
                        uint32_t  cmdDelayMs   = cmdDefaultDelayMs,
                        CmdOption cmdOptions   = CmdOption::None   ) :

                        type      { cmdType },
                        replyType { cmdReplyType },
                        priority  { cmdPrioLevel },
                        retryNr   { cmdRetryNr },
                        timeoutMs { cmdTimeoutMs },
                        periodMs  { cmdPeriodMs },
                        delayMs   { cmdDelayMs },
                        options   { cmdOptions } {}

        explicit Cmd(   CmdObj*   pObject, 
                        CmdType   cmdType      = CmdType::noCmd,
                        CmdType   cmdReplyType = CmdType::noCmd,
                        PrioLevel cmdPrioLevel = PrioLevel::low,
                        uint32_t  cmdRetryNr   = cmdDefaultRetryNr,
                        uint32_t  cmdTimeoutMs = cmdDefaultTimeoutMs,
                        uint32_t  cmdPeriodMs  = cmdDefaultPeriodMs,
                        uint32_t  cmdDelayMs   = cmdDefaultDelayMs,
                        CmdOption cmdOptions   = CmdOption::None   ) :
                
                        Cmd {   cmdType, 
                                cmdReplyType, 
                                cmdPrioLevel, 
                                cmdRetryNr, 
                                cmdTimeoutMs, 
                                cmdPeriodMs, 
                                cmdDelayMs,
                                cmdOptions  }
                        { 
                            setObj( pObject );
                        }
        
        void setObj( CmdObj* pObject )
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
        
        /* Initialization of state machine: state and timers init. */
        void init( uint32_t newToken )  {   token = newToken; 
                                            state = CmdState::Idle; 
                                            isReplied = false;
                                            retryCnt  = 0;
                                            timeoutTimerMs = getTimerMs(); 
                                            delayTimerMs   = getTimerMs();
                                            options        = CmdOption::None;    }

        void init( uint32_t newToken, CmdObj* pObject )  {  init( newToken );
                                                            setObj( pObject );  }
        
        void set(   CmdType   cmdType,
                    CmdObj*   pObject, 
                    PrioLevel cmdPrioLevel = PrioLevel::low,
                    CmdType   cmdReplyType = CmdType::noCmd,
                    uint32_t  cmdToken     = 0,

                    uint32_t  cmdRetryNr   = cmdDefaultRetryNr,
                    uint32_t  cmdTimeoutMs = cmdDefaultTimeoutMs,
                    uint32_t  cmdPeriodMs  = cmdDefaultPeriodMs,
                    uint32_t  cmdDelayMs   = cmdDefaultDelayMs,
                    CmdOption cmdOptions   = CmdOption::None     )
        {
            init( cmdToken, pObject );

            type      = cmdType;
            replyType = cmdReplyType;
            priority  = cmdPrioLevel;
            retryNr   = cmdRetryNr;
            timeoutMs = cmdTimeoutMs;
            periodMs  = cmdPeriodMs;
            delayMs   = cmdDelayMs;
            options   = cmdOptions;
        }

        // static bool priorityGreaterEqual( const Cmd &lCmd, const Cmd &rCmd ) { return lCmd.priority >= rCmd.priority; } const

        // static bool prioritySmallerEqual( const Cmd &lCmd, const Cmd &rCmd ) { return lCmd.priority <= rCmd.priority; } const

        // static bool priorityEqual( const Cmd &lCmd, const Cmd &rCmd ) { return lCmd.priority == rCmd.priority; } const                            
                                            
        void reset( void ) { type = CmdType::noCmd; isReplied = false; }

        CmdState execute( void );

        void replied( void ) { isReplied = ( replyType != CmdType::noCmd ); } /* Flag can be set only if a reply is expected. */

        CmdOption getOptions( void ) { return options; }

        const uint32_t getPeriod( void ) { return periodMs; };

        const bool isRetryDone( void ) { return ( retryCnt >= retryNr ); };

        CmdType  type;

        CmdType  replyType;         /* Command type expected as reply. */

        PrioLevel priority;

        uint32_t token{ 0 };        /* Each issued command has an unique token assigned. Replies to a specific command must have the same token. */

    private:

        CmdObj obj;

        CmdObj* pObj { &obj };


        uint32_t  retryNr;

        uint32_t  retryCnt;


        uint32_t  timeoutMs;

        uint32_t  timeoutTimerMs;

         
        uint32_t  periodMs;

        //uint32_t  periodTimerMs;


        uint32_t  delayMs;

        uint32_t  delayTimerMs;         /* In case of peiodic command only the first time the delay is applied. */
        

        CmdState  state{ CmdState::Idle };

        bool      suspend{ false };     /* Used to suspend (periodic) command execution. */

        bool      isReply{ false };

        bool      isReplied{ false };

        CmdOption options{ CmdOption::None };

        uint32_t  getTimerMs( void ) const;
};

#endif /* CMDCTRL_CMD_H_ */

