#!/usr/bin/env python3
"""Fix imports in generated protobuf files to use relative imports

This script is automatically run during the build process, but can also be
run manually if needed.

Usage:
    python3 scripts/fix_proto_imports.py <file1> <file2> ...
"""

import re
import sys


def fix_imports(filename):
    """Fix imports in a protobuf file to use relative imports"""
    with open(filename, 'r') as f:
        content = f.read()
    
    # Replace absolute imports with relative imports for EIH proto files
    # Changes: import EIHCamera_pb2 -> from . import EIHCamera_pb2
    #          import EIHCameraAPI_pb2 -> from . import EIHCameraAPI_pb2
    content = re.sub(
        r'^import (EIHCamera(?:API)?_pb2)',
        r'from . import \1',
        content,
        flags=re.MULTILINE
    )
    
    with open(filename, 'w') as f:
        f.write(content)
    
    print(f"Fixed imports in {filename}")


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)
    
    for filename in sys.argv[1:]:
        try:
            fix_imports(filename)
        except Exception as e:
            print(f"Error fixing {filename}: {e}", file=sys.stderr)
            sys.exit(1)
