#include <db/IndexPage.hpp>
#include<iostream>
#include <stdexcept>

using namespace db;

IndexPage::IndexPage(Page &page) {
  // TODO pa2: implement
  header = reinterpret_cast<IndexPageHeader *>(page.data());
  keys = reinterpret_cast<int *>(page.data() + sizeof(IndexPageHeader));
  capacity = (DEFAULT_PAGE_SIZE-sizeof(IndexPageHeader)-sizeof(size_t))/(sizeof(int) + sizeof(size_t));
  children = reinterpret_cast<size_t *>(keys + capacity);

}

bool IndexPage::insert(int key, size_t child) {
  // TODO pa2: implement

  int pos = 0;
  while (pos < header->size && keys[pos] < key) {
    ++pos;
  }

  for (int i = header->size; i > pos; --i) {
    keys[i] = keys[i - 1];
    children[i + 1] = children[i];
  }

  keys[pos] = key;
  children[pos + 1] = child;

  ++header->size;

  if (header->size >= capacity) {
    return true;
  }
  return false;
}

int IndexPage::split(IndexPage &new_page) {
  // TODO pa2: implement
  uint16_t mid = header->size / 2;
  for (int i = mid + 1, j = 0; i < header->size; ++i, ++j) {
    new_page.keys[j] = keys[i];
    new_page.children[j] = children[i];
  }
  new_page.children[header->size - mid - 1] = children[header->size];

  new_page.header->size = header->size - mid - 1;
  int split_key = keys[mid];
  header->size = mid;

  return split_key;

}
