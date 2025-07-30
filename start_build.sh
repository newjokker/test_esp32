#!/bin/bash

# 清理并重新上传
pio run --target clean
pio run --target upload
pio run --target uploadfs --upload-port /dev/cu.wchusbserial5A7A0249031


