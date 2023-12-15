# Define the path to clang-format and the source directory
$clangFormatPath = "clang-format"
$srcDirectory = "src"

# Get all .cpp, .hpp, and .h files in the source directory and its subdirectories
$files = Get-ChildItem -Path $srcDirectory -Include *.cpp,*.hpp,*.h -Recurse -File

# Format each file
foreach ($file in $files) {
    & $clangFormatPath -style=file -i $file.FullName
}
