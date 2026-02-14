set -e # Exit on error

PROJECT_ROOT="$(pwd)"           # Set the project root to the current directory
BUILD_DIR="$PROJECT_ROOT/build" # Define the build directory
EXECUTABLE="Desktop"

# Create the build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Navigate to the build directory
cd "$BUILD_DIR"

# Run CMake from the build directory, using the parent folder as the source
# Set the build type to Debug and enable debug symbols
echo "Running CMake..."
cmake -G Ninja -S "$PROJECT_ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Optionally, copy compile_commands.json to the project root for clangd
if [[ -f "$BUILD_DIR/compile_commands.json" ]]; then
  cp "$BUILD_DIR/compile_commands.json" "$PROJECT_ROOT/"
  echo "Copied compile_commands.json to project root."
fi

# Run Ninja to build the project
echo "Building project..."
ninja

# Check if gdb is installed
if ! command -v gdb &>/dev/null; then
  echo "Error: gdb is not installed. Please install it."
  exit 1
fi

# Check if the executable exists
if [[ ! -f "$BUILD_DIR/$EXECUTABLE" ]]; then
  echo "Error: Executable '$EXECUTABLE' not found!"
  exit 1
fi

echo "Starting GDB..."
gdb "$BUILD_DIR/$EXECUTABLE"
