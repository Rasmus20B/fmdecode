module;

#include <cstddef>
#include <cstdint>
#include <span>
#include <print>
#include <vector>
#include <array>

export module header;

static constexpr std::array<std::string_view, 2> versions = {
  "FP7",
  "FMP12",
};

export enum class FM_VER {
  FM7,
  FM12
};

export struct Header {
  bool valid;
  FM_VER version;
  std::string created_by;
};

export auto get_header_info(std::span<uint8_t> header) -> Header {
  Header result;

  constexpr std::array<uint8_t, 15> magic_number = {
    0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x05, 0x00, 0x02, 0x00, 0x02, 0xC0
  };

  constexpr std::array<char, 5> ver12_ext = {
    'H', 'B', 'A', 'M', '7'
  };

  result.valid = true;

  for(int i = 0; i < magic_number.size(); ++i) {
    if(magic_number[i] != header[i]) {
      result.valid = false;
      break;
    }
  }

  auto offset = 15;
  result.version = FM_VER::FM12;
  for(int i = 0; i < ver12_ext.size(); ++i) {
    if(ver12_ext[i] != header[offset]) {
      result.version = FM_VER::FM7;
    }
    offset++;
  }
  offset = 541;
  auto s_len = header[offset];
  std::string creation_program;
  creation_program.append(reinterpret_cast<char*>(&header[offset+1]), s_len);
  result.created_by = creation_program;
  return result;
}

export void print_header_info(Header h) {
  std::println("Header\n======");
  std::println("Valid FM file: {}", h.valid);
  std::println("FM Version: {}", versions[std::to_underlying(h.version)]);
  std::println("File Created by Program: {}", h.created_by);
}
