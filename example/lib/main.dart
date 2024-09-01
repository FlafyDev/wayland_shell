import 'package:flutter/material.dart';
import 'package:wayland_shell/wayland_shell.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();

  await WaylandShell.init(
    namespace: "wayland_shell_example",
    layer: WaylandShellLayer.top,
    anchors: [
      WaylandShellEdge.top,
      WaylandShellEdge.left,
      WaylandShellEdge.right,
    ],
    exclusiveZone: 50,
    height: 250,
  );

  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  bool e = true;

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        backgroundColor: Colors.black12,
        body: Column(
          children: [
            Container(
              width: double.infinity,
              height: 50,
              color: Colors.blue,
            ),
            Expanded(
              child: Center(
                child: InputRegion(
                  enabled: e,
                  child: Container(
                    padding: const EdgeInsets.all(16.0),
                    color: Colors.white.withOpacity(e ? 0.2 : 0),
                    child: TextButton(
                      child: const Text(
                        'Hello, World!',
                        style: TextStyle(
                          color: Colors.redAccent,
                          fontSize: 24,
                          // backgroundColor: Colors.greenAccent,
                        ),
                      ),
                      onPressed: () async {
                        print("Pressed");

                        setState(() {
                          e = false;
                        });
                        await Future.delayed(const Duration(seconds: 1));
                        setState(() {
                          e = true;

                          print("Reset");
                        });
                      },
                    ),
                  ),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
