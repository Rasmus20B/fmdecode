module;

#include <vector>

export module chunk_types;

export enum class FS_CHUNKS : uint16_t {

  /* Simple Data *
   * =========== */
  SD1 = 0x00,

  SD2 = 0x08,

  SD3 = (0x0E << 8) | 0xFF,
  
  /* Same Type */
  SD4 = 0x10,
  SD5 = 0x11,

  /* Same Type */
  SD6 = 0x12,
  SD7 = 0x13,
  SD8 = 0x14,
  SD9 = 0x15,

  /* Same Type */
  SD10 = 0x19,
  SD11 = 0x23,

  /* Same Type */
  SD12 = 0x1A,
  SD13 = 0x1B,
  SD14 = 0x1C,
  SD15 = 0x1D,

  /* Simple Key-Value *
   * ================ */

  KV1 = 0x01,

  /* Same Type */
  KV2 = 0x02,
  KV3 = 0x03,
  KV4 = 0x04,
  KV5 = 0x05,

  KV6 = 0x06,

  KV7 = 0x09,

  /* Same Type */
  KV8 = 0x0A,
  KV9 = 0x0B,
  KV10 = 0x0C,
  KV11 = 0x0D,

  KV12 = 0x0E,

  /* Long Key-Value *
   * ============== */

  LKV1 = 0x16,
  LKV2 = 0x17,
  LKV3 = 0x1E,
  LKV4 = 0x1F,

  /* Segmented Data *
   * ============== */
  SGD1 = 0x07,
  SGD2 = 0x0F,

  /* Path Push *
   * ========= */ 
  
  /* Same Type */
  Push1 = 0x20,
  // Push2 = 0x0E,

  /* Same Type */
  Push3 = (0x20 << 8) | 0xFE,
  Push4 = (0x0E << 8) | 0xFE,

  Push5 = 0x28,

  Push6 = 0x30,

  Push7 = 0x38,

  /* Path Pop *
   * ======== */ 

  /* Same Type */
  Pop1 = 0x3D,
  Pop2 = 0x40,

  /* No-op *
   * ===== */ 
  Noop = 0x80,

};

