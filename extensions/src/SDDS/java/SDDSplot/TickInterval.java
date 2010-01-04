/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

package SDDS.java.SDDSplot;

public class TickInterval {
    boolean do_exponential = false;
    int n_ticks = 9;
    double[] interval = {.2, .5, 1.0, 2.0};
    double delta;
    double start;
    int number;

    public TickInterval(double lower, double upper) {
	int expon, i;
	double mantissa;
	if (Math.abs(upper)>Math.abs(lower))
	  expon = (int)(Math.floor(log10(Math.abs(upper))));
	else
	  expon = (int)(Math.floor(log10(Math.abs(lower))));
	if ((expon > 3) || (expon < -2)) {
	  n_ticks = 7;
	  do_exponential = true;
	} else if (((upper + lower) != 0) && 
		   ((log10(Math.abs((upper+lower)/(2*(upper-lower))))) > 2.25)) {
	  n_ticks = 7;
	}
	if ((upper < 0) || ((lower < 0) && (Math.abs(lower) < Math.abs(lower))))
	  n_ticks--;
	mantissa = Math.pow(10.0, (log10(upper-lower)) - Math.floor(log10(upper-lower)));
	for (i=0;i<4;i++) {
	  if (mantissa/interval[i]<n_ticks)
	    break;
	}
	if (i == 4)
	  i--;
	delta = interval[i] * Math.pow(10.0, Math.floor(log10(upper-lower)));
	start = (Math.floor(lower/delta)+1) * delta;
	number = (int)(((upper - start) / delta) + 1);
    }

    public static double log10(double a) {
	return (Math.log(a)/Math.log(10));
    }
}
