 /*
 *   interrupts.h
 *
 *  Created on: Apr 18, 2010
 *      Author: anizzomc
 */
#ifndef INTERRUPS_H
#define INTERRUPS_H


void _irq00Handler(void);
void _irq01Handler(void);
void _irq02Handler(void);
void _irq03Handler(void);
void _irq04Handler(void);
void _irq05Handler(void);
void _irq06Handler(void);

/*
void _exception0Handler(void);

*/

void _cli(void);

void _sti(void);

void _hlt(void);

//Termina la ejecución de la cpu.
void haltcpu(void);

#endif /* INTERRUPS_H_ */
