import 'package:flutter/services.dart';

class WaylandShell {
  static const _methodChannel = MethodChannel('wayland_shell');

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

