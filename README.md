# libnymea-networkmanager

libnymea-networkmanager is the Qt/C++ helper library that lets nymea-based
systems talk to the Linux NetworkManager service over DBus. It provides
wrappers for wired and wireless devices, exposes network states and settings,
and can optionally spin up a Bluetooth Low Energy provisioning server so
headless appliances can be configured remotely.

## Features

- High-level API for tracking NetworkManager state, devices, and connections.
- Helpers for managing wired and Wi-Fi profiles, IP configuration, and DHCP.
- Optional BLE server that exposes provisioning services for remote setup.
- Designed to integrate with nymea daemons and other Qt applications.

## Documentation

The `docs/` folder contains the QDoc setup used to generate the API reference.
To build it locally run `qdoc docs/config.qdocconf` inside the repository root.

## License

libnymea-networkmanager is licensed under the GNU LGPL-3.0-or-later.
See `LICENSE.LGPL3` for the full text. The copyright is held by
nymea GmbH (2013‑2024) and chargebyte austria GmbH (2024‑2025).
