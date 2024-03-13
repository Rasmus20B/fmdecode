module;

#include <vector>
#include <print>

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
};

export template<typename Derived>
struct ChunkBase : public Chunk {
  virtual void print() {
    static_cast<Derived*>(this)->print();
  }
};

export struct ChunkPathPush : public ChunkBase<ChunkPathPush> {
  void print() override {
    std::println("===========================================");
    std::println("Chunk Type: Path Push.");
    std::println("===========================================");
  }
  size_t loc;
};

export struct ChunkPathPop : public ChunkBase<ChunkPathPop> {
  void print() override {
    std::println("===========================================");
    std::println("Chunk Type: Path Pop.");
    std::println("===========================================");
  }
};

export struct ChunkSimpleData : public ChunkBase<ChunkSimpleData> {
  size_t loc;
  void print() override {
    std::println("===========================================");
    std::println("Chunk Type: Simple Data.");
    std::println("===========================================");
  }
};

export struct ChunkSimpleKV : public ChunkBase<ChunkSimpleKV> {
  void print() override {
    std::println("===========================================");
    std::println("Chunk Type: Simple Key-Value.");
    std::println("===========================================");
  }
  size_t loc;
  size_t ref_len;
  size_t ref_loc;
};

export struct ChunkLongKV : public ChunkBase<ChunkLongKV> {
  void print() override {
    std::println("===========================================");
    std::println("Chunk Type: Long Key-Value.");
    std::println("===========================================");
  }

  size_t loc;
  size_t ref_len;
  size_t ref_loc;
};

export struct ChunkSegmented : public ChunkBase<ChunkSegmented> {
  void print() override {
    std::println("===========================================");
    std::println("Chunk Type: Segmented Data.");
    std::println("===========================================");
  }
  size_t loc;
  size_t seg_idx;
};

export struct ChunkNoop : public ChunkBase<ChunkNoop> {
  void print() override {
    std::println("===========================================");
    std::println("Chunk Type: Noop.");
    std::println("===========================================");
  }
};
