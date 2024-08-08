import 'dart:math';

import 'package:flutter/material.dart';
import 'package:wayland_shell/src/wayland_shell_base.dart';
import 'package:wayland_shell/src/global_rect/global_rect.dart';

class InputRegion extends StatefulWidget {
  const InputRegion({
    required this.child,
    this.enabled,
    super.key,
  });

  final Widget child;
  final bool? enabled;

  @override
  State<InputRegion> createState() => _InputRegionState();
}

class _InputRegionState extends State<InputRegion> {
  late final int _id;
  Rect? _lastRect;

  @override
  void initState() {
    _id = (DateTime.now().millisecondsSinceEpoch & 0x7fff << 16) |
        Random().nextInt(0xffff);

    super.initState();
  }

  @override
  void dispose() {
    WaylandShell.removeInputRegion(_id);
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    if (widget.enabled != false) {
      if (_lastRect != null) {
        WaylandShell.addInputRegion(_id, _lastRect!);
      }
    } else {
      WaylandShell.removeInputRegion(_id);
    }

    return GlobalRect(
      onChange: (rect) {
        _lastRect = rect;
        if (widget.enabled != false) {
          WaylandShell.addInputRegion(_id, rect);
        }
      },
      child: widget.child,
    );
  }
}
