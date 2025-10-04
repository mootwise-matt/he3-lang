#!/bin/bash

# He³ Build Name Update Script
# Usage: ./update_build_name.sh <new_build_name>

if [ $# -eq 0 ]; then
    echo "Usage: $0 <new_build_name>"
    echo "Current build name: Sirius"
    echo ""
    echo "Examples:"
    echo "  $0 Orion"
    echo "  $0 Vega"
    echo "  $0 Alpha"
    exit 1
fi

NEW_BUILD_NAME="$1"
CURRENT_BUILD_NAME="Sirius"

echo "Updating build name from '$CURRENT_BUILD_NAME' to '$NEW_BUILD_NAME'..."

# Update the build_info.h file
sed -i.bak "s/#define HE3_BUILD_NAME \"$CURRENT_BUILD_NAME\"/#define HE3_BUILD_NAME \"$NEW_BUILD_NAME\"/" src/shared/build_info.h

# Clean up backup file
rm -f src/shared/build_info.h.bak

echo "Build name updated to: $NEW_BUILD_NAME"
echo "Run 'make clean && make' to rebuild with the new name"
