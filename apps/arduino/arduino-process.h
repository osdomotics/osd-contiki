/*
 * Copyright (C) 2014-2018, Ralf Schlatterbeck Open Source Consulting
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
 * \devgroup Arduino Process
 *
 * This wraps the Arduino-API entry points `loop` and `setup` in a
 * contiki process.
 *
 * If the normal contiki includes are used and resources initialized in
 * `setup`, Contiki resources can be used in an arduino sketch.
 *
 * @{
 */

/**
 * \file
 *        Wrapper for Arduino sketches
 * \author
 *        Ralf Schlatterbeck <rsc@runtux.com>
 *
 */

#include "contiki.h"

/*--------------- enable sleep mode ---------------------------------------*/
void mcu_sleep_enable(void);
/*--------------- disable sleep mode ---------------------------------------*/
void mcu_sleep_disable(void);
/*--------------- sleep mode on---------------------------------------*/
void mcu_sleep_on(void);
/*--------------- sleep mode off---------------------------------------*/
void mcu_sleep_off(void);
/*---------------- set sleep value ------------------------------------*/
void mcu_sleep_set(uint8_t value);
/*---------------- loop periodic set value ------------------------------------*/
void loop_periodic_set(clock_time_t value);

extern void loop         (void);
extern void setup        (void);
extern void arduino_init (void);
extern void button       (void);

extern struct process arduino_sketch;

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */

/** @} */
