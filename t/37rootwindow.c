#include "tickit.h"
#include "tickit-window.h"
#include "taplib.h"
#include "taplib-mockterm.h"

int main(int argc, char *argv[])
{
  TickitTerm *tt = make_term(25, 80);
  TickitWindow *root = tickit_window_new_root(tt);

  // Basics
  {
    is_int(tickit_window_top(root), 0, "root tickit_window_top");
    is_int(tickit_window_left(root), 0, "root tickit_window_left");

    is_int(tickit_window_abs_top(root), 0, "root tickit_window_abs_top");
    is_int(tickit_window_abs_left(root), 0, "root tickit_window_abs_left");
    
    is_int(tickit_window_lines(root), 25, "root tickit_window_lines");
    is_int(tickit_window_cols(root), 80, "root tickit_window_cols");

    is_int(tickit_window_bottom(root), 25, "root tickit_window_bottom");
    is_int(tickit_window_right(root), 80, "root tickit_window_right");

    ok(tickit_window_parent(root) == NULL, "root tickit_window_parent");
    ok(tickit_window_root(root) == root, "root tickit_window_root");
  }

  return exit_status();
}
