#include "tickit-window.h"

#include <stdio.h>

/* TODO:
 * Event handling.
 * Focus handling.
 * The "later" system that lets us trigger actual changes.
 * Actual child management.
 * Deferred child / parent updating.
 */

#define ROOT_AS_WINDOW(root) (TickitWindow*)root
#define WINDOW_AS_ROOT(window) (TickitRootWindow*)window

struct TickitWindowCursorData {
  int line;
  int col;
  int shape;
  bool visible;
};

struct TickitWindow {
  TickitWindow *parent;
  TickitWindow *children;
  TickitWindow *next;
  TickitWindow *focused_child;
  TickitPen *pen;
  TickitRect rect;
  struct TickitWindowCursorData cursor;
  bool is_visible;
  bool is_focused;

  /* Callbacks */
  TickitWindowExposeFn *on_expose;
  void *on_expose_data;
  TickitWindowGeometryChangedFn *on_geometry_changed;
  void *on_geometry_changed_data;
};

struct TickitRootWindow {
  TickitWindow window;

  TickitTerm *term;
  TickitRectSet *damage;
  bool needs_expose;
  bool needs_restore;
  bool needs_later_processing;
};

static void _request_restore(TickitRootWindow *root);
static void _request_later_processing(TickitRootWindow *root);

static void init_window(TickitWindow *window, TickitWindow *parent, int top, int left, int lines, int cols)
{
  window->parent = parent;
  window->children = NULL;
  window->next = NULL;
  window->focused_child = NULL;
  window->pen = NULL;
  window->rect.top = top;
  window->rect.left = left;
  window->rect.lines = lines;
  window->rect.cols = cols;
  window->cursor.line = 0;
  window->cursor.col = 0;
  window->cursor.shape = TICKIT_TERM_CURSORSHAPE_BLOCK;
  window->cursor.visible = false;
  window->is_visible = false;
  window->is_focused = false;

  window->on_expose = NULL;
  window->on_expose_data = NULL;
  window->on_geometry_changed = NULL;
  window->on_geometry_changed_data = NULL;
}

static TickitWindow* new_window(TickitWindow *parent, int top, int left, int lines, int cols)
{
  TickitWindow *window = malloc(sizeof(TickitWindow));
  if(!window)
    return NULL;

  init_window(window, parent, top, left, lines, cols);

  return window;
}

TickitRootWindow* tickit_window_new_root(TickitTerm *term)
{
  int lines, cols;
  tickit_term_get_size(term, &lines, &cols);

  TickitRootWindow *root = malloc(sizeof(TickitRootWindow));
  if(!root)
    return NULL;

  init_window(ROOT_AS_WINDOW(root), NULL, 0, 0, lines, cols);

  root->term = term;
  root->needs_expose = false;
  root->needs_restore = false;
  root->needs_later_processing = false;

  root->damage = tickit_rectset_new();
  if(!root->damage) {
    tickit_window_destroy(ROOT_AS_WINDOW(root));
    return NULL;
  }
  return root;
}

TickitRootWindow *tickit_window_get_root(TickitWindow *window)
{
  TickitWindow *root = window;
  while(root->parent) {
    root = root->parent;
  }
  return WINDOW_AS_ROOT(root);
}

TickitWindow *tickit_window_new_subwindow(TickitWindow *parent, int top, int left, int lines, int cols)
{
  TickitWindow *window = new_window(parent, top, left, lines, cols);
  /* TODO: Add child to parent (end of child list). */
  return window;
}

TickitWindow *tickit_window_new_hidden_subwindow(TickitWindow *parent, int top, int left, int lines, int cols)
{
  TickitWindow *window = new_window(parent, top, left, lines, cols);
  /* TODO: Add child to parent (end of child list). */
  window->is_visible = false;
  return window;
}

TickitWindow *tickit_window_new_float(TickitWindow *parent, int top, int left, int lines, int cols)
{
  TickitWindow *window = new_window(parent, top, left, lines, cols);
  /* TODO: Add child to parent (front of child list). */
  return window;
}

TickitWindow *tickit_window_new_popup(TickitWindow *parent, int top, int left, int lines, int cols)
{
  TickitWindow *root = parent;
  while(root->parent) {
    top += root->rect.top;
    left += root->rect.left;
    root = root->parent;
  }
  TickitWindow *window = new_window(root, top, left, lines, cols);
  /* TODO: Add child to root (front of child list). */
  /* TODO: Steal input. */
  return window;
}

