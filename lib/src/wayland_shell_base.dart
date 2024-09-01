import 'package:flutter/services.dart';
import 'package:flutter/widgets.dart';

class WaylandShell {
  static const _methodChannel = MethodChannel('wayland_shell');

  static Future<void> init({
    required String namespace,
    WaylandShellLayer layer = WaylandShellLayer.top,
    List<WaylandShellEdge> anchors = const [],
    EdgeInsets margins = EdgeInsets.zero,
    // WaylandShellEdge? exclusiveEdge,
    int exclusiveZone = 0,
    WaylandShellKeyboardMode keyboardMode = WaylandShellKeyboardMode.none,
    bool useInputRegions = false,
    int? width,
    int? height,
  }) async {
    if (width != null && height != null) {
      await setSize(width, height);
    } else if (width != null) {
      await setWidth(width);
    } else if (height != null) {
      await setHeight(height);
    }

    await _methodChannel.invokeMethod('init', {
      'namespace': namespace,
      'layer': layer.index,
      'anchors': anchors.map((e) => e.index).toList(),
      'margins': [
        margins.left.round(),
        margins.right.round(),
        margins.top.round(),
        margins.bottom.round(),
      ],
      // 'exclusiveEdge': exclusiveEdge?.index,
      'exclusiveZone': exclusiveZone,
      'keyboardMode': keyboardMode.index,
      'useInputRegions': useInputRegions,
    });
  }

  static Future<void> setNamespace(String namespace) async {
    await _methodChannel.invokeMethod('setNamespace', namespace);
  }

  static Future<void> setLayer(WaylandShellLayer layer) async {
    await _methodChannel.invokeMethod('setLayer', layer.index);
  }

  static Future<void> setAnchor(WaylandShellEdge edge, bool enabled) async {
    await _methodChannel.invokeMethod('setAnchor', [edge.index, enabled]);
  }

  static Future<void> setMargins(EdgeInsets margins) async {
    await _methodChannel.invokeMethod('setMargins', [
      margins.left.round(),
      margins.right.round(),
      margins.top.round(),
      margins.bottom.round(),
    ]);
  }

  // NOTE: blocked by https://github.com/wmww/gtk-layer-shell/pull/184
  // static Future<void> setExclusiveEdge(WaylandShellEdge edge) async {
  //   await _methodChannel.invokeMethod('setExclusiveEdge', edge.index);
  // }

  static Future<void> setExclusiveZone(int size) async {
    await _methodChannel.invokeMethod('setExclusiveZone', size);
  }

  static Future<void> setKeyboardMode(WaylandShellKeyboardMode mode) async {
    await _methodChannel.invokeMethod('setKeyboardMode', mode.index);
  }

  static Future<void> setSize(int width, int height) async {
    await _methodChannel.invokeMethod('setSize', [width, height]);
  }

  static Future<void> setWidth(int width) async {
    await _methodChannel.invokeMethod('setWidth', width);
  }

  static Future<void> setHeight(int height) async {
    await _methodChannel.invokeMethod('setHeight', height);
  }

  static Future<void> addInputRegion(int id, Rect rect) async {
    await _methodChannel.invokeMethod('addInputRegion', [
      id,
      rect.left.toInt(),
      rect.top.toInt(),
      rect.width.toInt(),
      rect.height.toInt(),
    ]);
  }

  static Future<void> removeInputRegion(int id) async {
    await _methodChannel.invokeMethod('removeInputRegion', id);
  }

  static Future<void> clearInputRegions() async {
    await _methodChannel.invokeMethod('clearInputRegions');
  }

  static Future<void> setInput(bool recieve) async {
    await _methodChannel.invokeMethod('set_input', {
      'recieve': recieve,
    });
  }
}

enum WaylandShellLayer {
  // NOTE: order matches enum in gtk-layer-shell.h
  background,
  bottom,
  top,
  overlay,
}

enum WaylandShellEdge {
  // NOTE: order matches enum in gtk-layer-shell.h
  left,
  right,
  top,
  bottom,
}

enum WaylandShellKeyboardMode {
  // NOTE: order matches enum in gtk-layer-shell.h
  none,
  exclusive,
  onDemand,
}
