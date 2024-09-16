#include <db/BufferPool.hpp>
#include <db/Database.hpp>
#include <iostream>
#include <numeric>
#include <ostream>

using namespace db;

BufferPool::BufferPool()
// TODO pa1: add initializations if needed
:posIndex(0)
{
  // TODO pa1: additional initialization if needed
}

BufferPool::~BufferPool() {
  // TODO pa1: flush any remaining dirty pages
  for (auto &node : pageNodes) {
    if (node.dirty) {
      flushPageNode(node);
    }
  }
}

Page &BufferPool::getPage(const PageId &pid) {

  // TODO pa1: If already in buffer pool, make it the most recent page and return it

  // TODO pa1: If there are no available pages, evict the least recently used page. If it is dirty, flush it to disk

  // TODO pa1: Read the page from disk to one of the available slots, make it the most recent page

  auto it = std::find_if(pageNodes.begin(), pageNodes.end(), [&pid](const PageNode &node) { return node.pageId == pid; });

  // If the page is already in bufferbool
  if (it != pageNodes.end()) {
    pageNodes.splice(pageNodes.begin(), pageNodes, it);
    return pages[it->pindex];
  }


  if (pageNodes.size() >= DEFAULT_NUM_PAGES) {
    // if bufferpool is full, remove the less used node and flush it's page
    PageNode &lruPage = pageNodes.back();
    if (lruPage.dirty) {
      flushPageNode(lruPage);
    }
    //set the position to the now page
    posIndex = lruPage.pindex;
    pageNodes.pop_back();
  }else {
    // if bufferpool is not full
    posIndex = pageNodes.size();
  }

  PageNode newPageNode(pid, posIndex);
  Database &db = getDatabase();

  DbFile &file = db.get(pid.file);
  file.readPage(pages[newPageNode.pindex], pid.page);

  pageNodes.push_front(std::move(newPageNode));
  return pages[pageNodes.front().pindex];
}

void BufferPool::markDirty(const PageId &pid) {
  // TODO pa1: Mark the page as dirty. Note that the page must already be in the buffer pool
  const PageNode &node = findPage_const(pid);
  PageNode& nonConstRef = const_cast<PageNode&>(node);
  nonConstRef.dirty = true;
}

bool BufferPool::isDirty(const PageId &pid) const {
  // TODO pa1: Return whether the page is dirty. Note that the page must already be in the buffer pool
  const PageNode &node = findPage_const(pid);
  return node.dirty;
}

bool BufferPool::contains(const PageId &pid) const {
  // TODO pa1: Return whether the page is in the buffer pool
  auto it = std::find_if(pageNodes.begin(), pageNodes.end(), [&pid](const PageNode &node) { return node.pageId == pid; });

  if (it != pageNodes.end()) {
    return true;
  }
  return false;
}

void BufferPool::discardPage(const PageId &pid) {
  // TODO pa1: Discard the page from the buffer pool. Note that the page must already be in the buffer pool
  auto it = std::find_if(pageNodes.begin(), pageNodes.end(), [&pid](const PageNode &node) { return node.pageId == pid; });

  if (it != pageNodes.end()) {
    pageNodes.erase(it);
  } else {
    throw std::runtime_error("Page not found.");
  }
}

void BufferPool::flushPage(const PageId &pid) {
  // TODO pa1: Flush the page to disk. Note that the page must already be in the buffer pool
  const PageNode &node = findPage_const(pid);
  PageNode& nonConstRef = const_cast<PageNode&>(node);
  flushPageNode(nonConstRef);
}

void BufferPool::flushFile(const std::string &file) {
  // TODO pa1: Flush all pages of the file to disk
  for (auto &node : pageNodes) {
    if (node.pageId.file == file && node.dirty == true) {
      flushPageNode(node);
    }
  }
}

const BufferPool::PageNode &BufferPool::findPage_const(const PageId &id) const {
  auto it = std::find_if(pageNodes.cbegin(), pageNodes.cend(), [&id](const PageNode &node) { return node.pageId == id; });

  if (it != pageNodes.cend()) {
    return *it;
  } else {
    throw std::runtime_error("Page not found.");
  }
}


void BufferPool::flushPageNode(PageNode &node) {
  Database &db = getDatabase(); // Get the singleton instance of Database
  if(node.dirty==true) {
    std::cout << "Flushing page " << node.pageId.page << " from file " << node.pageId.file << std::endl;

    DbFile &f = db.get(node.pageId.file);
    f.writePage(pages[node.pindex], node.pageId.page);
    std::cout << "Flushed page " << node.pageId.page << " from file " << node.pageId.file << std::endl;
    node.dirty = false;
  }

}

