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
  FlView* view;
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

  // INPUT REGIONS
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

  // LAYER SHELL
  } else if (strcmp(method, "init") == 0) {
    const gchar* namespace_ = fl_value_get_string(fl_value_lookup_string(args, "namespace"));
    int layer = fl_value_get_int(fl_value_lookup_string(args, "layer"));
    FlValue* anchors = fl_value_lookup_string(args, "anchors");
    size_t anchors_count = fl_value_get_length(anchors);
    FlValue* margins = fl_value_lookup_string(args, "margins");
    // int exclusive_edge = fl_value_get_int(fl_value_lookup_string(args, "exclusiveEdge"));
    int exclusive_zone = fl_value_get_int(fl_value_lookup_string(args, "exclusiveZone"));
    int keyboard_mode = fl_value_get_int(fl_value_lookup_string(args, "keyboardMode"));
    bool use_input_regions = fl_value_get_bool(fl_value_lookup_string(args, "useInputRegions"));

    if (!gtk_layer_is_layer_window(self->window)) {
      // initialize
      gtk_layer_init_for_window(self->window);
      gtk_widget_show(GTK_WIDGET(self->window));
    } else {
      // reset
      gtk_layer_set_anchor(self->window, GTK_LAYER_SHELL_EDGE_LEFT, FALSE);
      gtk_layer_set_anchor(self->window, GTK_LAYER_SHELL_EDGE_RIGHT, FALSE);
      gtk_layer_set_anchor(self->window, GTK_LAYER_SHELL_EDGE_TOP, FALSE);
      gtk_layer_set_anchor(self->window, GTK_LAYER_SHELL_EDGE_BOTTOM, FALSE);
    }

    gtk_layer_set_namespace(self->window, namespace_);
    gtk_layer_set_layer(self->window, static_cast<GtkLayerShellLayer>(layer));
    for (size_t i = 0; i < anchors_count; i++) {
      gtk_layer_set_anchor(self->window,
                           static_cast<GtkLayerShellEdge>(fl_value_get_int(fl_value_get_list_value(anchors, i))),
                           TRUE);
    }
    // gtk_layer_set_exclude_edge(self->window, static_cast<GtkLayerShellEdge>(exclusive_edge));
    gtk_layer_set_exclusive_zone(self->window, exclusive_zone);
    gtk_layer_set_keyboard_mode(self->window, static_cast<GtkLayerShellKeyboardMode>(keyboard_mode));

    int left = fl_value_get_int(fl_value_get_list_value(margins, 0));
    int right = fl_value_get_int(fl_value_get_list_value(margins, 1));
    int top = fl_value_get_int(fl_value_get_list_value(margins, 2));
    int bottom = fl_value_get_int(fl_value_get_list_value(margins, 3));

    gtk_layer_set_margin(self->window, GTK_LAYER_SHELL_EDGE_LEFT, left);
    gtk_layer_set_margin(self->window, GTK_LAYER_SHELL_EDGE_RIGHT, right);
    gtk_layer_set_margin(self->window, GTK_LAYER_SHELL_EDGE_TOP, top);
    gtk_layer_set_margin(self->window, GTK_LAYER_SHELL_EDGE_BOTTOM, bottom);

    if (use_input_regions) {
      self->input_regions_manager->clear();
    }

    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_null()));
  } else if (strcmp(method, "setNamespace") == 0) {
    const gchar* namespace_ = fl_value_get_string(args);

    gtk_layer_set_namespace(self->window, namespace_);

    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_null()));
  } else if (strcmp(method, "setLayer") == 0) {
    int layer = fl_value_get_int(args);

    gtk_layer_set_layer(self->window, static_cast<GtkLayerShellLayer>(layer));

    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_null()));
  } else if (strcmp(method, "setAnchor") == 0) {
    int edge = fl_value_get_int(fl_value_get_list_value(args, 0));
    bool enabled = fl_value_get_bool(fl_value_get_list_value(args, 1));

    gtk_layer_set_anchor(self->window, static_cast<GtkLayerShellEdge>(edge), enabled);

    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_null()));
  } else if (strcmp(method, "setKeyboardMode") == 0) {
    int mode = fl_value_get_int(args);

    gtk_layer_set_keyboard_mode(self->window, static_cast<GtkLayerShellKeyboardMode>(mode));

    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_null()));
  } else if (strcmp(method, "setExclusiveZone") == 0) {
    int size = fl_value_get_int(args);

    gtk_layer_set_exclusive_zone(self->window, size);

    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_null()));
  // } else if (strcmp(method, "setExclusiveEdge") == 0) {
  //   int edge = fl_value_get_int(args);

  //   gtk_layer_set_exclude_edge(self->window, edge);

  //   response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_null()));

  // RESIZE
  } else if (strcmp(method, "setSize") == 0) {
    int width = fl_value_get_int(fl_value_get_list_value(args, 0));
    int height = fl_value_get_int(fl_value_get_list_value(args, 1));

    gtk_widget_set_size_request(GTK_WIDGET(self->window), width, height);

    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_null()));
  } else if (strcmp(method, "setWidth") == 0) {
    int width = fl_value_get_int(args);

    gtk_widget_set_size_request(GTK_WIDGET(self->window), width, gtk_widget_get_allocated_height(GTK_WIDGET(self->window)));

    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_null()));
  } else if (strcmp(method, "setHeight") == 0) {
    int height = fl_value_get_int(args);

    gtk_widget_set_size_request(GTK_WIDGET(self->window), gtk_widget_get_allocated_width(GTK_WIDGET(self->window)), height);

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

  // workaround for Flutter >=3.21 <3.25(?)
  // see https://github.com/flutter/engine/pull/54575
  GtkWidget* gl_area = find_gl_area(GTK_WIDGET(view));
  if (gl_area != NULL) {
    gtk_gl_area_set_has_alpha(GTK_GL_AREA(gl_area), TRUE);
  }

  WaylandShellPlugin* plugin = WAYLAND_SHELL_PLUGIN(
      g_object_new(wayland_shell_plugin_get_type(), nullptr));
  plugin->window = window;
  plugin->view = view;
  plugin->input_regions_manager = new InputRegionsManager(window);

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
