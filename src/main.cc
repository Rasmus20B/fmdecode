#include <cstddef>
#include <cstdint>
#include <span>
#include <print>
#include <vector>
#include <array>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

import sector;
import header;

import chunk;
import decode;

static constexpr size_t SEC_SIZE = 4096;

auto get_mapped_file(std::string_view path) -> std::span<uint8_t> {
  auto fd = open(path.data(), O_RDONLY);
  if(fd == -1) {
    std::println(stderr, "failed to open file: {}", path);
  }
  struct stat fileInfo;
  if(stat(path.data(), &fileInfo) == -1) {
    std::println(stderr, "unable to stat file: {}", path);
  }
  uint8_t* data = static_cast<uint8_t*>(mmap(0, fileInfo.st_size, PROT_READ, MAP_SHARED, fd, 0));
  if(data == MAP_FAILED) {
    std::println(stderr, "mmap failed.");
  }
  close(fd);
  return std::span<uint8_t> { data, static_cast<size_t>(fileInfo.st_size) };
}



auto main(int argc, char **argv) -> int {

  if(argc != 2) {
    std::println("Usage: {} FILENAME", argv[0]);
    return 0;
  }

  std::span map = get_mapped_file(argv[1]);
  std::vector<Sector> sectors;
  std::span sp { map };
  auto header = get_header_info(map.subspan(0, SEC_SIZE));
  print_header_info(header);
  auto offset = 4096;

  while(offset) {
    auto s = get_sector_info(sp.subspan(offset, SEC_SIZE));
    s.id = sectors.size();
    sectors.push_back(std::move(s));
    offset = SEC_SIZE * s.next;
  }

  std::vector<std::unique_ptr<Chunk>> chunks;
  for(auto &s : sectors) {
    auto c = decode_sector(s);
    std::move(c.begin(), c.end(), std::back_inserter(chunks));
  }

  for(auto &c: chunks) {
    c->print();
  }
  return 0;
}
