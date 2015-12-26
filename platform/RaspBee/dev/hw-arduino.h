/*
 * Copyright (c) 2014, Ralf Schlatterbeck Open Source Consulting
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \defgroup compatibility Arduino - Contiki
 *
 * This defines contiki-compatible hardware definitions for running
 * arduino sketches (or just to call arduino-compatible function). 
 * For now only for osd hardware, a similar file should exist for each
 * arduino-compatible hardware.
 *
 * @{
 */

/**
 * \file
 *        Header file for arduino compatibility
 * \author
 *        Ralf Schlatterbeck <rsc@runtux.com>
 *
 */

#ifdef __cplusplus
extern "C"{
#endif

#include "contiki.h"

/*
 * The OSD hardware only supports timer 3 for PWM, timer 2 is used by
 * contiki for sleep/wakeup timing and is not usable for PWM.
 */
#define digitalPinToTimer(pin)                \
    ( (pin) == 2                              \
    ? TIMER3A                                 \
    : ( (pin) == 3                            \
      ? TIMER3B                               \
      : ((pin == 4) ? TIMER3C : NOT_ON_TIMER) \
      )                                       \
    )

/* Only init timer 3 with phase correct pwm 8-bit and prescaler 64 */
#define arduino_pwm_timer_init() \
  (hwtimer_ini (3, HWT_WGM_PWM_PHASE_8_BIT, HWT_CLOCK_PRESCALER_64, 0))

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */

#ifdef __cplusplus
} // extern "C"
#endif

/** @} */
