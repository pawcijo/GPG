import subprocess
import os

# Function to run a command and print its output in real-time
def run_command(command, cwd=None):
    process = subprocess.Popen(command, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    for line in iter(process.stdout.readline, ''):
        print(line, end='')  # Print stdout line by line
    for line in iter(process.stderr.readline, ''):
        print(line, end='')  # Print stderr line by line
    process.stdout.close()
    process.stderr.close()
    return process.wait()

# Define the relative paths for the build and source directories
build_dir = "external/newton-4.00/sdk/build"
source_dir = "../"

# Ensure the build directory exists
os.makedirs(build_dir, exist_ok=True)

# Run the cmake command in the build directory
cmake_result = run_command(["cmake", source_dir], cwd=build_dir)

# Check the cmake result
if cmake_result == 0:
    print("CMake configuration successful")
    
    # Run the make command in the build directory
    make_result = run_command(["make","-j","4"], cwd=build_dir)
    
    # Check the make result
    if make_result == 0:
        print("Make build successful")
        
        # Run the make install command in the build directory
        make_install_result = run_command(["sudo", "make", "install"], cwd=build_dir)
        
        # Check the make install result
        if make_install_result == 0:
            print("Make install successful")
        else:
            print("Make install failed")
    else:
        print("Make build failed")
else:
    print("CMake configuration failed")
