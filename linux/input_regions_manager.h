#include <unordered_map>
#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>
#include <cstring>
#include <gtk-layer-shell/gtk-layer-shell.h>

class InputRegionsManager {
private:
  std::unordered_map<int, cairo_rectangle_int_t> input_regions = {};
  GtkWindow* window;

  void apply_input_regions();

public:
  InputRegionsManager(GtkWindow* window);

  void add(int id, int x, int y, int width, int height);

  void remove(int id);
  
  void clear();
};
