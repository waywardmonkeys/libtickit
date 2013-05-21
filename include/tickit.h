#ifndef __TICKIT_H__
#define __TICKIT_H__

#include <stdlib.h>

/*
 * Tickit events
 */

/* bitmasks */
typedef enum {
  TICKIT_EV_RESIZE = 0x01, // Term = lines, cols
  TICKIT_EV_KEY    = 0x02, // Term = type(TickitKeyEventType), str
  TICKIT_EV_MOUSE  = 0x04, // Term = type(TickitMouseEventType), button, line, col
  TICKIT_EV_CHANGE = 0x08, // Pen = {none}

  TICKIT_EV_UNBIND = 0x80000000, // event handler is being unbound
} TickitEventType;

typedef enum {
  TICKIT_KEYEV_KEY = 1,
  TICKIT_KEYEV_TEXT,
} TickitKeyEventType;

typedef enum {
  TICKIT_MOUSEEV_PRESS = 1,
  TICKIT_MOUSEEV_DRAG,
  TICKIT_MOUSEEV_RELEASE,
  TICKIT_MOUSEEV_WHEEL,
} TickitMouseEventType;

enum {
  TICKIT_MOUSEWHEEL_UP = 1,
  TICKIT_MOUSEWHEEL_DOWN,
};

enum {
  TICKIT_MOD_SHIFT = 0x01,
  TICKIT_MOD_ALT   = 0x02,
  TICKIT_MOD_CTRL  = 0x04,
};

typedef struct {
  int         lines, cols; // RESIZE
  int         type;        // KEY, MOUSE
  const char *str;         // KEY
  int         button;      // MOUSE
  int         line, col;   // MOUSE
  int         mod;         // KEY, MOUSE
} TickitEvent;

/*
 * TickitPen
 */

typedef struct TickitPen TickitPen;

typedef enum {
  TICKIT_PEN_FG,         /* colour */
  TICKIT_PEN_BG,         /* colour */
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
  TICKIT_PENTYPE_INT,
  TICKIT_PENTYPE_COLOUR,
} TickitPenAttrType;

TickitPen *tickit_pen_new(void);
TickitPen *tickit_pen_clone(TickitPen *orig);
void       tickit_pen_destroy(TickitPen *pen);

int tickit_pen_has_attr(const TickitPen *pen, TickitPenAttr attr);
int tickit_pen_is_nonempty(const TickitPen *pen);
int tickit_pen_is_nondefault(const TickitPen *pen);

int  tickit_pen_get_bool_attr(const TickitPen *pen, TickitPenAttr attr);
void tickit_pen_set_bool_attr(TickitPen *pen, TickitPenAttr attr, int val);

int  tickit_pen_get_int_attr(const TickitPen *pen, TickitPenAttr attr);
void tickit_pen_set_int_attr(TickitPen *pen, TickitPenAttr attr, int val);

int  tickit_pen_get_colour_attr(const TickitPen *pen, TickitPenAttr attr);
void tickit_pen_set_colour_attr(TickitPen *pen, TickitPenAttr attr, int value);
int  tickit_pen_set_colour_attr_desc(TickitPen *pen, TickitPenAttr attr, const char *value);

void tickit_pen_clear_attr(TickitPen *pen, TickitPenAttr attr);

int tickit_pen_equiv_attr(const TickitPen *a, const TickitPen *b, TickitPenAttr attr);
int tickit_pen_equiv(const TickitPen *a, const TickitPen *b);

void tickit_pen_copy_attr(TickitPen *dst, const TickitPen *src, TickitPenAttr attr);
void tickit_pen_copy(TickitPen *dst, const TickitPen *src, int overwrite);

typedef void TickitPenEventFn(TickitPen *tt, TickitEventType ev, TickitEvent *args, void *data);

int  tickit_pen_bind_event(TickitPen *tt, TickitEventType ev, TickitPenEventFn *fn, void *data);
void tickit_pen_unbind_event_id(TickitPen *tt, int id);

TickitPenAttrType tickit_pen_attrtype(TickitPenAttr attr);
const char *tickit_pen_attrname(TickitPenAttr attr);
TickitPenAttr tickit_pen_lookup_attr(const char *name);

/*
 * TickitRect
 */

typedef struct {
  int top;
  int left;
  int lines;
  int cols;
} TickitRect;

void tickit_rect_init_sized(TickitRect *rect, int top, int left, int lines, int cols);
void tickit_rect_init_bounded(TickitRect *rect, int top, int left, int bottom, int right);

static inline int tickit_rect_bottom(const TickitRect *rect)
{ return rect->top + rect->lines; }

static inline int tickit_rect_right (const TickitRect *rect)
{ return rect->left + rect->cols; }

int tickit_rect_intersect(TickitRect *dst, const TickitRect *a, const TickitRect *b);

int tickit_rect_intersects(const TickitRect *a, const TickitRect *b);
int tickit_rect_contains(const TickitRect *large, const TickitRect *small);

int tickit_rect_add(TickitRect ret[3], const TickitRect *a, const TickitRect *b);
int tickit_rect_subtract(TickitRect ret[4], const TickitRect *orig, const TickitRect *hole);

/*
 * TickitRectSet
 */

typedef struct TickitRectSet TickitRectSet;

TickitRectSet *tickit_rectset_new(void);
void tickit_rectset_destroy(TickitRectSet *trs);

void tickit_rectset_clear(TickitRectSet *trs);

size_t tickit_rectset_rects(const TickitRectSet *trs);
size_t tickit_rectset_get_rects(const TickitRectSet *trs, TickitRect rects[], size_t n);

