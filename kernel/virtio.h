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
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_MAGIC_ಮೌಲ್ಯ		0x000 // 0x74726976
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_ಆವೃತ್ತಿ		0x004 // version; should be 2
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_DEVICE_ID		0x008 // device type; 1 is net, 2 is disk
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_VENDOR_ID		0x00c // 0x554d4551
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_DEVICE_FEATURES	0x010
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_DRIVER_FEATURES	0x020
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_ಸರತಿ_SEL		0x030 // select queue, write-only
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_ಸರತಿ_ಅಂಕೆ_MAX	0x034 // max size of current queue, read-only
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_ಸರತಿ_ಅಂಕೆ		0x038 // size of current queue, write-only
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_ಸರತಿ_READY		0x044 // ready bit
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_ಸರತಿ_NOTIFY	0x050 // write-only
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_ಅಡಚಣೆ_ಸ್ಥಿತಿ	0x060 // read-only
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_ಅಡಚಣೆ_ACK	0x064 // write-only
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_ಸ್ಥಿತಿ		0x070 // read/write
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_ಸರತಿ_DESC_ಕೆಳಗಿನ	0x080 // physical address for descriptor table, write-only
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_ಸರತಿ_DESC_ಮೇಲಿನ	0x084
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_DRIVER_DESC_ಕೆಳಗಿನ	0x090 // physical address for available ring, write-only
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_DRIVER_DESC_ಮೇಲಿನ	0x094
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_DEVICE_DESC_ಕೆಳಗಿನ	0x0a0 // physical address for used ring, write-only
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_MMIO_DEVICE_DESC_ಮೇಲಿನ	0x0a4
// clang-format on

// status register bits, from qemu virtio_config.h
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_CONFIG_S_ACKNOWLEDGE 1
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_CONFIG_S_DRIVER      2
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_CONFIG_S_DRIVER_OK   4
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_CONFIG_S_FEATURES_OK 8

// device feature bits
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_BLK_F_RO             5  /* Disk is read-only */
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_BLK_F_SCSI           7  /* Supports scsi command passthru */
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_BLK_F_FLUSH          9  /* Cache flush command supported */
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_BLK_F_CONFIG_WCE     11 /* Writeback mode available in config */
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_BLK_F_MQ             12 /* support more than one vq */
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_F_ANY_LAYOUT         27
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_RING_F_ಪರೋಕ್ಷ_DESC 28
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_RING_F_EVENT_IDX     29

// this many virtio descriptors.
// must be a power of two.
#ವ್ಯಾಖ್ಯಾನಿಸು ಅಂಕೆ_2 8

// a single descriptor, from the spec.
ರಚನೆ virtq_desc {
  uint64 addr;
  uint32 len;
  uint16 flags;
  uint16 ಮುಂದಿನ;
};
#ವ್ಯಾಖ್ಯಾನಿಸು VRING_DESC_F_ಮುಂದಿನ  1 // chained with another descriptor
#ವ್ಯಾಖ್ಯಾನಿಸು VRING_DESC_F_WRITE 2 // device writes (vs read)

// the (entire) avail ring, from the spec.
ರಚನೆ virtq_avail {
  uint16 flags;     // always zero
  uint16 idx;       // driver will write ring[idx] next
  uint16 ring[ಅಂಕೆ_2]; // descriptor numbers of chain heads
  uint16 ಬಳಸದ_2;
};

// one entry in the "used" ring, with which the
// device tells the driver about completed requests.
ರಚನೆ virtq_ಬಳಸಿದ_elem {
  uint32 id; // index of start of completed descriptor chain
  uint32 len;
};

ರಚನೆ virtq_ಬಳಸಿದ {
  uint16 flags; // always zero
  uint16 idx;   // device increments when it adds a ring[] entry
  ರಚನೆ virtq_ಬಳಸಿದ_elem ring[ಅಂಕೆ_2];
};

// these are specific to virtio block devices, e.g. disks,
// described in Section 5.2 of the spec.

#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_BLK_T_ಒಳ  0 // read the disk
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO_BLK_T_ಹೊರಹೋಗು 1 // write the disk

// the format of the first descriptor in a disk request.
// to be followed by two more descriptors containing
// the block, and a one-byte status.
ರಚನೆ virtio_blk_req {
  uint32 type; // VIRTIO_BLK_T_IN or ..._OUT
  uint32 reserved;
  uint64 sector;
};