void tickit_window_destroy(TickitWindow *window)
{
  window->pen = NULL;

  window->on_expose = NULL;
  window->on_expose_data = NULL;
  window->on_geometry_changed = NULL;
  window->on_geometry_changed_data = NULL;

  /* TODO: Destroy children */
  /* TODO: Kill pending geometry changes */

  if(window->parent) {
    /* TODO: Remove from parent's children. */
  }

  /* Root cleanup */
  if(!window->parent) {
    TickitRootWindow *root = WINDOW_AS_ROOT(window);
    if(root->damage) {
      tickit_rectset_destroy(root->damage);
    }
  }
  free(window);
}

void tickit_window_raise(TickitWindow *window)
{
  /* TODO: Reorder child to be earlier. */
}

void tickit_window_raise_to_front(TickitWindow *window)
{
  /* TODO: Reorder child to be first. */
}

void tickit_window_lower(TickitWindow *window)
{
  /* TODO: Reorder child to be later. */
}

void tickit_window_lower_to_back(TickitWindow *window)
{
  /* TODO: Reorder child to be last. */
}

void tickit_window_show(TickitWindow *window)
{
  window->is_visible = true;
  if(window->parent) {
    /* TODO: Focused child stuff. */
  }
  tickit_window_expose(window, NULL);
}

void tickit_window_hide(TickitWindow *window)
{
  window->is_visible = false;

  if(window->parent) {
    /* TODO: Focused child stuff. */
    tickit_window_expose(window->parent, &window->rect);
  }
}

bool tickit_window_is_visible(TickitWindow *window)
{
  return window->is_visible;
}

int tickit_window_top(const TickitWindow *window)
{
  return window->rect.top;
}

int tickit_window_abs_top(const TickitWindow *window)
{
  int top = window->rect.top;
  TickitWindow* parent = window->parent;
  while(parent) {
    top += parent->rect.top;
    parent = parent->parent;
  }
  return top;
}

int tickit_window_left(const TickitWindow *window)
{
  return window->rect.left;
}

int tickit_window_abs_left(const TickitWindow *window)
{
  int left = window->rect.left;
  TickitWindow* parent = window->parent;
  while(parent) {
    left += parent->rect.left;
    parent = parent->parent;
  }
  return left;
}

int tickit_window_lines(const TickitWindow *window)
{
  return window->rect.lines;
}

int tickit_window_cols(const TickitWindow *window)
{
  return window->rect.cols;
}

void tickit_window_resize(TickitWindow *window, int lines, int cols)
{
  tickit_window_set_geometry(window, window->rect.top, window->rect.left, lines, cols);
}

void tickit_window_reposition(TickitWindow *window, int top, int left)
{
  tickit_window_set_geometry(window, top, left, window->rect.lines, window->rect.cols);
  if(window->is_focused) {
    _request_restore(tickit_window_get_root(window));
  }
}

void tickit_window_set_geometry(TickitWindow *window, int top, int left, int lines, int cols)
{
  if((window->rect.top != top) ||
     (window->rect.left != left) ||
     (window->rect.lines != lines) ||
     (window->rect.cols != cols))
  {
    window->rect.top = top;
    window->rect.left = left;
    window->rect.lines = lines;
    window->rect.cols = cols;

    if(window->on_geometry_changed) {
      window->on_geometry_changed(window, window->on_geometry_changed_data);
    }
  }
}

void tickit_window_set_on_geometry_changed(TickitWindow *window, TickitWindowGeometryChangedFn *fn, void *data)
{
  window->on_geometry_changed = fn;
  window->on_geometry_changed_data = data;
}

void tickit_window_set_pen(TickitWindow *window, TickitPen *pen)
{
  /* TODO: Refcounting the pen would be nice. Until then, we assume we don't own it. */
  window->pen = pen;
}

