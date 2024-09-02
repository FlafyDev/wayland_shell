import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart';

class GlobalRect extends SingleChildRenderObjectWidget {
  GlobalRect({
    super.key,
    required super.child,
    this.painter,
    this.onChange,
  });

  final OffsetPainter? painter;
  final void Function(Rect)? onChange;
  Rect? _lastRect;

  @override
  RenderObject createRenderObject(BuildContext context) {
    return RenderGlobalRect(
      painter: painter,
      reportSize: _reportSize,
    );
  }

  @override
  void updateRenderObject(BuildContext context, RenderGlobalRect renderObject) {
    renderObject
      ..painter = painter
      ..reportSize = _reportSize
      ..markNeedsPaint();
  }

  void _reportSize(Rect rect) {
    onChange?.call(rect);
  }
}

class RenderGlobalRect extends RenderBox
    with RenderObjectWithChildMixin<RenderBox> {
  RenderGlobalRect({this.painter, this.reportSize}) : super();
  OffsetPainter? painter;
  void Function(Rect)? reportSize;
  Rect? _lastRect;

  @override
  void performLayout() {
    final child = this.child;
    if (child != null) {
      child.layout(constraints, parentUsesSize: true);
      size = child.size;
    } else {
      size = constraints.smallest;
    }
  }

  @override
  void paint(PaintingContext context, Offset offset) {
    final globalRect = localToGlobal(Offset.zero) & size;
    painter?.paint(context.canvas, offset, globalRect);
    if (globalRect != _lastRect) {
      _lastRect = globalRect;
      reportSize?.call(globalRect);
    }
    if (child != null) {
      context.paintChild(child!, offset);
    }
  }

  @override
  bool hitTestChildren(BoxHitTestResult result, {required Offset position}) {
    return child?.hitTest(result, position: position) == true;
  }
}

abstract class OffsetPainter {
  void paint(Canvas canvas, Offset offset, Rect globalRect);
}
