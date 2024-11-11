# Examples

This folder contains application examples that demonstrate how to use the OSI utilities library.
After build, the executables can be found in the examples folder in the build directory.

### convert_osi2mcap

This example demonstrates how to convert an OSI native binary trace file to an MCAP file.
Simply pass a .osi file and a .mcap destination file name as arguments to the executable.

```bash
./convert_osi2mcap <input_osi_file> <output_mcap_file>
```

### example_mcap_reader

This example demonstrates how to read an MCAP file into your application.
It simply prints the message types and timestamps of the OSI mcap file.
You can try it with an example mcap file generated with `example_mcap_writer` (see following section) with

```bash
./example_mcap_reader example_mcap.mcap
```

### example_mcap_writer

This example demonstrates how to write OSI data to an MCAP file from your application.
As an example, a SensorView message with one moving object is created and written to the MCAP file.
It creates the example file `example_mcap.mcap` in the temp directory of your operating system, on Linx this is `/tmp`.

```bash
./example_mcap_writer
```

### example_native_binary_reader

This example demonstrates how to read a native binary trace file into your application.
It simply prints the message types and timestamps of the OSI trace file.
You can try it with an example osi trace file generated with `example_native_binary_writer` (see following section) with

```bash
./example_native_binary_reader /tmp/example_native_binary_writer.osi --type SensorView
```

### example_native_binary_writer

This example demonstrates how to write OSI data to a native binary file from your application.
As an example, a SensorView message with one moving object is created and written to the OSI trace file.
It creates the example file `example_native_binary_writer.osi` in the temp directory of your operating system, on Linx this is `/tmp`.

```bash
./example_native_binary_writer
```

### example_txth_reader

This example demonstrates how to read a human-readable txth trace file into your application.
It simply prints the message types and timestamps of the OSI trace file.
You can try it with an example osi trace file generated with `example_txth_writer` (see following section) with

```bash
./example_native_binary_reader /tmp/example_txth_writer.txth --type SensorView
```

### example_txth_writer

This example demonstrates how to write OSI data to a human-readable txth file from your application.
As an example, a SensorView message with one moving object is created and written to the txth file.
It creates the example file `example_txth_writer.txth` in the temp directory of your operating system, on Linx this is `/tmp`.

```bash
./example_txth_writer
```
