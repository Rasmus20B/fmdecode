module;

#include <cstddef>
#include <print>
#include <span>
#include <vector>


export module sector;

export struct Sector {
  size_t id;
  bool deleted;
  uint32_t level;
  uint32_t previous;
  uint32_t next;
  std::vector<uint8_t> payload;
};

export auto get_sector_info(std::span<uint8_t> sector) -> Sector {
  Sector result;
  result.deleted = sector[0];
  result.level = sector[1] & 0x00FFFFFF;
  result.previous = (sector[4] << 24) | (sector[5] << 16) | (sector[6] << 8) | (sector[7]);
  result.next = (sector[8] << 24) | (sector[9] << 16) | (sector[10] << 8) | (sector[11]);
  result.payload = { sector.begin() + 20,  sector.end() };
  return result;
}

export auto print_sector_header(Sector sec) {
  std::println("Sector Info.");
  std::println("Deleted: {}", sec.deleted);
  std::println("level: {}", sec.level);
  std::println("previous: {}", sec.previous);
  std::println("next: {}", sec.next);
}
