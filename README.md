# 307lib Documentation

You can find the github pages site [here.](https://radj307.github.io/307lib/html/index.html)

## Generating Documentation
1. Clone the repository & `cd` to it.  
`git clone https://github.com/radj307/307lib && cd 307lib`
2. Clone the `gh-pages` branch as a subdirectory of the main repository, and clone all submodules.  
`git clone --single-branch --branch gh-pages https://github.com/radj307/307lib gh-pages && cd gh-pages`
3. Initialize & clone submodules.  
`git submodule init && git submodule update`
4. Run doxygen
`doxygen ./Doxyfile`


To execute all of the above steps, you can use this one-liner:  
`git clone https://github.com/radj307/307lib && cd 307lib && git clone --single-branch --branch gh-pages https://github.com/radj307/307lib gh-pages && cd gh-pages && git submodule init && git submodule update && doxygen ./Doxyfile`  
