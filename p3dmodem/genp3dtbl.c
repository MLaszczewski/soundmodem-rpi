/*****************************************************************************/

/*
 *      genp3dtbl.c  --  Generate tables for the AO-40 P3D PSK modem.
 *
 *      Copyright (C) 1999-2000  Thomas Sailer (sailer@ife.ee.ethz.ch)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Please note that the GPL allows you to use the driver, NOT the radio.
 *  In order to use the radio, you need a license from the communications
 *  authority of your country.
 *
 */

/*****************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "modem.h"
#include "p3d.h"
#include "mat.h"

#include <math.h>
#include <float.h>
#include <stdio.h>

/* ---------------------------------------------------------------------- */

extern inline unsigned int hweight32(unsigned int w)
{
        unsigned int res = (w & 0x55555555) + ((w >> 1) & 0x55555555);
        res = (res & 0x33333333) + ((res >> 2) & 0x33333333);
        res = (res & 0x0F0F0F0F) + ((res >> 4) & 0x0F0F0F0F);
        res = (res & 0x00FF00FF) + ((res >> 8) & 0x00FF00FF);
        return (res & 0x0000FFFF) + ((res >> 16) & 0x0000FFFF);
}

/* ---------------------------------------------------------------------- */

#define COSBITS 9

static void gencos(FILE *f)
{
	unsigned int i;

	fprintf(f, "static const int16_t costab[%u] = {", (1<<(COSBITS)));
	for (i = 0; ; i++) {
		if (!(i & 7))
			fprintf(f, "\n\t");
		fprintf(f, "%6d", (int)(32767*cos(i * (2.0 * M_PI / (1<<(COSBITS))))));
		if (i >= (1<<(COSBITS))-1)
			break;
		fprintf(f, ", ");
	}
	fprintf(f, "\n};\n\n#define COS(x) (costab[(((x)>>%u)&0x%x)])\n#define SIN(x) COS((x)+0xc000)\n\n",
		16-COSBITS, (1<<(COSBITS))-1);
}

/* ---------------------------------------------------------------------- */

static void gencrc(FILE *f)
{
	unsigned int i, j, k;

	fprintf(f, "static const u_int16_t amsat_crc[256] = {");
	for (i = 0;; i++) {
		j = 0;
		for (k = 0; k < 8; k++) {
			j = (j << 1) | (((j >> 15) ^ (i >> (7-k))) & 1);
			if (j & 1)
				j ^= (1 << 5) | (1 << 12);
		}
		if (!(i & 15))
			fprintf(f, "\n\t");
		fprintf(f, "0x%04x", j & 0xffff);
		if (i >= 255)
			break;
		fprintf(f, ", ");
	}
	fprintf(f, "\n};\n\n");
}

/* ---------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
	printf("/*\n * automatically generated by %s, do not edit!\n */\n\n"
	       "#ifdef HAVE_CONFIG_H\n#include \"config.h\"\n#endif\n\n#include \"modem.h\"\n\n", argv[0]);
	gencos(stdout);
	gencrc(stdout);
	return 0;
}
