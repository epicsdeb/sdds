/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: plot_data_sets()
 * purpose: plots multiple data sets on the same graph
 *      
 * Michael Borland, 1991-994.
 $Log: not supported by cvs2svn $
 Revision 1.10  2009/09/23 21:45:40  borland
 Use mtimes() instead of mtime() for date/time stamp (includes seconds).

 Revision 1.9  2002/08/14 17:24:52  soliday
 Added Open License

 Revision 1.8  2000/06/02 20:56:53  soliday
 Added thickness option to lineTypeDefault, drawLine, axes, tick and subtick
 options.

 Revision 1.7  2000/04/27 19:29:27  soliday
 Added support for line thickness.

 Revision 1.6  1999/07/22 18:35:04  soliday
 Added WIN32 support

 Revision 1.5  1999/06/03 16:07:36  soliday
 Removed compiler warnings under linux.

 Revision 1.4  1998/08/25 15:05:28  borland
 New version with major changes to allow multiple scales on x and y axes.

 * Revision 1.3  1996/02/29  05:42:34  borland
 * Added variable dot size using subtype qualifier of -graphic option.
 * Required adding an extra argument to the plot_dots() routine.
 *
 * Revision 1.2  1995/09/05  21:09:31  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "graph.h"

#if defined(_WIN32)
#include <windows.h>
#define sleep(sec) Sleep(sec * 1000)
#else
#include <unistd.h>
#endif

long window(double *x_new, double *y_new, double *sx_new, double *sy_new,
    double *x_old, double *y_old, double *sx_old, double *sy_old,
    double xmin, double xmax, double ymin, double ymax, long npts);
void label_graph(int graph_number, double x_center, double *x, double *y, long npts);
void adjust_overlay_data(double *x, double *u, long n_pts, double mag, 
    double posi, long flags, double xrange, double xmin, long margins_added);
void make_legend_old(char **legend, long *plot_code, long *subtype, int n, int do_box);
void time_date_stamp();
void strip_bounding_quotes(char *s, char *qb, char *qe);
long hstrlen(char *s);

