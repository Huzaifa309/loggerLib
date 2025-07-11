#!/bin/bash

# Script to copy the built library to the distribution lib folder

echo "Copying built library to distribution folder..."

# Create lib directory if it doesn't exist
mkdir -p lib

# Copy the built library
if [ -f "build/lib/libloggerlib.a" ]; then
    cp build/lib/libloggerlib.a lib/
    echo "✓ Successfully copied libloggerlib.a to lib/"
else
    echo "✗ Error: build/lib/libloggerlib.a not found"
    echo "Please build the project first:"
    echo "  mkdir -p build && cd build"
    echo "  cmake .. && make"
    exit 1
fi

echo "Library is ready for distribution!" 