void tickit_window_expose(TickitWindow *window, const TickitRect *exposed)
{
  TickitRect damaged;
  if(exposed) {
    if(!tickit_rect_intersect(&damaged, &window->rect, exposed)) {
      return;
    }
  } else {
    damaged = window->rect;
  }

  if(!window->is_visible) {
    return;
  }

  if(window->parent) {
    tickit_rect_translate(&damaged, window->rect.top, window->rect.left);
    tickit_window_expose(window->parent, &damaged);
    return;
  }

  /* If we're here, then we're a root window. */
  TickitRootWindow *root = WINDOW_AS_ROOT(window);
  if(tickit_rectset_contains(root->damage, &damaged)) {
    return;
  }

  #ifdef DEBUG
  fprintf(stderr, "Damage root %d, %d, %d, %d\n", damaged.top, damaged.left, damaged.lines, damaged.cols);
  #endif

  tickit_rectset_add(root->damage, &damaged);

  root->needs_expose = true;
  _request_later_processing(root);
}

void tickit_window_set_on_expose(TickitWindow *window, TickitWindowExposeFn *fn, void *data)
{
  window->on_expose = fn;
  window->on_expose_data = data;
}

static void _do_expose(TickitWindow *window, const TickitRect *rect, TickitRenderBuffer *rb)
{
  if(window->pen) {
    tickit_renderbuffer_setpen(rb, window->pen);
  }

  for(TickitWindow* child = window->children; child; child = child->next) {
    if(!child->is_visible) {
      continue;
    }

    TickitRect exposed;
    if(tickit_rect_intersect(&exposed, rect, &child->rect)) {
      tickit_renderbuffer_save(rb);

      tickit_renderbuffer_clip(rb, &exposed);
      tickit_renderbuffer_translate(rb, child->rect.top, child->rect.left);
      tickit_rect_translate(&exposed, -child->rect.top, -child->rect.left);
      _do_expose(child, &exposed, rb);

      tickit_renderbuffer_restore(rb);
    }

    tickit_renderbuffer_mask(rb, &child->rect);
  }

  if(window->on_expose) {
    window->on_expose(window, rect, rb, window->on_expose_data);
  }
}

static void _request_restore(TickitRootWindow *root)
{
  root->needs_restore = true;
  _request_later_processing(root);
}

static void _request_later_processing(TickitRootWindow *root)
{
  root->needs_later_processing = true;
}

static void _do_restore(TickitRootWindow *root)
{
  TickitWindow *window = ROOT_AS_WINDOW(root);
  while(window) {
    if(!window->focused_child) {
      break;
    }
    if(!window->focused_child->is_visible) {
      break;
    }
    window = window->focused_child;
  }

  if(window && window->is_focused && window->cursor.visible) {
    /* TODO finish the visibility check here. */
    tickit_term_setctl_int(root->term, TICKIT_TERMCTL_CURSORVIS, 1);
    int cursor_line = window->cursor.line + tickit_window_abs_top(window);
    int cursor_col = window->cursor.col + tickit_window_abs_left(window);
    tickit_term_goto(root->term, cursor_line, cursor_col);
    tickit_term_setctl_int(root->term, TICKIT_TERMCTL_CURSORSHAPE, window->cursor.shape);
  } else {
    tickit_term_setctl_int(root->term, TICKIT_TERMCTL_CURSORVIS, 0);
  }

  tickit_term_flush(root->term);
}

static void _do_later_processing(TickitRootWindow *root)
{
  root->needs_later_processing = false;

  /* TODO: Do deferred geometry changes. */

  if(root->needs_expose) {
    root->needs_expose = false;

    TickitWindow *root_window = ROOT_AS_WINDOW(root);
    TickitRenderBuffer *rb = tickit_renderbuffer_new(root_window->rect.lines, root_window->rect.cols);

    int damage_count = tickit_rectset_rects(root->damage);
    TickitRect *rects = alloca(damage_count * sizeof(TickitRect));
    tickit_rectset_get_rects(root->damage, rects, damage_count);

    tickit_rectset_clear(root->damage);

    for(int i = 0; i < damage_count; i++) {
      TickitRect *rect = &rects[i];
      tickit_renderbuffer_save(rb);
      tickit_renderbuffer_clip(rb, rect);
      _do_expose(root_window, rect, rb);
      tickit_renderbuffer_restore(rb);
    }

    tickit_renderbuffer_flush_to_term(rb, root->term);
    tickit_renderbuffer_destroy(rb);

    root->needs_restore = true;
  }

  if(root->needs_restore) {
    root->needs_restore = false;
    _do_restore(root);
  }
}
