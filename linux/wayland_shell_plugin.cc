#include "include/wayland_shell/wayland_shell_plugin.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>

#include <cstring>

#include <gtk-layer-shell/gtk-layer-shell.h>
#include "wayland_shell_plugin_private.h"
#include "input_regions_manager.h"

#define WAYLAND_SHELL_PLUGIN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), wayland_shell_plugin_get_type(), \
                              WaylandShellPlugin))

struct _WaylandShellPlugin {
  GObject parent_instance;
  GtkWindow* window;
  InputRegionsManager* input_regions_manager;
};

G_DEFINE_TYPE(WaylandShellPlugin, wayland_shell_plugin, g_object_get_type())

// Called when a method call is received from Flutter.
static void wayland_shell_plugin_handle_method_call(
    WaylandShellPlugin* self,
    FlMethodCall* method_call) {
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar* method = fl_method_call_get_name(method_call);
  FlValue *args = fl_method_call_get_args(method_call);

  if (strcmp(method, "addInputRegion") == 0) {
    int id = fl_value_get_int(fl_value_get_list_value(args, 0));
    int x = fl_value_get_int(fl_value_get_list_value(args, 1));
    int y = fl_value_get_int(fl_value_get_list_value(args, 2));
    int width = fl_value_get_int(fl_value_get_list_value(args, 3));
    int height = fl_value_get_int(fl_value_get_list_value(args, 4));
    self->input_regions_manager->add(id, x, y, width, height);

    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_null()));
  } else if (strcmp(method, "removeInputRegion") == 0) {
    int id = fl_value_get_int(args);
    self->input_regions_manager->remove(id);

    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_null()));
  } else if (strcmp(method, "clearInputRegions") == 0) {
    self->input_regions_manager->clear();

    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_null()));
  } else {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  fl_method_call_respond(method_call, response, nullptr);
}

static void wayland_shell_plugin_dispose(GObject* object) {
  G_OBJECT_CLASS(wayland_shell_plugin_parent_class)->dispose(object);
  delete WAYLAND_SHELL_PLUGIN(object)->input_regions_manager;
}

static void wayland_shell_plugin_class_init(WaylandShellPluginClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = wayland_shell_plugin_dispose;
}

static void wayland_shell_plugin_init(WaylandShellPlugin* self) { }

static void method_call_cb(FlMethodChannel* channel, FlMethodCall* method_call,
                           gpointer user_data) {
  WaylandShellPlugin* plugin = WAYLAND_SHELL_PLUGIN(user_data);
  wayland_shell_plugin_handle_method_call(plugin, method_call);
}

GtkWidget* find_gl_area(GtkWidget* widget) {
  if (GTK_IS_GL_AREA(widget)) {
    return widget;
  }

  if (GTK_IS_CONTAINER(widget)) {
    GList* children = gtk_container_get_children(GTK_CONTAINER(widget));
    for (GList* iter = children; iter != NULL; iter = g_list_next(iter)) {
      GtkWidget* child = GTK_WIDGET(iter->data);
      GtkWidget* gl_area = find_gl_area(child);
      if (gl_area != NULL) {
        g_list_free(children);
        return gl_area;
      }
    }
    g_list_free(children);
  }

  return NULL;
}

void wayland_shell_plugin_register_with_registrar(FlPluginRegistrar* registrar) {
  FlView* view = fl_plugin_registrar_get_view(registrar);
  GtkWindow* window = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view)));

  GdkScreen* screen;
  GdkVisual* visual;
  gtk_widget_set_app_paintable(GTK_WIDGET(window), TRUE);
  screen = gdk_screen_get_default();
  visual = gdk_screen_get_rgba_visual(screen);
  if (visual != NULL && gdk_screen_is_composited(screen)) {
    gtk_widget_set_visual(GTK_WIDGET(window), visual);
  }

  GtkWidget *current_titlebar = gtk_window_get_titlebar(GTK_WINDOW(window));
  if (current_titlebar) {
    gtk_widget_destroy(current_titlebar); // Destroy the header bar
    // gtk_window_set_titlebar(window, NULL); // Clear the titlebar reference in the window
  }

  gtk_widget_hide(GTK_WIDGET(window));

  gtk_layer_init_for_window(window);
  gtk_layer_set_namespace(window, "wayland_shell");
  gtk_layer_set_keyboard_mode(window, GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
  gtk_layer_set_layer(window, GTK_LAYER_SHELL_LAYER_TOP);
  gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_TOP, TRUE);
  gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
  gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
  gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
  // gtk_layer_set_exclusive_edge(window, GTK_LAYER_SHELL_EDGE_BOTTOM);

  gtk_widget_show(GTK_WIDGET(window));

  GtkWidget* gl_area = find_gl_area(GTK_WIDGET(view));
  if (gl_area != NULL) {
    gtk_gl_area_set_has_alpha(GTK_GL_AREA(gl_area), TRUE);
  }

  WaylandShellPlugin* plugin = WAYLAND_SHELL_PLUGIN(
      g_object_new(wayland_shell_plugin_get_type(), nullptr));
  plugin->window = window;
  plugin->input_regions_manager = new InputRegionsManager(window);

  plugin->input_regions_manager->clear();

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            "wayland_shell",
                            FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(channel, method_call_cb,
                                            g_object_ref(plugin),
                                            g_object_unref);

  g_object_unref(plugin);
}
