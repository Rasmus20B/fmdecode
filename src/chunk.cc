module;

#include <vector>
#include <print>
#include <ranges>
#include <any>
export module chunk;

export enum class FMP_CHUNK_TYPE : uint16_t {
  PATH_PUSH,
  PATH_POP,
  DATA_SIMPLE,
  REF_SIMPLE,
  REF_LONG,
  DATA_SEGMENT
};


export struct Chunk {
  virtual ~Chunk() {}
  virtual void print() = 0;


  FMP_CHUNK_TYPE type;
  size_t len;
  uint8_t* block_start;
};

export template<typename Derived>
struct ChunkBase : public Chunk {
  virtual void print() {
    static_cast<Derived*>(this)->print();
  }

  virtual Derived get_chunk() {
    return static_cast<Derived*>(this)->get_chunk();
  }

  // virtual std::string get_vals() {
  //   return static_cast<Derived*>(this)->get_vals();
  // }
};

export struct ChunkPathPush : public ChunkBase<ChunkPathPush> {

  ChunkPathPush get_chunk() override {
    return *this;
  }

  void print() override {
    std::println("Chunk Type: Path Push.");
    std::println("Pushing {} to path.", std::string(reinterpret_cast<char*>(block_start + loc), len));
    std::println("===========================================");
  }

  // std::string get_vals() override {
  //   return std::string(reinterpret_cast<char*>(block_start + loc), len);
  // }
  size_t loc;
};

export struct ChunkPathPop : public ChunkBase<ChunkPathPop> {
  void print() override {
    std::println("Chunk Type: Path Pop.");
    std::println("===========================================");
  }
};

export struct ChunkSimpleData : public ChunkBase<ChunkSimpleData> {
  size_t loc;
  void print() override {
    std::println("Chunk Type: Simple Data.");
    auto data = std::string(reinterpret_cast<char*>(block_start + loc), len);
    for(auto &c: data) {
      c = c ^ 0x5a;
    }
    std::println("Data[{}]: {}", len, data);
    std::println("===========================================");
  }
};

export struct ChunkSimpleKV : public ChunkBase<ChunkSimpleKV> {
  void print() override {
    std::println("Chunk Type: Simple Key-Value.");
    if(ref_len)
      std::println("data[{}]: {}", ref_len, std::string_view(reinterpret_cast<char*>(block_start + ref_loc), ref_len));
    else
      std::println("data[{}].", ref_len);
    std::println("===========================================");
  }
  size_t loc;
  size_t ref_len;
  size_t ref_loc;
};

export struct ChunkLongKV : public ChunkBase<ChunkLongKV> {
  void print() override {
    std::println("Chunk Type: Long Key-Value.");
    std::println("===========================================");
  }

  size_t loc;
  size_t ref_len;
  size_t ref_loc;
};

export struct ChunkSegmented : public ChunkBase<ChunkSegmented> {
  void print() override {
    std::println("Chunk Type: Segmented Data.");
    std::println("===========================================");
  }
  size_t loc;
  size_t seg_idx;
};

export struct ChunkNoop : public ChunkBase<ChunkNoop> {
  void print() override {
    std::println("Chunk Type: Noop.");
    std::println("===========================================");
  }
};
