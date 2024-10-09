/**
 * \file            cmdConfig.h
 * \brief           Commands configuration file.
 * \author          giancarlo.marcolin@gmail.com
 */

#ifndef CMDCTRL_CMDCONFIG_H_
#define CMDCTRL_CMDCONFIG_H_

#include <cstdint>

constexpr uint32_t cmdBufferSize = 16;             /* Channel size. */

constexpr uint32_t cmdDefaultTimeoutMs = 50;

constexpr uint32_t cmdDefaultPeriodMs = 0;

constexpr uint32_t cmdDefaultDelayMs = 0;

constexpr uint32_t cmdDefaultRetryNr = 3;


enum class CmdType : int {  noCmd,
                            cmd1,
                            cmd2,
                            cmd3,
                            cmd4,
                            cmd5,
                            cmd6,
                            Max,

                            /* helpers. */
                            begin = 0,
                            end = Max };


enum class PrioLevel : int {    l0,
                                l1,
                                l2,
                                l3,
                                l4,
                                l5,
                                Max,

                                /* helpers. */
                                invalid = -1,
                                low = 0,
                                high = Max - 1,
                                medium = (low+high)/2,

                                begin = 0,
                                end = Max };


#endif /* CMDCTRL_CMDCONFIG_H_ */
