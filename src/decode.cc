module;

#include <memory>
#include <print>
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

  while(offset < 4076) {
    auto chunk_type = (program[offset]);
    switch(chunk_type) {
      case 0x00:
        {
          offset++;
          auto chunk = std::make_unique<ChunkSimpleData>();
          chunk->type = FMP_CHUNK_TYPE::DATA_SIMPLE;
          chunk->len = 1; 
          chunk->loc = offset++;
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
          break;
        }
      case 0x01:
      case 0x02:
      case 0x03:
      case 0x04:
      case 0x05:
        {
          offset++;
          auto chunk = std::make_unique<ChunkSimpleKV>();
          chunk->type = FMP_CHUNK_TYPE::REF_SIMPLE;
          chunk->ref_loc = offset++;
          chunk->loc = offset;
          chunk->len = (chunk_type == 0x01) + 2 * (chunk_type - 0x01);
          offset += chunk->len;
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
          chunks.push_back(std::move(chunk));
          break;
        }
        
      case 0x09:
        {
          offset++;
          auto chunk = std::make_unique<ChunkSimpleKV>();
          chunk->type = FMP_CHUNK_TYPE::REF_SIMPLE;
          chunk->ref_loc = get_path_int(std::span<uint8_t>(program.begin() + offset, program.begin() + offset + 1));
          offset += 2;
          chunk->len = program[offset];
          chunk->loc = offset;
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }
        break;

      case 0x0E:
        {
          if(program[offset++] == 0xFF) {
            offset++;
            auto chunk = std::make_unique<ChunkSimpleKV>();
            chunk->type = FMP_CHUNK_TYPE::REF_SIMPLE;
            chunk->ref_loc = get_path_int(std::span<uint8_t>(program.begin() + offset, program.begin() + offset + 1));
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
          auto chunk = std::make_unique<ChunkSimpleData>();
          chunk->type = FMP_CHUNK_TYPE::DATA_SIMPLE;
          chunk->len = 1 + (2 *(chunk_type - 0x10));
          chunk->loc = offset++;
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }
        break;

      case 0x19:
      case 0x23:
        {
          auto chunk = std::make_unique<ChunkSimpleData>();
          chunk->type = FMP_CHUNK_TYPE::DATA_SIMPLE;
          chunk->len = program[offset++];
          chunk->loc = offset;
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }

      case 0x1A:
      case 0x1B:
      case 0x1C:
      case 0x1D:
        {
          auto chunk = std::make_unique<ChunkSimpleData>();
          chunk->type = FMP_CHUNK_TYPE::DATA_SIMPLE;
          chunk->len = 2 * (chunk_type - 0x19);
          chunk->loc = offset++;
          offset += chunk->len;
          chunks.push_back(std::move(chunk));
        }
        break;

      case 0x20:
        {
          auto chunk = std::make_unique<ChunkPathPush>();
          chunk->type = FMP_CHUNK_TYPE::PATH_PUSH;
          offset++;
          if(program[offset+1] == 0xFE) {
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

      case 0xC0:
        {
          auto chunk = std::make_unique<ChunkPathPop>();
          chunk->type = FMP_CHUNK_TYPE::PATH_POP;
          chunks.push_back(std::move(chunk));
          offset++;
          break;
        }

      case 0xE0:
        {
          auto chunk = std::make_unique<ChunkPathPush>();
          chunk->type = FMP_CHUNK_TYPE::PATH_PUSH;
          offset++;
          if(program[offset+1] == 0xFE) {
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

      // case 0xFF:
      //   {
      //     chunk_type = program[offset++];
      //     if(0x40 <= chunk_type <= 0x80) {
      //       offset++;
      //       auto chunk = std::make_unique<ChunkSimpleKV>();
      //       chunk->type = FMP_CHUNK_TYPE::REF_SIMPLE;
      //       chunk->ref_loc = get_path_int(std::span<uint8_t>(program.begin() + offset, program.begin() + offset + 1));
      //       offset += 2;
      //       chunk->len = program[offset];
      //       chunk->loc = offset;
      //       offset += chunk->len;
      //       chunks.push_back(std::move(chunk));
      //     } else if(0x01 <= 0x04) {
      //
      //     }
      //   }
        break;
      default:
        std::println("Unimplemented Chunk Type. {:x}", program[offset]);
    }
    // std::println("offset: {}. chunk_type: {:x}", offset, chunk_type);
    offset++;
  }

  for(auto &c: chunks) {
    c->print();
  }
  return chunks;
}
