# zpp
![CMake](https://github.com/AndrewShpagin/zpp/actions/workflows/cmake.yml/badge.svg)
The c++ interface to operate ZIP files. No libraries required. Just one header and on c file.

It is based on miniz - https://code.google.com/archive/p/miniz/ (completely free)

## Including into your project
Just add **miniz.c** and **zpp.h** into your project. Include **zpp.h** and use the classes defined there.

## How to use?
Look the **zipFileWriter** and **zipFileReader**, the interfaces are self-obvious. Examples of usage
1. Create zip from the folder
```cpp
zipFileWriter z("output_path.zip");
z.addFolder("./folder_to_zip/");
z.flush();
```
2. Add single file to ZIP
```cpp
zipFileWriter z("output_path.zip");
z.addFile("./some_path/filename.txt", "path_in_archive.txt");
z.flush();
``` 
3. Add string/raw data as the record in the ZIP file.
```cpp
zipFileWriter z("output_path.zip");
z.addString("The text to be ziped", "path_in_archive.txt");
z.flush();
```
4. Extract the whole archive
```cpp
zipFileReader r("path_to_archive.zip");
	r.extractAll("./output_path/");
```
5. Extract single file (the first one in the ZIP). It is helpful if the ZIP contains just one file.
```cpp
zipFileReader r("path_to_archive.zip");
r.extractFirstToFile("destination_path_for_single_file.txt");
```
6. List all files, extract only required files
```cpp
zipFileReader r("path_to_archive.zip");
auto list = r.getFilesList();
for (auto e = list.begin(); e != list.end(); e++) {
	std::cout << e->c_str() << "\n";
	std::filesystem::path p = "./extract_path/";
	p.append(e->c_str());
	r.extractToFile(e->c_str(), p.generic_u8string()); 
}
```
