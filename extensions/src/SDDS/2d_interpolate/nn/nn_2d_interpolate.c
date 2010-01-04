/* file: nn_2d_interpolate.c
 * nn 2d interpolate routine
 $Log: nn_2d_interpolate.c,v $
 Revision 1.1  2007/05/04 21:48:27  shang
 first version

*/
#include "mdb.h"
#include "nn_2d_interpolate.h"


specs* specs_create(void)
{
  specs* s = malloc(sizeof(specs));
  
  s->generate_points = s->thin =  s->nointerp =  s->linear =  s->invariant =  s->square = s->range = 0;
  s->nx =  s->ny = s->nxd =  s->nyd = -1;
  s->rmax = NaN;
  s->wmin = -DBL_MAX;
  s->zoom = 1.0;
  s->xmin =  s->xmax =  s->ymin = s->ymax = NaN; 
  s->npoints = 0;
  s->k = 140;
  s->nppc = 5;
  s->method = NN;
  return s;
}

static void points_write(int n, point* points)
{
    int i;

    for (i = 0; i < n; ++i) {
        point* p = &points[i];

        if (isnan(p->z))
            printf("%.15g %.15g NaN\n", p->x, p->y);
        else
            printf("%.15g %.15g %.15g\n", p->x, p->y, p->z);
    }
}


void generate_output_points(specs *spec, int *nout, point **pout)
{
  double stepx, stepy;
  double x0, xx, yy, y0, xdiff2, xav, ydiff2, yav;
  int i, j, ii;
  
  if (spec->nx < 1 || spec->ny < 1) {
    fprintf(stderr, "The output grid can be less than 1x1.\n");
    exit(1);
  } 
  if (!isnan(spec->zoom) && spec->zoom>1) {
    xdiff2 = (spec->xmax - spec->xmin)/2.0;
    xav = (spec->xmax + spec->xmin)/2.0;
    ydiff2 = (spec->ymax - spec->ymin)/2.0;
    yav = (spec->ymax + spec->ymin)/2.0;
    spec->xmin = xav - xdiff2 * spec->zoom;
    spec->xmax = xav + xdiff2 * spec->zoom;
    spec->ymin = yav - ydiff2 * spec->zoom;
    spec->ymax = yav + ydiff2 * spec->zoom;
  }
  
  *nout = spec->nx * spec->ny;
  *pout = malloc(*nout * sizeof(point));
  
  stepx = (spec->nx > 1) ? (spec->xmax - spec->xmin) / (spec->nx - 1) : 0.0;
  stepy = (spec->ny > 1) ? (spec->ymax - spec->ymin) / (spec->ny - 1) : 0.0;
  x0 = (spec->nx > 1) ? spec->xmin : (spec->xmin + spec->xmax) / 2.0;
  y0 = (spec->ny > 1) ? spec->ymin : (spec->ymin + spec->ymax) / 2.0; 
  ii = 0;
  xx = x0;
  yy = y0; 
  for (j = 0; j < spec->ny; j++) {
    xx = x0;
    for (i = 0; i < spec->nx; i++) {
      point* p = &(*pout)[ii];          
      p->x = xx;
      p->y = yy;
      xx += stepx;
      ii++;
    }
    yy += stepy;
  }
  spec->dx = stepx;
  spec->dy = stepy;
  if (spec->nx==1) 
    spec->xmin = spec->xmax = x0;
  if (spec->ny==1)
    spec->ymin = spec->ymax = y0; 
}

/*nn_routine*/
void do_nn_2d_interpolate(specs *spec, int *nin, point **pin, int *nout, point **pout) 
{
  minell* me = NULL;
  double k = NaN;
  
  if (spec->thin == 1)
    points_thingrid(nin, pin, spec->nxd, spec->nyd);
  else if (spec->thin == 2)
    points_thinlin(nin, pin, spec->rmax);
  
  if (spec->nointerp) {
    points_write(*nin, *pin);
    return;
  }
  if (!(*nout)) {
    points_getrange(*nin, *pin, spec->zoom, &spec->xmin, &spec->xmax, &spec->ymin, &spec->ymax);
    points_generate(spec->xmin, spec->xmax, spec->ymin, spec->ymax, spec->nx, spec->ny, nout, pout);
    if (spec->nx>1)
      spec->dx = (spec->xmax - spec->xmin)/(spec->nx -1);
    else 
      spec->dx = 0;
    if (spec->ny>1)
      spec->dy = (spec->ymax - spec->ymin)/(spec->ny -1);
    else 
      spec->dy=0;
  }
  if (spec->invariant) {
    me = minell_build(*nin, *pin);
    minell_scalepoints(me, *nin, *pin);
    minell_scalepoints(me, *nout, *pout);
  } else if (spec->square) {
    k = points_scaletosquare(*nin, *pin);
    points_scale(*nout, *pout, k);
  }
  if (spec->linear)
    lpi_interpolate_points(*nin, *pin, *nout, *pout);
  else
    nnpi_interpolate_points(*nin, *pin, spec->wmin, *nout, *pout);
  
  if (spec->invariant)
    minell_rescalepoints(me, *nout, *pout);
  else if (spec->square)
    points_scale(*nout, *pout, 1.0 / k);
  
  if (me) minell_destroy(me);
  return;
}
