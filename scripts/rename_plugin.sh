#!/bin/bash
# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
#
# Renames the 'vendor' placeholder used across the plugin skeleton to the given name.
#
# Usage: scripts/rename_plugin.sh <name>
#
# <name> must be a lower case C++ identifier (e.g. 'acme'). The following replacements are applied to file contents,
# file names and directory names under apps/, include/ and lib/ (plus CMakeLists.txt and .gitlab-ci.yml):
#   vendor -> <name>,  Vendor -> <Name>,  VENDOR -> <NAME>
# Comments are renamed as well, so review them afterwards.

set -euo pipefail

if [ $# -ne 1 ] || ! [[ "$1" =~ ^[a-z][a-z0-9_]*$ ]]; then
  echo "Usage: $0 <name>   (lower case identifier, e.g. acme)" >&2
  exit 1
fi

lower="$1"
capital="${lower^}"
upper="${lower^^}"

cd "$(dirname "$0")/.."

dirs=(apps include lib)

# Replace the file contents.
grep -rlI -e vendor -e Vendor -e VENDOR "${dirs[@]}" CMakeLists.txt .gitlab-ci.yml | while read -r file; do
  sed -i -e "s/vendor/${lower}/g" -e "s/Vendor/${capital}/g" -e "s/VENDOR/${upper}/g" "$file"
done

# Rename the files and directories, deepest first.
find "${dirs[@]}" -depth -name '*[Vv]endor*' | while read -r path; do
  base="$(basename "$path")"
  new_base="$(echo "$base" | sed -e "s/vendor/${lower}/g" -e "s/Vendor/${capital}/g")"
  mv "$path" "$(dirname "$path")/${new_base}"
done

echo "Renamed the plugin placeholder 'vendor' to '${lower}'."
