module;

#include <memory>
#include <print>
#include <vector>

import sector;
import chunk_types;

export module decode;

struct FSNode {
  std::unique_ptr<FSNode> parent;
  std::vector<std::unique_ptr<FSNode>> children;
};

export auto decode_sector(Sector s) -> void {

  auto &program = s.payload;
  int offset = 0;

  while(offset < 4076) {

    auto chunk_type = program[offset];

    std::println("offset: {}. chunk_type: {:x}", offset, chunk_type);
    switch(static_cast<FS_CHUNKS>(chunk_type)) {
      case FS_CHUNKS::SD1:
        offset += 2;
        std::println("SIMPLE DATA1. Offset: {}", offset);
        break;
      case FS_CHUNKS::SD2:
        offset += 2;
        std::println("SIMPLE DATA2");
        break;
      case FS_CHUNKS::SD3:
        offset += 7;
        std::println("SIMPLE DATA3");
        break;
      case FS_CHUNKS::SD4:
      case FS_CHUNKS::SD5:
        offset += 7;
        std::println("SIMPLE DATA4");
        break;
      case FS_CHUNKS::SD6:
      case FS_CHUNKS::SD7:
      case FS_CHUNKS::SD8:
      case FS_CHUNKS::SD9:
        offset += 2 * (chunk_type - 0x10) + 2;
        std::println("SIMPLE DATA5");
        break;
      case FS_CHUNKS::SD10:
      case FS_CHUNKS::SD11:
        offset += 2;
        std::println("SIMPLE DATA6");
        break;
      case FS_CHUNKS::SD12:
      case FS_CHUNKS::SD13:
      case FS_CHUNKS::SD14:
      case FS_CHUNKS::SD15:
        offset += 2 * (chunk_type - 0x19) + 1;
        std::println("SIMPLE DATA7: offset: {}", offset);
        break;


      case FS_CHUNKS::KV1:
        offset += 3;
        break;
      case FS_CHUNKS::KV2:
      case FS_CHUNKS::KV3:
      case FS_CHUNKS::KV4:
      case FS_CHUNKS::KV5:
        offset += 3 + (2 * (chunk_type - 1));
        break;
      case FS_CHUNKS::KV6: 
        {
          auto n = program[offset + 1];
          offset += n + 3;
        }
        break;
      case FS_CHUNKS::KV7:
        offset += 4;
        break;
      case FS_CHUNKS::KV8:
      case FS_CHUNKS::KV9:
      case FS_CHUNKS::KV10:
      case FS_CHUNKS::KV11:
        offset += 3 + (2 * (chunk_type - 9));
        break;
      case FS_CHUNKS::KV12:
        std::println("Found a simple kv.");
        break;

      case FS_CHUNKS::LKV1:
      case FS_CHUNKS::LKV2:
      case FS_CHUNKS::LKV3:
      case FS_CHUNKS::LKV4:
        std::println("Found a long KV");
        break;

      case FS_CHUNKS::SGD1:
      case FS_CHUNKS::SGD2:
        std::println("Found a Segmented Data chunk.");
        break;

      case FS_CHUNKS::Push1:
      // case FS_CHUNKS::Push2:
        offset += 2; 
        break;
      case FS_CHUNKS::Push3:
      case FS_CHUNKS::Push4:
        offset += 10;
        break;
      case FS_CHUNKS::Push5:
        offset += 3;
        std::println("FODUN A PUSH BRUH");
        break;
      case FS_CHUNKS::Push6:
        offset += 4;
        break;


      case FS_CHUNKS::Pop1:
      case FS_CHUNKS::Pop2:
        offset += 1;
        break;

      case FS_CHUNKS::Noop:
        offset += 1;
        break;

      default:
        std::println("Unimplemented : {:x}.", chunk_type);
        return;
    }
  }
}
