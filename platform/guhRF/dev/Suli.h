/*
 * Copyright (c) 2015, Ralf Schlatterbeck Open Source Consulting
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
 * \defgroup Suli compatibility
 *
 * Seeed Unified Library Interface for Contiki OS
 * See also https://github.com/Seeed-Studio/Suli
 *
 * Note: For now this only implements what we need to the LED_Strip example
 * working. We're re-using existing Arduino wrappers where possible.
 *
 * @{
 */

/**
 * \file
 *        Header file for Seeed compatibility
 * \author
 *        Ralf Schlatterbeck <rsc@runtux.com>
 */

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

#define suli_delay_ms(ms) clock_delay_msec(ms)
#define suli_delay_us(us) clock_delay_usec(us)

/* These are used by Suli */
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef int16_t  int16;
typedef int32_t  int32;

typedef int IO_T;    // IO type
typedef int PIN_T;   // pin name
typedef int DIR_T;   // pin direction

/* From Arduino.h */
#define HAL_PIN_INPUT  INPUT
#define HAL_PIN_OUTPUT OUTPUT
#define HAL_PIN_HIGH   HIGH
#define HAL_PIN_LOW    LOW

static inline void suli_pin_init (IO_T *pio, PIN_T pin)
{
    *pio = pin;
}

static inline void suli_pin_dir (IO_T *pio, DIR_T dir)
{
    pinMode (*pio, dir);
}

static inline void suli_pin_write (IO_T *pio, int16 state)
{
    digitalWrite (*pio, state);
}

static inline int16 suli_pin_read (IO_T *pio)
{
    return digitalRead (*pio);
}

#ifdef __cplusplus
} // extern "C"
#endif

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */

/** @} */
