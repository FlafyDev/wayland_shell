import 'package:flutter_test/flutter_test.dart';
import 'package:wayland_shell/wayland_shell.dart';
import 'package:wayland_shell/wayland_shell_platform_interface.dart';
import 'package:wayland_shell/wayland_shell_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockWaylandShellPlatform
    with MockPlatformInterfaceMixin
    implements WaylandShellPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final WaylandShellPlatform initialPlatform = WaylandShellPlatform.instance;

  test('$MethodChannelWaylandShell is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelWaylandShell>());
  });

  test('getPlatformVersion', () async {
    WaylandShell waylandShellPlugin = WaylandShell();
    MockWaylandShellPlatform fakePlatform = MockWaylandShellPlatform();
    WaylandShellPlatform.instance = fakePlatform;

    expect(await waylandShellPlugin.getPlatformVersion(), '42');
  });
}