void tickit_rectset_add(TickitRectSet *trs, const TickitRect *rect);
void tickit_rectset_subtract(TickitRectSet *trs, const TickitRect *rect);

int tickit_rectset_intersects(const TickitRectSet *trs, const TickitRect *rect);
int tickit_rectset_contains(const TickitRectSet *trs, const TickitRect *rect);

/*
 * TickitTerm
 */

typedef struct TickitTerm TickitTerm;
typedef void TickitTermOutputFunc(TickitTerm *tt, const char *bytes, size_t len, void *user);

TickitTerm *tickit_term_new(void);
TickitTerm *tickit_term_new_for_termtype(const char *termtype);
void tickit_term_destroy(TickitTerm *tt);

const char *tickit_term_get_termtype(TickitTerm *tt);

void tickit_term_set_output_fd(TickitTerm *tt, int fd);
int  tickit_term_get_output_fd(const TickitTerm *tt);
void tickit_term_set_output_func(TickitTerm *tt, TickitTermOutputFunc *fn, void *user);
void tickit_term_set_output_buffer(TickitTerm *tt, size_t len);

void tickit_term_flush(TickitTerm *tt);

/* fd is allowed to be unset (-1); works abstractly */
void tickit_term_set_input_fd(TickitTerm *tt, int fd);
int  tickit_term_get_input_fd(const TickitTerm *tt);

int  tickit_term_get_utf8(const TickitTerm *tt);
void tickit_term_set_utf8(TickitTerm *tt, int utf8);

void tickit_term_input_push_bytes(TickitTerm *tt, const char *bytes, size_t len);
void tickit_term_input_readable(TickitTerm *tt);
int  tickit_term_input_check_timeout(TickitTerm *tt);
void tickit_term_input_wait(TickitTerm *tt);

void tickit_term_get_size(const TickitTerm *tt, int *lines, int *cols);
void tickit_term_set_size(TickitTerm *tt, int lines, int cols);
void tickit_term_refresh_size(TickitTerm *tt);

typedef void TickitTermEventFn(TickitTerm *tt, TickitEventType ev, TickitEvent *args, void *data);

int  tickit_term_bind_event(TickitTerm *tt, TickitEventType ev, TickitTermEventFn *fn, void *data);
void tickit_term_unbind_event_id(TickitTerm *tt, int id);

void tickit_term_print(TickitTerm *tt, const char *str);
int  tickit_term_goto(TickitTerm *tt, int line, int col);
void tickit_term_move(TickitTerm *tt, int downward, int rightward);
int  tickit_term_scrollrect(TickitTerm *tt, int top, int left, int lines, int cols, int downward, int rightward);

void tickit_term_chpen(TickitTerm *tt, const TickitPen *pen);
void tickit_term_setpen(TickitTerm *tt, const TickitPen *pen);

void tickit_term_clear(TickitTerm *tt);
void tickit_term_erasech(TickitTerm *tt, int count, int moveend);

typedef enum {
  /* This is part of the API so additions must go at the end only */
  TICKIT_TERMCTL_ALTSCREEN = 1,
  TICKIT_TERMCTL_CURSORVIS,
  TICKIT_TERMCTL_MOUSE,
  TICKIT_TERMCTL_CURSORBLINK,
  TICKIT_TERMCTL_CURSORSHAPE,
  TICKIT_TERMCTL_ICON_TEXT,
  TICKIT_TERMCTL_TITLE_TEXT,
  TICKIT_TERMCTL_ICONTITLE_TEXT,
  TICKIT_TERMCTL_KEYPAD_APP,
  TICKIT_TERMCTL_COLORS, // read-only
} TickitTermCtl;

typedef enum {
  TICKIT_TERM_CURSORSHAPE_BLOCK = 1,
  TICKIT_TERM_CURSORSHAPE_UNDER,
  TICKIT_TERM_CURSORSHAPE_LEFT_BAR,
} TickitTermCursorShape;

int tickit_term_getctl_int(TickitTerm *tt, TickitTermCtl ctl, int *value);
int tickit_term_setctl_int(TickitTerm *tt, TickitTermCtl ctl, int value);
int tickit_term_setctl_str(TickitTerm *tt, TickitTermCtl ctl, const char *value);

/*
 * String handling utilities
 */

typedef struct {
  size_t bytes;
  int    codepoints;
  int    graphemes;
  int    columns;
} TickitStringPos;

size_t tickit_string_count(const char *str, TickitStringPos *pos, const TickitStringPos *limit);
size_t tickit_string_countmore(const char *str, TickitStringPos *pos, const TickitStringPos *limit);

// Some convenient mutators for TickitStringPos structs

static inline void tickit_stringpos_zero(TickitStringPos *pos) {
  pos->bytes = pos->codepoints = pos->graphemes = pos->columns = 0;
}

static inline void tickit_stringpos_limit_bytes(TickitStringPos *pos, size_t bytes) {
  pos->codepoints = pos->graphemes = pos->columns = -1;
  pos->bytes = bytes;
}

static inline void tickit_stringpos_limit_codepoints(TickitStringPos *pos, int codepoints) {
  pos->bytes = pos->graphemes = pos->columns = -1;
  pos->codepoints = codepoints;
}

static inline void tickit_stringpos_limit_graphemes(TickitStringPos *pos, int graphemes) {
  pos->bytes = pos->codepoints = pos->columns = -1;
  pos->graphemes = graphemes;
}

static inline void tickit_stringpos_limit_columns(TickitStringPos *pos, int columns) {
  pos->bytes = pos->codepoints = pos->graphemes = -1;
  pos->columns = columns;
}

#endif
