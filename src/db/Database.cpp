#include <db/Database.hpp>
#include <iostream>

using namespace db;

BufferPool &Database::getBufferPool() { return bufferPool; }

Database &db::getDatabase() {
  static Database instance;
  return instance;
}

void Database::add(std::unique_ptr<DbFile> file) {
  // TODO pa1: add the file to the catalog. Note that the file must not exist.

  for (const auto& existingFile : files) {
    if (existingFile->getName() == file->getName()) {
      throw std::runtime_error("File with name \"" + file->getName() + "\" already exixts.");
      return;
    }
  }

  files.push_back(std::move(file));
  std::cout << "File added to database." << std::endl;

}

std::unique_ptr<DbFile> Database::remove(const std::string &name) {
  // TODO pa1: remove the file from the catalog. Note that the file must exist.
  for (auto it = files.begin(); it != files.end(); ++it) {
    if ((*it)->getName() == name) {
      std::unique_ptr<DbFile> removedFile = std::move(*it);
      files.erase(it);
      std::cout << "File \"" << name << "\" removed from database." << std::endl;
      return removedFile;
    }
  }
  throw std::runtime_error("File with name \"" + name + "\" does not exist.");
}

DbFile &Database::get(const std::string &name) const {
  // TODO pa1: get the file from the catalog. Note that the file must exist.
  for (const auto& file : files) {
    if (file->getName() == name) {
      return *file;
    }
  }
  throw std::runtime_error("File with name \"" + name + "\" does not exist.");

}
