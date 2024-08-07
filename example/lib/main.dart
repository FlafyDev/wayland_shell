import 'package:flutter/material.dart';
import 'package:wayland_shell/wayland_shell.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  @override
  void initState() {
    WaylandShell.clearInputRegions();

    super.initState();
  }

  bool e = true;

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        backgroundColor: Colors.transparent,
        body: Center(
          child: InputRegion(
            enabled: e,
            child: Container(
              padding: const EdgeInsets.all(16.0),
              color: Colors.white.withOpacity(0.2),
              child: TextButton(
                child: const Text(
                  'Hello, World!',
                  style: TextStyle(
                    color: Colors.redAccent,
                    fontSize: 32.0,
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
                  });
                },
              ),
            ),
          ),
        ),
      ),
    );
  }
}
