/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: multi_plot()
 * purpose: to allow screen/hardcopy plotting of several sets of
 *          data on the same screen, with scaling, labeling, etc.
 $Log: not supported by cvs2svn $
 Revision 1.3  1998/08/25 15:05:27  borland
 New version with major changes to allow multiple scales on x and y axes.

 * Revision 1.2  1995/09/05  21:09:27  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "graph.h"

int multi_plot(double **x, double **y, double **sigma_x, double **sigma_y,
    long *n, int np, char *title, char *xlab, char *ylab, char *top_l,
    double *limit, int *n_subticks, long *plot_code, long flags, 
    char *device_name, int pen, OVERLAY overlay, char **legend, double *pspace,
    double *lspace, long pause_interval, ZOOM *zoom
    )
{
    double ave, spread;
    int mapping_set, add_margins;
    double map[4];

    mapping_set = 0;
    if (limit[0]==limit[1] && limit[2]==limit[3]) {
        /* auto-scaling needed, unless mapping is already set */
        get_mapping(map, map+1, map+2, map+3);
        if (map[0]!=map[1] && map[2]!=map[3]) {
            mapping_set = 1;
            }
        }

    add_margins = 0 + (flags&UNSUPPRESS_X_ZERO ? 0 : 1) + (flags&UNSUPPRESS_Y_ZERO ? 0 : 2);
    flags |= add_margins*X_MARGINS_ADDED;

    if (!mapping_set) {
        if (limit[0]==limit[1] || limit[2]==limit[3])
            find_plot_window(limit, x, y, sigma_x, sigma_y, n,
                    plot_code, np, add_margins);
        if (flags&UNSUPPRESS_X_ZERO) {
            if (limit[0]>0) {
                ave    = limit[1]/2.;
                spread = 1.1*ave;
                }
            else if (limit[1]<0) {
                ave    = limit[0]/2.;
                spread = -1.1*ave;
                }
            else {
                /* add margins */
                flags |= X_MARGINS_ADDED;
                ave    = (limit[0]+limit[1])/2;
                spread = 1.1*(limit[1]-limit[0])/2;
                }
            limit[0] = ave-spread;
            limit[1] = ave+spread;
            }
        if (flags&UNSUPPRESS_Y_ZERO) {
            if (limit[2]>0) {
                ave    = limit[3]/2.;
                spread = 1.1*ave;
                }
            else if (limit[3]<0) {
                ave    = limit[2]/2.;
                spread = -1.1*ave;
                }
            else {
                /* add margins */
                flags |= Y_MARGINS_ADDED;
                ave    = (limit[2]+limit[3])/2;
                spread = 1.1*(limit[3]-limit[2])/2;
                }
            limit[2] = ave-spread;
            limit[3] = ave+spread;
            }
        }
    else 
        limit = map;

    if (zoom->xmag>0) {
        if (zoom->auto_center)
            ave = (limit[0] + limit[1])/2;
        else
            ave = zoom->xcent;
        spread = (limit[1]-limit[0])/(2*zoom->xmag);
        limit[0] = ave - spread;
        limit[1] = ave + spread;
        }
    if (zoom->ymag>0) {
        if (zoom->auto_center)
            ave = (limit[2] + limit[3])/2;
        else
            ave = zoom->ycent;
        spread = (limit[3]-limit[2])/(2*zoom->ymag);
        limit[2] = ave - spread;
        limit[3] = ave + spread;
        }

    /* screen plotting */
    return(
           plot_data_sets(x, y, sigma_x, sigma_y, n, np, title, xlab, ylab, top_l, limit, n_subticks,
                          plot_code, flags, device_name, pen, overlay, legend, pspace, lspace,
                          pause_interval, zoom)
           ) ;
    }


