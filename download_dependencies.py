import os
import shutil
import subprocess
from git import Repo

def run_command(command, cwd=None):
    """Helper function to run a command and yield output line by line."""
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=cwd, universal_newlines=True)
    for line in process.stdout:
        yield line.strip()
    process.stdout.close()
    return_code = process.wait()
    if return_code != 0:
        raise subprocess.CalledProcessError(return_code, command)

def clone_and_copy_folder(repo_url, folder_name, dest_path, tag_name=None):
    # Create a temporary directory to clone the repository
    temp_dir = "temp_repo"
    
    try:
        # Clone the repository
        print(f"Cloning repository from {repo_url}...")

        if tag_name:
            print(f"Checking out tag {tag_name}...")
            command = ['git', 'clone', '--branch', tag_name, '--single-branch', repo_url, temp_dir]
        else:
            command = ['git', 'clone', repo_url, temp_dir]
        
        for output in run_command(command):
            print(output)

        # Define the source and destination paths
        src_path = os.path.join(temp_dir, folder_name)
        dst_path = dest_path

        # Check if the source folder exists in the repository
        if not os.path.exists(src_path):
            print(f"The folder '{folder_name}' does not exist in the repository.")
            return
        
        # Copy the folder to the destination path
        print(f"Copying folder from {src_path} to {dst_path}...")
        shutil.copytree(src_path, dst_path)
        print(f"Folder copied successfully to {dst_path}.")
    except subprocess.CalledProcessError as e:
        print(f"Error: Command '{e.cmd}' returned non-zero exit status {e.returncode}.")
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        # Remove the temporary directory
        print(f"Removing temporary directory {temp_dir}...")
        shutil.rmtree(temp_dir, ignore_errors=True)
        print(f"Temporary directory {temp_dir} removed.")

# Example usage
repo_url = "https://github.com/MADEAPPS/newton-dynamics.git"
folder_name = "newton-4.00"
dest_path = "external/newton-4.00"

tag_name = "v4.02"  # specify the tag name

clone_and_copy_folder(repo_url, folder_name, dest_path, tag_name)
