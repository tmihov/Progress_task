# ProgressUploader

Uploads a file to MOVEit Transfer using the REST API. Tested on Fedora 42.

## Dependencies

libcurl

nlohmann_json

## Building

``` bash
cmake -S . -B ./build
cmake --build ./build
```

## Usage

``` bash
ProgressUploader <username> <password> <file>
```
