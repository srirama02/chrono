$pyver = python --version
if($pyver -match '3.9' -or $pyver -match '3.10'){
    Get-ChildItem -Directory "C:\Users\builder\miniconda3\envs\build-env\pkgs\numpy-base*" | Rename-Item -NewName numpy-base
}
else{
    Get-ChildItem -Directory "C:\Users\builder\miniconda3\envs\build-env\pkgs\numpy*" | Rename-Item -NewName numpy-base
}