int plot_data_sets(double **x, double **y, double **sigma_x, double **sigma_y,
    long *n, int np, char *title, char *xlab, char *ylab, char *top_line,
    double *limit, int *n_subticks, long *plot_code, long flags, 
    char *device_name, int pen, OVERLAY overlay, char **legend, double *pspace,
    double *lspace, long pause_interval, ZOOM *zoom)
{
    int i, npts;
    double *xn = NULL, *yn = NULL, *sxn = NULL, *syn = NULL;
    double xmin, xmax, ymin, ymax;
    double x_center;
    double xbar_size, ybar_size;
    double map[4];
    double pmin, pmax, qmin, qmax;
    int orig_linetype;
    double last_x = 0, last_y = 0;

    double ave, spread;
    int mapping_set, add_margins;

    mapping_set = 0;
    if (limit[0]==limit[1] && limit[2]==limit[3]) {
        /* auto-scaling needed, unless mapping is already set */
        get_mapping(map, map+1, map+2, map+3);
        if (map[0]!=map[1] && map[2]!=map[3])
            mapping_set = 1;
        }

    add_margins = 0 + (flags&UNSUPPRESS_X_ZERO ? 0 : 1) + (flags&UNSUPPRESS_Y_ZERO ? 0 : 2);
    flags |= add_margins*X_MARGINS_ADDED;

    if (!mapping_set) {
        if (limit[0]==limit[1] || limit[2]==limit[3])
            find_plot_window(limit, x, y, sigma_x, sigma_y, n,
                    plot_code, np, add_margins);
#if defined(DEBUG)
        fprintf(stderr, "auto-scaled limit: %e, %e, %e, %e\n", limit[0], limit[1], limit[2], limit[3]);
#endif
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

    if ((xmin=limit[0])==(xmax=limit[1]) || (ymin=limit[2])==(ymax=limit[3])) {
        if ( (xmin = limit[0]) >= (xmax = limit[1]) ) {
            fputs("warning: xmin>=xmax in plot_data_sets--no output generated\n", stderr); 
            return(1);
            }
        if ( (ymin = limit[2]) >= (ymax = limit[3]) ) {
            fputs("warning: ymin>=ymax in plot_data_sets--no output generated\n", stderr); 
            return(1);
            }
        }
        
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

    if (!(flags&DEVICE_DEFINED)) {
        if (change_term(device_name, strlen(device_name))== -1) {
            fprintf(stderr, "error: unknown graphics device.\n");
            list_terms(stderr);
            return(0);
            }
        set_title_at_top(0);
        graphics_on();
        if (legend && legend[0]) {
            if (lspace[0]==lspace[1]) {
                /* adjust pspace to suit default legend space */
                get_pspace(&pmin, &pmax, &qmin, &qmax);
                set_pspace(0.15, 0.85, qmin, qmax);
                if (lspace[2]==lspace[3])
                    set_legend_space(1.02, 1.18, 0.0, 0.9);
                else
                    set_legend_space(1.02, 1.18, lspace[2], lspace[3]);
                }
            else
                set_legend_space(lspace[0], lspace[1], lspace[2], lspace[3]);
            }
        if (flags&TITLE_AT_TOP) {
            /* adjust pspace to make room at top for the title */
            get_pspace(&pmin, &pmax, &qmin, &qmax);
            qmax -= 0.05;
            qmin -= 0.05;
            set_pspace(pmin, pmax, qmin, qmax);
            }
        if (pspace[0]!=pspace[1] || pspace[2]!=pspace[3]) {
            get_pspace(&pmin, &pmax, &qmin, &qmax);
            if (pspace[0]<pspace[1]) {
                pmin = pspace[0];
                pmax = pspace[1];
                }
            if (pspace[2]<pspace[3]) {
                qmin = pspace[2];
                qmax = pspace[3];
                }
            set_pspace(pmin, pmax, qmin, qmax);
            }
        set_clipping(1, 1, 1);
        }
    set_mapping(limit[0], limit[1], limit[2], limit[3]);

    if (flags&TITLE_AT_TOP)
        set_title_at_top(1);
    else
        set_title_at_top(0);

    x_center = (xmax+xmin)/2;
    ybar_size = (xmax-xmin)/150.0;
    xbar_size = (ymax-ymin)/150.0;

    if (flags&EQUAL_ASPECT1) {
        set_aspect((double)1.0);
        }
    else if (flags&EQUAL_ASPECT_1) {
        set_aspect((double)(-1.0));
        }

    if (flags&DO_WINDOW) {
        /* find the maximum number of points in any of the arrays of points,
         * and allocate two arrays to hold that number of points.
         */
        for (i=npts=0; i<np; i++)
            if (n[i]>npts)
                npts = n[i];
        xn = tmalloc(npts*sizeof(*xn));
        yn = tmalloc(npts*sizeof(*yn));
        sxn = tmalloc(npts*sizeof(*sxn));
        syn = tmalloc(npts*sizeof(*syn));
        }

    orig_linetype = get_linetype();
    /* loop over the sets of points, plotting each graph */
    for (i=0; i<np; i++) {
        if (flags&MULTIPEN && i!=0 && !(flags&UNMULTI_LAST && i==np-1))
            plot_code[i] = (plot_code[0]&0xff00) | ((plot_code[0]&0x00ff)+i)%0x00ff;
        if (flags&DO_WINDOW)
            /* window the ith set of points to get only those points within
             * [xmin,xmax]x[ymin,ymax].
             */
            npts = window(xn, yn, sxn, syn, x[i], y[i], sigma_x[i], sigma_y[i],
                        xmin, xmax, ymin, ymax, n[i]);
        else {
            npts = n[i];
            xn = x[i];
            yn = y[i];
            sxn = sigma_x[i];
            syn = sigma_y[i];
            }

        if (npts==0)
            continue;

        if (plot_code[i]&PLOT_LINE) {
            /* plot a line */
            if (flags&DO_SEVER)
                plot_lines_sever(xn, yn, npts, plot_code[i]&PLOT_CODE_MASK,0);
            else
                plot_lines(xn, yn, npts, plot_code[i]&PLOT_CODE_MASK,0);
            }
        else if (plot_code[i]&PLOT_DOTS) {
            plot_dots(xn, yn, npts, plot_code[i]&PLOT_CODE_MASK, 0);
            }
        else if (plot_code[i]&PLOT_IMPULSE) {
            plot_impulses(xn, yn, npts, plot_code[i]&PLOT_CODE_MASK, 0);
            }
        else if (plot_code[i]&PLOT_SYMBOL || plot_code[i]&PLOT_CSYMBOL) {
            plot_points_old(xn, yn, npts, plot_code[i]&(PLOT_CODE_MASK+PLOT_SIZE_MASK));
            if (flags&DO_CONNECT || plot_code[i]&PLOT_CSYMBOL) {
                if (flags&DO_SEVER)
                    plot_lines_sever(xn, yn, npts, 0,0);
                else
                    plot_lines(xn, yn, npts, 0,0);
                }
            }
        else if (plot_code[i]&PLOT_ERRORBAR) {
            plot_error_bars(xn, yn, sxn, syn, npts, plot_code[i]&PLOT_CODE_MASK, 0);
            if (flags&DO_CONNECT) {
                if (flags&DO_SEVER)
                    plot_lines_sever(xn, yn, npts, 0,0);
                else
                    plot_lines(xn, yn, npts, 0,0);
                }
            }
        else if (plot_code[i]&PLOT_ARROW)
            plot_arrows_old(xn, yn, sxn, syn, npts, plot_code[i]&PLOT_CODE_MASK, flags);
        if (flags&NUMBER_GRAPHS)
            label_graph(i, x_center, xn, yn, npts);
        last_x = xn[npts-1];
        last_y = yn[npts-1];
    }
    set_linetype(orig_linetype);
    
    if (overlay) {
        double *xov, *yov;
        do {
            xov = tmalloc(sizeof(*xov)*overlay->n_pts);
            yov = tmalloc(sizeof(*yov)*overlay->n_pts);
            adjust_overlay_data(xov, overlay->x, overlay->n_pts, 
                overlay->hmag, overlay->hoff, overlay->flags, 
                xmax-xmin, xmin, flags&X_MARGINS_ADDED);
            adjust_overlay_data(yov, overlay->y, overlay->n_pts, 
                overlay->vmag, overlay->voff, overlay->flags, 
                ymax-ymin, ymin, flags&Y_MARGINS_ADDED);
            plot_lines(xov, yov, overlay->n_pts, overlay->plotcode&PLOT_CODE_MASK,0);
            if (!(overlay->flags&OVL_ANOTHER_FOLLOWS))
                break;
            free(xov); 
            free(yov);
            overlay++;
            } while (1);
        }
    set_linetype(orig_linetype);

    /* do scales, border, axes, labels, etc. if desired */
    if (!(flags&NO_SCALES)) {
        if (!(flags&X_GRID))
            make_scales(0, 0, 1, 0.0, 0.0, 0.0, 0.0, 1.0, n_subticks[0], 0, 0, 0, 0);
        else
            make_scales(0, 0, 1, 0.0, -1.0, 0.0, 0.0, 1.0, n_subticks[0], 0, 0, 0, 0);
        if (!(flags&Y_GRID))
            make_scales(1, 0, 1, 0.0, 0.0, 0.0, 0.0, 1.0, n_subticks[1], 0, 0, 0, 0);
        else
            make_scales(1, 0, 1, 0.0, -1.0, 0.0, 0.0, 1.0, n_subticks[1], 0, 0, 0, 0);
        }

    if (!(flags&NO_BORDER))
        border();

    if (flags&DO_X_AXIS) {
        make_x_axis(0);
        }
    if (flags&DO_Y_AXIS) {
        make_y_axis(0);
        }

    if (!(flags&NO_LABELS)) {
        plot_labels(title, xlab, ylab);
        plot_topline(top_line);
        }

    if (legend)
        make_legend_old(legend, plot_code, NULL, np, flags&BOX_LEGEND);

    if (flags&DATE_STAMP)
        time_date_stamp();

    pmove(last_x, last_y);

    if (!(flags&NO_GREND)) {
        if (flags&SLEEP_WAIT)
            frame_end(2);
        else if (!(flags&RAPID_FIRE))
            frame_end(1);
        else {
            frame_update();
            if (pause_interval>0) {
	      sleep((unsigned)pause_interval);
	    }
            frame_end(0);
            }
        }

    return(1);
    }

/* routine: window()
 * purpose: filter set of points {(xold, yold)} with a specified
 *          window, copying the points inside the window into user-supplied
 *          arrays.
 */

long window(
    double *x_new, double *y_new,
    double *sx_new, double *sy_new,
    double *x_old, double *y_old,
    double *sx_old, double *sy_old,
    double xmin, double xmax, double ymin, double ymax,
    long npts
    )
{
    long i, j;
    double x, y;

    for (i=j=0; i<npts; i++) {
        x = x_old[i];
        if (x<xmin)
            continue;
        if (x>xmax)
            continue;
        y = y_old[i];
        if (y<ymin)
            continue;
        if (y>ymax)
            continue;
        x_new[j] = x;
        y_new[j] = y;
        if (sx_old!=0)
            sx_new[j] = sx_old[i];
        if (sy_old!=0)
            sy_new[j] = sy_old[i];
        j++;
        }
    return(j);
    }

/* routine: label_graph()
 * purpose: put a number on a graph in order to label it.
 */

void label_graph(int graph_number, double x_center, double *x, double *y, long npts)
{
    int i, i_closest_point;
    double min_dist_to_data_point;
    double xd, yd;
    static char s[10];

    /* find the point in the data that's closest to the center
     */
    min_dist_to_data_point = 1.6e38;
    i_closest_point        =  npts/2;
    for (i=0; i<npts; i++)
        if ( ((double)fabs((double)(x[i]-x_center))) < min_dist_to_data_point ) {
            min_dist_to_data_point = (double)fabs((double)(x[i]-x_center));
            i_closest_point = i;
            }
    
    i_closest_point += (graph_number%3)-1;
    if (i_closest_point<0)
        i_closest_point = 0;
    if (i_closest_point>=npts)
        i_closest_point = npts - 1;

    sprintf(s, "%d", graph_number);

    xd = x[i_closest_point];
    yd = y[i_closest_point];
    plot_string(&xd, &yd, s);
    }

void adjust_overlay_data(
    double *x, double *u, long n_pts, double mag, 
    double posi, long flags, double xrange, double xmin,
    long margins_added
    )
{
    long i;
    double umin, umax, urange;

    umin = umax = 0;
    find_min_max(&umin, &umax, u, n_pts);
    if (umin==umax)
        bomb("range of data is zero (adjust_overlay_data)", NULL);
    urange = (umax - umin); /* *(margins_added?1.1:1.0); */

    if (mag<=0) {
        /* plot the data unaltered */
        for (i=0; i<n_pts; i++)
            x[i] = u[i];
        return;
        }

    if (flags&OVL_USERC_OFFSET) {
        for (i=0; i<n_pts; i++) 
            x[i] = mag*xrange/urange*u[i] + posi;
        }
    else
        for (i=0; i<n_pts; i++)
            x[i] = mag*xrange/urange*u[i] + xmin + xrange*posi;
/*                    (xmin+(margins_added?0.05*xrange/1.1:0.0)) + xrange*posi; */
    }

void make_legend_old(char **legend, long *plot_code, long *subtype, int n, int do_box)
{
    int i;
    double h_save, v_save, hu, vu;
    double dv, xmin, xmax, ymin, ymax;
    double x[2], y[2], sx[2], sy[2], yrange, xrange;
    double xb[5], yb[5];        /* for box */
    double pmin, pmax, qmin, qmax;
    double posc, posl, posr, post; /* center, left, right, top, and bottom of legend region in pspace coordinates */
    double dpos_lr;
    double lpmin, lpmax, lqmin, lqmax, asf, v_offset, legend_xrange;
    long len, maxlen, n_legends, orig_linetype;
    
    get_char_size(&h_save, &v_save, 1);   /* save character size in user's coordinates */
    hu = h_save; vu = v_save;
    get_mapping(&xmin, &xmax, &ymin, &ymax);
    get_pspace(&pmin, &pmax,&qmin, &qmax);
    if (!(yrange=ymax-ymin))
        bomb("y range is zero (make_legend_old)", NULL);
    if (!(xrange=xmax-xmin))
        bomb("x range is zero (make_legend_old)", NULL);
    if (pmin==pmax || qmin==qmax)
        bomb("pspace invalid", NULL);
    for (i=n_legends=0; i<n; i++) {
        if (legend[i] && !is_blank(legend[i]))
            n_legends++;
        }
    if (n_legends==0)
        return;
    if (yrange/n_legends<vu*5) {
       vu = yrange/n_legends/5.0;
       hu = h_save/v_save*vu;
       set_char_size(hu, vu, 1);
       }

    /* get legend space, which specifies the region for legends
     * in (0,1)x(0,1) == (pmin,pmax)x(qmin,qmax) coordinates 
     */
    get_legend_space(&lpmin, &lpmax, &lqmin, &lqmax);

    /* compute pspace coordinates of legend region */
    posr = pmin + lpmax*(pmax-pmin);
    posl = pmin + lpmin*(pmax-pmin); 
    posc = (posl+posr)/2;
    dpos_lr = (posr-posl)/2;
    post = qmin + lqmax*(qmax-qmin);
    ymax = (post-qmin)*yrange/(qmax-qmin) + ymin;

    legend_xrange = xrange/(pmax-pmin)*(posr-posl);
    for (i=maxlen=0; i<n; i++) {
        if (!legend[i] || is_blank(legend[i])) 
            continue;
#if 0
        strip_bounding_quotes(legend[i], "$i", "$d");
#endif
        strip_bounding_quotes(legend[i], "$v", "$u");
        if ((len=hstrlen(legend[i]))>maxlen)
            maxlen = len;
        }
    if (legend_xrange<maxlen*hu) {
        hu = legend_xrange/maxlen;
        vu = hu/h_save*v_save;
        set_char_size(hu, vu, 1);
        }

    dv = vu;

    if (do_box) {
        dpos_lr *= 0.8;
        xb[0] = xb[1] = xb[4] = (posl-pmin)*xrange/(pmax-pmin) + xmin;
        xb[2] = xb[3] = (posr-pmin)*xrange/(pmax-pmin) + xmin;
        yb[1] = yb[2] = ymax + 3.25*dv;
        }

    asf = 1;
    set_arrow_scale_factor(&asf);
    v_offset = 0;
    set_clipping(0, 0, 0);
    orig_linetype = get_linetype();
    for (i=0; i<n; i++) {
        x[0] = (posc-pmin)*xrange/(pmax-pmin)+xmin;
        y[0] = ymax + 1.5*dv - 5*i*dv + v_offset;
        if (!legend[i] || is_blank(legend[i])) {
            v_offset += 5*dv;
            continue;
            }
        cplot_string(x, y, legend[i]);
        switch (plot_code[i]&0xff00) {
            case PLOT_LINE:
            case PLOT_IMPULSE:
                x[0] = (posc-pmin-dpos_lr)*xrange/(pmax-pmin) + xmin;
                x[1] = (posc-pmin+dpos_lr)*xrange/(pmax-pmin) + xmin;
                y[0] = y[1] = ymax - (5*i+0)*dv + v_offset;
                plot_lines(x, y, 2, plot_code[i]&PLOT_CODE_MASK,0);
                break;
            case PLOT_DOTS:
                x[0] = (posc-pmin)*xrange/(pmax-pmin)+xmin;
                y[0] = ymax - (5*i+0)*dv + v_offset;
                plot_dots(x, y, 1, plot_code[i]&PLOT_CODE_MASK, subtype?subtype[i]:0);
                break;
            case PLOT_SYMBOL:
                if (subtype && subtype[i]!=orig_linetype)
                    set_linetype(subtype[i]);
                x[0] = (posc-pmin)*xrange/(pmax-pmin)+xmin;
                y[0] = ymax - (5*i+0)*dv + v_offset;
                plot_points_old(x, y, 1, plot_code[i]&(PLOT_CODE_MASK+PLOT_SIZE_MASK));
                if (subtype && subtype[i]!=orig_linetype)
                    set_linetype(orig_linetype);
                break;
            case PLOT_CSYMBOL:
                if (subtype && subtype[i]!=orig_linetype)
                    set_linetype(subtype[i]);
                x[0] = (posc-pmin)*xrange/(pmax-pmin)+xmin;
                y[0] = y[1] = ymax - (5*i+0)*dv + v_offset;
                plot_points_old(x, y, 1, plot_code[i]&(PLOT_CODE_MASK+PLOT_SIZE_MASK)); 
                x[0] = (posc-pmin-dpos_lr)*xrange/(pmax-pmin)+xmin;
                x[1] = (posc-pmin+dpos_lr)*xrange/(pmax-pmin)+xmin;
                plot_lines(x, y, 2, get_linetype(),0);
                if (subtype && subtype[i]!=orig_linetype)
                    set_linetype(orig_linetype);
                break;
            case PLOT_ERRORBAR:
                if (subtype && subtype[i]!=orig_linetype)
                    set_linetype(subtype[i]);
                x[0] = (posc-pmin)*xrange/(pmax-pmin)+xmin;
                y[0] = ymax - (5*i+0)*dv + v_offset;
                sx[0] = dv*0.25;
                sy[0] = xrange*0.01;
                plot_error_bars(x, y, sx, sy, 1, plot_code[i]&(PLOT_CODE_MASK+PLOT_SIZE_MASK), 0); 
                if (subtype && subtype[i]!=orig_linetype)
                    set_linetype(orig_linetype);
                break;
            case PLOT_ARROW:
                if (subtype && subtype[i]!=orig_linetype)
                    set_linetype(subtype[i]);
                x[0] = (posc-pmin-dpos_lr)*xrange/(pmax-pmin) + xmin;
                x[1] = (posc-pmin+dpos_lr)*xrange/(pmax-pmin) + xmin;
                y[0] = y[1] = ymax - (5*i+0)*dv + v_offset;
                plot_arrow_old(x[0], y[0], x[1]-x[0], 0.0, -1.0, 30./180.*PI, plot_code[i]&PLOT_CODE_MASK, 0L);
                if (subtype && subtype[i]!=orig_linetype)
                    set_linetype(orig_linetype);
                break;
            default:
                bomb("unknown plot code in make_legend_old()", NULL);
                break;
            }
        }

    if (do_box) {
        yb[0] = yb[3] = yb[4] = y[0] - 0.75*dv;
        plot_lines(xb, yb, 5, orig_linetype,0);
        }
    set_char_size(h_save, v_save, 1);
    set_arrow_scale_factor(&asf);
    set_clipping(1,1,1);
    set_linetype(orig_linetype);
    }

void time_date_stamp()
{
    double h_save, v_save, hu, vu;
    double xmin, xmax, ymin, ymax;
    double x[2], y[2], yrange, xrange;
    double pmin, pmax, qmin, qmax;
    double posc, posl, posr;
    char *ptr0, *ptr1;

    get_char_size(&h_save, &v_save, 1);   /* save character size in user's coordinates */
    hu = h_save; vu = v_save;
    get_mapping(&xmin, &xmax, &ymin, &ymax);
    get_pspace(&pmin, &pmax,&qmin, &qmax);
    if (!(yrange=ymax-ymin))
        bomb("y range is zero (time_date_stamp)", NULL);
    if (!(xrange=xmax-xmin))
        bomb("x range is zero (time_date_stamp)", NULL);
    if (pmin==pmax || qmin==qmax)
        bomb("pspace invalid", NULL);

    hu = (xmax-xmin)*0.01;
    vu = hu*v_save/h_save;
    set_char_size(hu, vu, 1);
    posc = 1 - (1-pmax)/2;
    posl = posc - 0.8*(1-pmax)/2;
    posr = posc + 0.8*(1-pmax)/2;
    set_clipping(0, 0, 0);
    ptr0 = mtimes();
    if ((ptr1=strrchr(ptr0, ' '))) {
        *ptr1++ = 0;
        x[0] = (posc-pmin)*xrange/(pmax-pmin)+xmin;
        y[0] = ymax + 2.0*vu;
        cplot_string(x, y, ptr0);
        x[0] = (posc-pmin)*xrange/(pmax-pmin)+xmin;
        y[0] = ymax + 3.75*vu;
        cplot_string(x, y, ptr1);
        }
    free(ptr0);
    set_char_size(h_save, v_save, 1);
    set_clipping(1,1,1);
    }

void strip_bounding_quotes(char *s, char *qb, char *qe)
{
    char *ptr;
    int level;

    ptr = s;
    level = 0;
    while (*ptr) {
        if (strncmp(ptr, qb, strlen(qb))==0) {
            if (level==-1 || level==0)
               strcpy_ss(ptr, ptr+2);
            else
               ptr += 2;
            level++; 
            }
        else if (strncmp(ptr, qe, strlen(qe))==0) {
            if (level==1 || level==0)
               strcpy_ss(ptr, ptr+2);
            else
               ptr += 2;
            level--;
            }
        else
            ptr++;
        }
    }

long hstrlen(char *s)
{
    double size=1, count=0;
    while (*s) {
        if (*s=='$') {
            switch (*(s+1)) {
              case 'd': case 'a': case 'b':
                size *= 0.5;
                s += 1;
                break;
              case 'i': case 'n':
                size *= 2;
                s += 1;
                break;
              case '$':
                s += 1;
                count += size;
                break;
              default:
                break;
                }
            }
        else {
            count += size;
            }
        s++;
        }
    return( (long)(count+0.5) );
    }

void find_plot_window(
    double *limit, double **x, double **y, double **sigma_x, double **sigma_y, 
    long *n, long *plot_code, int np, int add_margins
    )
{
    int i, j;
    int do_sigma_x, do_sigma_y;
    double xij, _xij, _yij;
    double ave, spread, ymin, ymax, xmin, xmax;
    double *x_i, *y_i;

    if (limit[0]==limit[1]) {
        /* auto-scale on x */
        xmin = DBL_MAX;
        xmax = -DBL_MAX;
        for (i=0; i<np; i++) {
            do_sigma_x = (sigma_x[i] && (plot_code[i]&PLOT_ERRORBAR));
            for (j=0; j<n[i]; j++) {
                xij = x[i][j];
                if (xmin>(_xij=xij-(do_sigma_x?sigma_x[i][j]:0)))
                    xmin = _xij;
                if (xmax<(_xij=xij+(do_sigma_x?sigma_x[i][j]:0))) 
                    xmax = _xij;
                }
            }
        if (xmin==DBL_MAX || xmax==-DBL_MAX)
            bomb("all data files are empty", NULL);
        if (add_margins&1) {
            ave = (xmin+xmax)/2;
            spread = 1.1*(xmax-xmin)/2;
            xmin = ave - spread;
            xmax = ave + spread;
            }
        limit[0] = xmin;
        limit[1] = xmax;
        }
    else {
        xmin = limit[0];
        xmax = limit[1];
        }
    if (fabs(xmin-xmax)<1e-18) {
        ave = (xmax+xmin)/2;
        limit[0] = xmin = ave - 1;
        limit[1] = xmax = ave + 1;
        }

    
    if (limit[2]==limit[3]) {
        ymin =  DBL_MAX;
        ymax = -DBL_MAX;
        /* search all y values for global ymin, ymax */
        for (i=0; i<np; i++) {
            y_i = y[i];
            x_i = x[i];
            do_sigma_x = (sigma_x[i] && (plot_code[i]&PLOT_ERRORBAR));
            do_sigma_y = (sigma_y[i] && (plot_code[i]&PLOT_ERRORBAR));
            for (j=0;  j<n[i];  j++) {
                if ((x_i[j]+(do_sigma_x?sigma_x[i][j]:0))>xmax || 
                    (x_i[j]-(do_sigma_x?sigma_x[i][j]:0))<xmin) 
                    continue;
                if ((_yij=y_i[j]-(do_sigma_y?sigma_y[i][j]:0))<ymin)
                    ymin = _yij;
                if ((_yij=y_i[j]+(do_sigma_y?sigma_y[i][j]:0))>ymax)
                    ymax = _yij;
                }
            }
        if (add_margins&2) {
            ave = (ymin+ymax)/2;
            spread = 1.1*(ymax-ymin)/2;
            limit[2] = ave - spread;
            limit[3] = ave + spread;
            }
        else {
            limit[2] = ymin;
            limit[3] = ymax;
            }
        }
    if (fabs(limit[2]-limit[3])<1e-18) {
        limit[2] -= 1;
        limit[3] += 1;
        }
    }


