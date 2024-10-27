#include <cstring>
#include <db/BTreeFile.hpp>
#include <db/Database.hpp>
#include <db/IndexPage.hpp>
#include <db/LeafPage.hpp>
#include <stdexcept>

using namespace db;

BTreeFile::BTreeFile(const std::string &name, const TupleDesc &td, size_t key_index)
    : DbFile(name, td), key_index(key_index) {}

void BTreeFile::insertTuple(const Tuple &t) {
  // TODO pa2: implement
  BufferPool &bufferPool = getDatabase().getBufferPool();
  size_t page = root_id;
  if(numPages==1) { // root is leaf
    PageId pid{name, page};
    Page &p = bufferPool.getPage(pid);
    LeafPage lp(p, td, key_index);
    uint16_t size = lp.header->size;
    if(lp.insertTuple(t)) {//split leaf
      PageId new_pid{name, numPages};
      Page &new_p = bufferPool.getPage(new_pid);
      LeafPage new_lp(new_p, td, key_index);
      int split_key = lp.split(new_lp);

    }

  }

}

void BTreeFile::deleteTuple(const Iterator &it) {
  // Do not implement
}

Tuple BTreeFile::getTuple(const Iterator &it) const {
  // TODO pa2: implement
  BufferPool &bufferPool = getDatabase().getBufferPool();
  PageId pid{name, it.page};
  Page &p = bufferPool.getPage(pid);
  LeafPage lp(p, td, key_index);
  return lp.getTuple(it.slot);
}

void BTreeFile::next(Iterator &it) const {
  // TODO pa2: implement
  if(it==this->end()) {
    return;
  }
  BufferPool &bufferPool = getDatabase().getBufferPool();
  PageId pid{name, it.page};
  Page &p = bufferPool.getPage(pid);
  LeafPage lp(p, td, key_index);
  if(it.slot<lp.header->size-1) {
    it.slot++;
    return;
  }else {
    it.page=lp.header->next_leaf;
    it.slot=0;
    return;
  }
}

Iterator BTreeFile::begin() const {
  // TODO pa2: implement
  BufferPool &bufferPool = getDatabase().getBufferPool();
  size_t page = root_id;
  if(numPages==1) { // root is leaf
    PageId pid{name, page};
    Page &p = bufferPool.getPage(pid);
    const LeafPage lp(p, td, key_index);
    uint16_t size = lp.header->size;
    if(size==0) {
      return {*this, page, static_cast<size_t>(-1)};
    }
    return {*this, page, 0};
  }else {// root is index

  }

}

Iterator BTreeFile::end() const {
  // TODO pa2: implement
  BufferPool &bufferPool = getDatabase().getBufferPool();
  size_t page = root_id;
  if(numPages==1) { // root is leaf
    PageId pid{name, page};
    Page &p = bufferPool.getPage(pid);
    const LeafPage lp(p, td, key_index);
    uint16_t size = lp.header->size;
    return {*this, page, static_cast<size_t>(size - 1)};
  }else {
    //root is index
  }
}
