# Simracing / Hardware Tooling Probe Track

**Status:** Long-term validation track

## 1. Motivation

The user has a long-term DIY simracing project idea: force-feedback wheel base, hydro handbrake, load-cell pedals, H/sequential shifter, interchangeable wheels, and calibration/config tooling.

Veyr should not jump directly into firmware. First use small host-side tools to validate binary protocols, unit-safe data, config, and native APIs.

## 2. HW-01 — Telemetry parser

Forces:

- binary I/O;
- byte buffers;
- endianness;
- fixed-size arrays;
- error handling.

## 3. HW-02 — Pedal calibration CLI

Forces:

- config files;
- numeric conversions;
- newtypes for units;
- validation diagnostics;
- persistence.

## 4. HW-03 — Force-feedback profile tool

Forces:

- structured config;
- curves/tables;
- binary export;
- safe numeric ranges.

## 5. HW-04 — Device profile manager

Forces:

- platform device enumeration later;
- native APIs;
- handles;
- optional data;
- stable file formats.

## 6. HW-05 — HID/serial probe

Forces:

- OS-specific APIs;
- async/blocking I/O decision pressure;
- resource cleanup;
- thread-affine handles.

## 7. Future embedded direction

Embedded/microcontroller support is a future research track. It should be gated by:

- no-std support;
- target abstraction;
- allocator optionality;
- panic/abort policy;
- strict unsafe FFI boundary.

## Project cards

Smaller validation projects for this track are split into dedicated cards under:

```text
docs/design/tracks/projects/
```

Use those cards when planning milestones so the track develops through small concrete programs rather than one oversized project.
