// zpp.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <functional>
#include <iostream>
#include <filesystem>

#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"


/// The simple c++ wrapper to create zip archives as simply as possible. Just create object, add items, flush at the end.  
class zipFileWriter {
	mz_zip_archive ar;
	bool errors;
public:
	zipFileWriter(const std::string& destArchiveName);
	~zipFileWriter();
	/// add the file to archive
	void addFile(const std::string& filename, const std::string& nameInArchive);
	/// add the string to archive
	void addString(const std::string& string, const std::string& nameInArchive);
	/// add the raw data
	void addData(void* data, int Length, const std::string& nameInArchive);
	/// add all files from the folder
	void addFolder(const std::string& path);
	/// write the archive. You should not add anything after this command.
	void flush();
	/// returns true if all operations are successful
	bool successful();
};

/// The simple c++ interface to extract ZIP files
class zipFileReader {
	mz_zip_archive ar;
	bool errors;
	int getIndex(const std::string& nameInArchive) {
		return mz_zip_reader_locate_file(&ar, nameInArchive.c_str(), nullptr, 0);
	}
	void createPathForFile(const std::string&);
public:
	/// Open the archive
	zipFileReader(const std::string& archiveName);
	~zipFileReader();

	/// Get list of files in the archive
	std::vector<std::string> getFilesList();
	/// Extract one file from the archive to the destination folder
	void extractToFolder(const std::string& nameInArchive, const std::string& destFolder);
	/// Extract one file from the archive to file, name may be different from the filename in the archive
	void extractToFile(const std::string& nameInArchive, const std::string& destFilename);
	/// Extract al files to the folder
	void extractAll(const std::string& destFolder);
	/// Extract the first file in the archive to the destination filename. It is useful if you have just one file in the archive.
	void extractFirstToFile(const std::string& destFilename);
};

inline zipFileWriter::zipFileWriter(const std::string& destArchiveName) {
	errors = false;
	try{
		std::filesystem::remove(destArchiveName);
		std::memset(&ar, 0, sizeof(ar));
		errors = true;
		errors = !mz_zip_writer_init_file(&ar, destArchiveName.c_str(), 0);
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
	}
}

inline zipFileWriter::~zipFileWriter() {
	flush();
}

inline void zipFileWriter::addFile(const std::string& filename, const std::string& nameInArchive) {
	if (!errors) {
		errors |= !mz_zip_writer_add_file(&ar, nameInArchive.c_str(), filename.c_str(), nullptr,
		                                 0, MZ_BEST_COMPRESSION);
	}
}

inline void zipFileWriter::addString(const std::string& string, const std::string& nameInArchive) {
	errors |= mz_zip_writer_add_mem(&ar, nameInArchive.c_str(), string.c_str(), string.length(), MZ_BEST_COMPRESSION);
}

inline void zipFileWriter::addData(void* data, int Length, const std::string& nameInArchive) {
	errors |= mz_zip_writer_add_mem(&ar, nameInArchive.c_str(), data, Length, MZ_BEST_COMPRESSION);
}

inline void zipFileWriter::addFolder(const std::string& path) {
	int L = path.length();
	try {
		std::filesystem::path p = path;
		for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
			if (entry.is_regular_file()) {
				addFile(entry.path().generic_u8string(), std::filesystem::relative(entry, p).generic_u8string());
			}
		}
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
	}
}

inline void zipFileWriter::flush() {
	errors |= mz_zip_writer_finalize_archive(&ar);
	errors |= mz_zip_writer_end(&ar);
}

inline bool zipFileWriter::successful() {
	return errors;
}

inline void zipFileReader::createPathForFile(const std::string& destFilename) {
	try {
		std::filesystem::path f = destFilename;
		f.remove_filename();
		std::filesystem::create_directories(f);
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
	}
}

inline zipFileReader::zipFileReader(const std::string& archiveName) {
	std::memset(&ar, 0, sizeof(ar));
	errors = !mz_zip_reader_init_file(&ar, archiveName.c_str(), 0);
}

inline zipFileReader::~zipFileReader() {
	mz_zip_reader_end(&ar);
}

inline std::vector<std::string> zipFileReader::getFilesList() {
	std::vector<std::string> list;
	size_t n = mz_zip_reader_get_num_files(&ar);
	for (int i = 0; i < n; i++) {
		if (mz_zip_reader_is_file_encrypted(&ar, i))continue;
		mz_zip_archive_file_stat file_stat;
		if (mz_zip_reader_file_stat(&ar, i, &file_stat)) {
			char name[2048];
			mz_zip_reader_get_filename(&ar, i, name, sizeof(name));
			list.push_back(name);
		}
	}
	return list;
}

inline void zipFileReader::extractToFolder(const std::string& nameInArchive, const std::string& destFolder) {
	int idx = getIndex(nameInArchive);
	if (idx != -1) {
		try {
			std::filesystem::path p = destFolder;
			p.append(nameInArchive);
			createPathForFile(p.generic_u8string());
			mz_zip_reader_extract_to_file(&ar, idx, p.generic_u8string().c_str(), 0);
		}
		catch (std::exception e) {
			std::cout << e.what() << std::endl;
		}
	}
}

inline void zipFileReader::extractToFile(const std::string& nameInArchive, const std::string& destFilename) {
	int idx = getIndex(nameInArchive);
	if (idx != -1) {
		mz_zip_reader_extract_to_file(&ar, idx, destFilename.c_str(), 0);
	}
}

inline void zipFileReader::extractAll(const std::string& destFolder) {
	size_t n = mz_zip_reader_get_num_files(&ar);
	for (int i = 0; i < n; i++) {
		if (mz_zip_reader_is_file_encrypted(&ar, i))continue;
		mz_zip_archive_file_stat file_stat;
		if (mz_zip_reader_file_stat(&ar, i, &file_stat)) {
			try {
				char name[2048];
				mz_zip_reader_get_filename(&ar, i, name, sizeof(name));
				std::filesystem::path p = destFolder;
				p.append(name);
				createPathForFile(p.generic_u8string());
				mz_zip_reader_extract_to_file(&ar, i, p.generic_u8string().c_str(), 0);
			}
			catch (std::exception e) {
				std::cout << e.what() << std::endl;
			}
		}
	}
}

inline void zipFileReader::extractFirstToFile(const std::string& destFilename) {
	size_t n = mz_zip_reader_get_num_files(&ar);
	if (n > 0) {
		createPathForFile(destFilename);
		mz_zip_reader_extract_to_file(&ar, 0, destFilename.c_str(), 0);
	}
}
