#include <unordered_map>
#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>
#include <cstring>
#include <gtk-layer-shell/gtk-layer-shell.h>
#include "input_regions_manager.h"

void InputRegionsManager::apply_input_regions() {
  cairo_region_t *final_region = cairo_region_create();

  for (const auto& entry : input_regions) {
    cairo_region_t *region = cairo_region_create_rectangle(&entry.second);
    cairo_region_union(final_region, region);
    cairo_region_destroy(region);
  }

  gtk_widget_input_shape_combine_region(GTK_WIDGET (window), final_region);
  cairo_region_destroy(final_region);
}

InputRegionsManager::InputRegionsManager(GtkWindow* window) : window(window) {}

void InputRegionsManager::add(int id, int x, int y, int width, int height) {
  cairo_rectangle_int_t rect = { x, y, width, height };

  input_regions[id] = rect;

  apply_input_regions();
}

void InputRegionsManager::remove(int id) {
  input_regions.erase(id);

  apply_input_regions();
}

void InputRegionsManager::clear() {
  input_regions.clear();

  apply_input_regions();
}
