# DUW Collector

C++ application for monitoring Polish passport office queue data.

## Build

```bash
mkdir build
cd build
cmake ..
make
```

## Run

```bash
./duw-collector
```

## Environment Variables

- `MODE`: Set to "polling" for continuous monitoring
- `POLLING_RATE_SECONDS`: Polling interval (default: 5)
- `DB_PATH`: Database file path (default: "duw_data.db")