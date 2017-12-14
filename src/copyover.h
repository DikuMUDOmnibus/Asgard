/*
 * copyover.h
 *
 *  Created on: 9 Feb 2011
 *      Author: Nico
 *  Global declarations for copyover.c
 */

#ifndef COPYOVER_H_
#define COPYOVER_H_

void setup_from_args(int argc, char **argv);
void recover_hotboot_state();
void setup_signal_handlers();

extern int port;
extern bool hotboot;
extern int control;

#endif /* COPYOVER_H_ */
