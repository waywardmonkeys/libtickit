#ifndef __TICKIT_H__
#define __TICKIT_H__

#include <stdlib.h>

/*
 * TickitPen
 */

typedef struct TickitPen TickitPen;

typedef enum {
  TICKIT_PEN_FG,         /* number: TODO - colour? */
  TICKIT_PEN_BG,         /* number: TODO - colour? */
  TICKIT_PEN_BOLD,       /* bool */
  TICKIT_PEN_UNDER,      /* bool: TODO - number? */
  TICKIT_PEN_ITALIC,     /* bool */
  TICKIT_PEN_REVERSE,    /* bool */
  TICKIT_PEN_STRIKE,     /* bool */
  TICKIT_PEN_ALTFONT,    /* number */

  TICKIT_N_PEN_ATTRS
} TickitPenAttr;

typedef enum {
  TICKIT_PENTYPE_BOOL,
  TICKIT_PENTYPE_INT
} TickitPenAttrType;

TickitPen *tickit_pen_new(void);
void       tickit_pen_destroy(TickitPen *pen);

int tickit_pen_has_attr(TickitPen *pen, TickitPenAttr attr);
int tickit_pen_is_nonempty(TickitPen *pen);
int tickit_pen_is_nondefault(TickitPen *pen);

int  tickit_pen_get_bool_attr(TickitPen *pen, TickitPenAttr attr);
void tickit_pen_set_bool_attr(TickitPen *pen, TickitPenAttr attr, int val);

int  tickit_pen_get_int_attr(TickitPen *pen, TickitPenAttr attr);
void tickit_pen_set_int_attr(TickitPen *pen, TickitPenAttr attr, int val);

void tickit_pen_clear_attr(TickitPen *pen, TickitPenAttr attr);

int tickit_pen_equiv_attr(TickitPen *a, TickitPen *b, TickitPenAttr attr);

void tickit_pen_copy_attr(TickitPen *dst, TickitPen *src, TickitPenAttr attr);

TickitPenAttrType tickit_pen_attrtype(TickitPenAttr attr);

/*
 * TickitTerm
 */

typedef struct TickitTerm TickitTerm;
typedef void TickitTermOutputFunc(TickitTerm *tt, const char *bytes, size_t len, void *user);

TickitTerm *tickit_term_new(void);
TickitTerm *tickit_term_new_for_termtype(const char *termtype);
void tickit_term_free(TickitTerm *tt);
void tickit_term_destroy(TickitTerm *tt);

void tickit_term_set_output_fd(TickitTerm *tt, int fd);
void tickit_term_set_output_func(TickitTerm *tt, TickitTermOutputFunc *fn, void *user);

void tickit_term_get_size(TickitTerm *tt, int *lines, int *cols);
void tickit_term_set_size(TickitTerm *tt, int lines, int cols);
void tickit_term_refresh_size(TickitTerm *tt);

void tickit_term_print(TickitTerm *tt, const char *str);
void tickit_term_goto(TickitTerm *tt, int line, int col);
void tickit_term_move(TickitTerm *tt, int downward, int rightward);
int  tickit_term_scrollrect(TickitTerm *tt, int top, int left, int lines, int cols, int downward, int rightward);

void tickit_term_chpen(TickitTerm *tt, TickitPen *pen);
void tickit_term_setpen(TickitTerm *tt, TickitPen *pen);

void tickit_term_clear(TickitTerm *tt);
void tickit_term_erasech(TickitTerm *tt, int count, int moveend);

void tickit_term_set_mode_altscreen(TickitTerm *tt, int on);
void tickit_term_set_mode_cursorvis(TickitTerm *tt, int on);
void tickit_term_set_mode_mouse(TickitTerm *tt, int on);

#endif
