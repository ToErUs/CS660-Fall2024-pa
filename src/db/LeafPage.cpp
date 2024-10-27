#include <db/LeafPage.hpp>
#include <stdexcept>
#include <cstring>
using namespace db;

LeafPage::LeafPage(Page &page, const TupleDesc &td, size_t key_index) : td(td), key_index(key_index) {
  // TODO pa2: implement
  header = reinterpret_cast<LeafPageHeader *>(page.data());
  data = page.data() + sizeof(LeafPageHeader);
  capacity=(DEFAULT_PAGE_SIZE - sizeof(LeafPageHeader))/td.length();
}

bool LeafPage::insertTuple(const Tuple &t) {
  // TODO pa2: implement
  if (!td.compatible(t)) {
    throw std::runtime_error("Tuple not compatible with TupleDesc");
  }
  int new_key = std::get<int>(t.get_field(key_index));

  size_t pos = 0;
  while (pos < header->size) {
    int existing_key = std::get<int>(getTuple(pos).get_field(key_index));
    if (existing_key >= new_key) {
      break;
    }
    ++pos;
  }

  if (pos < header->size) {
    int existing_key = std::get<int>(getTuple(pos).get_field(key_index));
    if (existing_key == new_key) {
      td.serialize(data + pos * td.length(),t);
      return header->size >= capacity;
    }
  }

  if (header->size >= capacity) {
    return true;
  }

  for (size_t i = header->size; i > pos; --i) {
    std::memcpy(data + i * td.length(), data + (i - 1) * td.length(), td.length());
  }

  td.serialize(data + pos * td.length(),t);

  ++header->size;

  return header->size >= capacity;
}

int LeafPage::split(LeafPage &new_page) {
  // TODO pa2: implement
  size_t mid = header->size / 2;
  new_page.header->size = header->size - mid;

  header->size = mid;

  size_t src_offset = (mid * td.length());
  size_t dest_offset = 0;

  std::memcpy(new_page.data + dest_offset, data + src_offset, new_page.header->size * td.length());
  new_page.header->next_leaf = header->next_leaf;
  header->next_leaf = reinterpret_cast<size_t>(&new_page);
  field_t first_key_field = new_page.getTuple(0).get_field(key_index);

  return std::get<int>(first_key_field);
}

Tuple LeafPage::getTuple(size_t slot) const {
  // TODO pa2: implement
  if (slot >= header->size) {
    throw std::out_of_range("Index out of range");
  }

  size_t pos = slot * td.length();

  return td.deserialize(data + pos);
}
