/*
 * pertinence.h
 *
 *  Created on: 6 avr. 2017
 *      Author: robin
 */

#ifndef PERTINENCE_H_
#define PERTINENCE_H_
#include <stdlib.h>
#include <math.h>
#include "structures.h"
#include "utils.h"

void calculPertinence(Element** index, uint8_t* emptyLines, double* nabla, double* delta, int n);

#endif /* PERTINENCE_H_ */
