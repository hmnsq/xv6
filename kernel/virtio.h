//
// virtio device definitions.
// for both the mmio interface, and virtio descriptors.
// only tested with qemu.
//
// the virtio spec:
// https://docs.oasis-open.org/virtio/virtio/v1.1/virtio-v1.1.pdf
//

// virtio mmio control registers, mapped starting at 0x10001000.
// from qemu virtio_mmio.h
// clang-format off
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_MAGIC_ਮੁੱਲ		0x000 // 0x74726976
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_ਸੰਸਕਰਣ		0x004 // version; should be 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_DEVICE_ID		0x008 // device type; 1 is net, 2 is disk
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_VENDOR_ID		0x00c // 0x554d4551
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_DEVICE_FEATURES	0x010
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_DRIVER_FEATURES	0x020
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_ਕਤਾਰ_SEL		0x030 // select queue, write-only
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_ਕਤਾਰ_ਅੰਕ_MAX	0x034 // max size of current queue, read-only
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_ਕਤਾਰ_ਅੰਕ		0x038 // size of current queue, write-only
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_ਕਤਾਰ_READY		0x044 // ready bit
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_ਕਤਾਰ_NOTIFY	0x050 // write-only
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_ਰੁਕਾਵਟ_ਸਥਿਤੀ	0x060 // read-only
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_ਰੁਕਾਵਟ_ACK	0x064 // write-only
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_ਸਥਿਤੀ		0x070 // read/write
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_ਕਤਾਰ_DESC_ਨੀਵਾਂ	0x080 // physical address for descriptor table, write-only
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_ਕਤਾਰ_DESC_ਉੱਚਾ	0x084
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_DRIVER_DESC_ਨੀਵਾਂ	0x090 // physical address for available ring, write-only
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_DRIVER_DESC_ਉੱਚਾ	0x094
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_DEVICE_DESC_ਨੀਵਾਂ	0x0a0 // physical address for used ring, write-only
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_MMIO_DEVICE_DESC_ਉੱਚਾ	0x0a4
// clang-format on

// status register bits, from qemu virtio_config.h
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_CONFIG_S_ACKNOWLEDGE 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_CONFIG_S_DRIVER      2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_CONFIG_S_DRIVER_OK   4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_CONFIG_S_FEATURES_OK 8

// device feature bits
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_BLK_F_RO             5  /* Disk is read-only */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_BLK_F_SCSI           7  /* Supports scsi command passthru */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_BLK_F_FLUSH          9  /* Cache flush command supported */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_BLK_F_CONFIG_WCE     11 /* Writeback mode available in config */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_BLK_F_MQ             12 /* support more than one vq */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_F_ANY_LAYOUT         27
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_RING_F_ਅਸਿੱਧਾ_DESC 28
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_RING_F_EVENT_IDX     29

// this many virtio descriptors.
// must be a power of two.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਕ_2 8

// a single descriptor, from the spec.
ਬਣਤਰ virtq_desc {
  uint64 addr;
  uint32 len;
  uint16 flags;
  uint16 ਅਗਲਾ;
};
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VRING_DESC_F_ਅਗਲਾ  1 // chained with another descriptor
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VRING_DESC_F_WRITE 2 // device writes (vs read)

// the (entire) avail ring, from the spec.
ਬਣਤਰ virtq_avail {
  uint16 flags;     // always zero
  uint16 idx;       // driver will write ring[idx] next
  uint16 ring[ਅੰਕ_2]; // descriptor numbers of chain heads
  uint16 ਅਣਵਰਤਿਆ_2;
};

// one entry in the "used" ring, with which the
// device tells the driver about completed requests.
ਬਣਤਰ virtq_ਵਰਤਿਆ_elem {
  uint32 id; // index of start of completed descriptor chain
  uint32 len;
};

ਬਣਤਰ virtq_ਵਰਤਿਆ {
  uint16 flags; // always zero
  uint16 idx;   // device increments when it adds a ring[] entry
  ਬਣਤਰ virtq_ਵਰਤਿਆ_elem ring[ਅੰਕ_2];
};

// these are specific to virtio block devices, e.g. disks,
// described in Section 5.2 of the spec.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_BLK_T_ਵਿੱਚ  0 // read the disk
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRTIO_BLK_T_ਬਾਹਰਲਾ 1 // write the disk

// the format of the first descriptor in a disk request.
// to be followed by two more descriptors containing
// the block, and a one-byte status.
ਬਣਤਰ virtio_blk_req {
  uint32 type; // VIRTIO_BLK_T_IN or ..._OUT
  uint32 reserved;
  uint64 sector;
};
