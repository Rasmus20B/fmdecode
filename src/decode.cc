module;

#include <memory>
#include <print>
#include <ranges>
#include <vector>

import sector;
import chunk;

export module decode;

size_t get_path_int(std::span<uint8_t> buf) {

  if(buf.size() == 1) {
    return buf[0];
  } else if(buf.size() == 2) {
    return 0x80 + ((buf[0] & 0x7F) << 8) + buf[1];
  }
  return 0;
}

export auto decode_sector(Sector s) -> std::vector<std::unique_ptr<Chunk>> {
  auto &program = s.payload;
  std::vector<std::unique_ptr<Chunk>> chunks;
  int offset = 0;
  size_t delayed_pops = 0;

  while(offset < 4076) {
    auto chunk_type = (program[offset]);
    std::println("");
    for(auto i: std::views::iota(0, 10)) {
      std::print("{:x} ", program[offset+i]);
    }
    std::println("");
    std::println("offset: {}. chunk_type: {:x}", offset, chunk_type);
    std::println("");

    if((chunk_type & 0xC0) == 0xC0) {
      chunk_type &= 0x3F;
      delayed_pops++;
    }

    switch(chunk_type) {
      case 0x00:
        {
          offset++;
          if(program[offset] == 0x00) {
            break;
          }
          auto chunk = std::make_unique<ChunkSimpleData>();
          chunk->type = FMP_CHUNK_TYPE::DATA_SIMPLE;
          chunk->len = 1; 
          chunk->loc = offset;
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }
        break;

      case 0x01:
      case 0x02:
      case 0x03:
      case 0x04:
      case 0x05:
        {
          offset++;
          auto chunk = std::make_unique<ChunkSimpleKV>();
          chunk->type = FMP_CHUNK_TYPE::REF_SIMPLE;
          chunk->ref_loc = program[offset++];
          chunk->loc = offset;
          chunk->len = (chunk_type == 0x01) + (2 * (chunk_type - 0x01));
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }
        break;
      case 0x06:
        {
          offset++;
          auto chunk = std::make_unique<ChunkSimpleKV>();
          chunk->type = FMP_CHUNK_TYPE::REF_SIMPLE;
          chunk->ref_loc = program[offset++];
          chunk->len = program[offset++];
          chunk->loc = offset;
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }
        break;
      case 0x07:
        {
          offset++;
          auto chunk = std::make_unique<ChunkSegmented>();
          chunk->seg_idx = program[offset++];
          chunk->len = get_path_int(std::span<uint8_t>(&program[offset], 2));
          offset += 2;
          chunk->loc = offset;
          offset += chunk->len;
          std::println("Found length: {}", chunk->len);
          chunks.push_back(std::move(chunk));
        }
        break;
      case 0x08:
        {
          offset++;
          auto chunk = std::make_unique<ChunkSimpleData>();
          chunk->type = FMP_CHUNK_TYPE::DATA_SIMPLE;
          chunk->len = program[offset++];
          chunk->loc = offset;
          offset += chunk->len;
          chunks.emplace_back(std::move(chunk));
        }
        break;
        
      case 0x09:
        {
          offset++;
          auto chunk = std::make_unique<ChunkSimpleKV>();
          chunk->type = FMP_CHUNK_TYPE::REF_SIMPLE;
          chunk->ref_loc = get_path_int(std::span<uint8_t>(&program[offset], 2));
          offset += 2;
          chunk->len = program[offset];
          chunk->loc = offset;
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }
        break;

      case 0x0A:
      case 0x0B:
      case 0x0C:
      case 0x0D:
        {
          offset++;
          auto chunk = std::make_unique<ChunkSimpleKV>();
          chunk->type = FMP_CHUNK_TYPE::REF_SIMPLE;
          chunk->ref_loc = get_path_int(std::span<uint8_t>(&program[offset], 2));
          offset += 2;
          chunk->loc = offset;
          chunk->len = 2 * (chunk_type - 0x09);
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }
        break;

      case 0x0E:
        {
          if(program[offset++] == 0xFF) {
            offset++;
            auto chunk = std::make_unique<ChunkSimpleKV>();
            chunk->ref_loc = get_path_int(std::span<uint8_t>(&program[offset], 2));
            offset += 2;
            chunk->len = program[offset];
            chunk->loc = offset;
            offset += chunk->len;
            chunks.push_back(std::move(chunk));
          } else {
            auto chunk = std::make_unique<ChunkSimpleData>();
            chunk->type = FMP_CHUNK_TYPE::DATA_SIMPLE;
            chunk->len = program[offset++];
            chunk->loc = offset;
            offset += chunk->len;
            chunks.push_back(std::move(chunk));
          }
        }
        break;

      case 0x10:
      case 0x11:
        // TODO: Fix up fields 
        {
          offset++;
          auto chunk = std::make_unique<ChunkSimpleData>();
          chunk->type = FMP_CHUNK_TYPE::DATA_SIMPLE;
          chunk->len = 3 + (chunk_type - 0x10);
          chunk->loc = offset++;
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }
        break;

      case 0x12:
      case 0x13:
      case 0x14:
      case 0x15:
        // TODO: Double check the length calculation
        {
          offset++;
          auto chunk = std::make_unique<ChunkSimpleData>();
          chunk->type = FMP_CHUNK_TYPE::DATA_SIMPLE;
          chunk->len = 1 + (2 *(chunk_type - 0x10));
          chunk->loc = offset++;
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }
        break;
      case 0x17:
        {
          offset++;
          auto chunk = std::make_unique<ChunkLongKV>();
          chunk->type = FMP_CHUNK_TYPE::REF_LONG;
          chunk->ref_loc = offset;
          chunk->ref_len = 3;
          offset += chunk->ref_len;
          chunk->len = get_path_int(std::span<uint8_t>(&program[offset], 2));
          offset += 2;
          chunk->loc = offset;
          offset += chunk->len;
          chunks.emplace_back(std::move(chunk));
        }
        break;

      case 0x23:
        {
          offset++;
          auto chunk = std::make_unique<ChunkSimpleData>();
          chunk->type = FMP_CHUNK_TYPE::DATA_SIMPLE;
          chunk->len = program[offset++];
          chunk->loc = offset;
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }
        break;
      case 0x19:
      case 0x1A:
      case 0x1B:
      case 0x1C:
      case 0x1D:
        {
          offset++;
          auto chunk = std::make_unique<ChunkSimpleData>();
          chunk->type = FMP_CHUNK_TYPE::DATA_SIMPLE;
          chunk->len = program[offset++];
          chunk->loc = offset;
          offset += chunk->len + (chunk_type == 0x19) + (2 * (chunk_type - 0x19));
          std::println("Found length: {}", chunk->len);
          chunks.push_back(std::move(chunk));
        }
        break;

      case 0x1E:
        {
          offset++;
          auto chunk = std::make_unique<ChunkLongKV>();
          chunk->type = FMP_CHUNK_TYPE::REF_LONG;
          chunk->ref_len = program[offset++];
          chunk->ref_loc = offset;
          offset += chunk->ref_len;
          chunk->len = program[offset++];
          chunk->loc = offset;
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }
        break;

      case 0x20:
        {
          auto chunk = std::make_unique<ChunkPathPush>();
          chunk->type = FMP_CHUNK_TYPE::PATH_PUSH;
          offset++;
          if(program[offset] == 0xFE) {
            offset++;
            chunk->len = 8;
          } else {
            chunk->len = 1;
          }
          chunk->loc = offset;
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }
        break;

      case 0x28:
        {
          auto chunk = std::make_unique<ChunkPathPush>();
          chunk->type = FMP_CHUNK_TYPE::PATH_PUSH;
          offset++;
          chunk->loc = offset;
          chunk->len = 2;
          offset += chunk->len;
        }
        break;
      case 0x30:
        {
          auto chunk = std::make_unique<ChunkPathPush>();
          chunk->type = FMP_CHUNK_TYPE::PATH_PUSH;
          offset++;
          chunk->loc = offset;
          chunk->len = 3;
          offset += chunk->len;
        }
        break;

      case 0x38:
        {
          auto chunk = std::make_unique<ChunkPathPush>();
          chunk->type = FMP_CHUNK_TYPE::PATH_PUSH;
          offset++;
          chunk->len = program[offset++];
          chunk->loc = offset;
          offset += chunk->len;
        }
        break;
      case 0x3D:
      case 0x40:
        {
          auto chunk = std::make_unique<ChunkPathPop>();
          chunks.emplace_back(std::move(chunk));
          offset++;
        }
        break;

      case 0x80:
        {
          auto chunk = std::make_unique<ChunkNoop>();
          chunks.emplace_back(std::move(chunk));
          offset++;
        }
        break;

      case 0xC0:
        {
          auto chunk = std::make_unique<ChunkPathPop>();
          chunk->type = FMP_CHUNK_TYPE::PATH_POP;
          chunks.push_back(std::move(chunk));
          offset++;
        }
        break;

      default:
        std::println("Unimplemented Chunk Type. {:x}", program[offset]);
        return chunks;
    }

    while(delayed_pops) {
      std::println("Pushing delayed pop");
      auto delayed_pop = std::make_unique<ChunkPathPop>();
      chunks.push_back(std::move(delayed_pop));
      delayed_pops--;
    }
  }

  // for(auto &c: chunks) {
  //   c->print();
  // }
  return chunks;
}
