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
#घोषणा VIRTIO_MMIO_MAGIC_मूल्य		0x000 // 0x74726976
#घोषणा VIRTIO_MMIO_VERSION		0x004 // version; should be 2
#घोषणा VIRTIO_MMIO_DEVICE_ID		0x008 // device type; 1 is net, 2 is disk
#घोषणा VIRTIO_MMIO_VENDOR_ID		0x00c // 0x554d4551
#घोषणा VIRTIO_MMIO_DEVICE_FEATURES	0x010
#घोषणा VIRTIO_MMIO_DRIVER_FEATURES	0x020
#घोषणा VIRTIO_MMIO_QUEUE_SEL		0x030 // select queue, write-only
#घोषणा VIRTIO_MMIO_QUEUE_अंक_MAX	0x034 // max size of current queue, read-only
#घोषणा VIRTIO_MMIO_QUEUE_अंक		0x038 // size of current queue, write-only
#घोषणा VIRTIO_MMIO_QUEUE_READY		0x044 // ready bit
#घोषणा VIRTIO_MMIO_QUEUE_NOTIFY	0x050 // write-only
#घोषणा VIRTIO_MMIO_INTERRUPT_STATUS	0x060 // read-only
#घोषणा VIRTIO_MMIO_INTERRUPT_ACK	0x064 // write-only
#घोषणा VIRTIO_MMIO_STATUS		0x070 // read/write
#घोषणा VIRTIO_MMIO_QUEUE_DESC_निचला	0x080 // physical address for descriptor table, write-only
#घोषणा VIRTIO_MMIO_QUEUE_DESC_ऊपरी	0x084
#घोषणा VIRTIO_MMIO_DRIVER_DESC_निचला	0x090 // physical address for available ring, write-only
#घोषणा VIRTIO_MMIO_DRIVER_DESC_ऊपरी	0x094
#घोषणा VIRTIO_MMIO_DEVICE_DESC_निचला	0x0a0 // physical address for used ring, write-only
#घोषणा VIRTIO_MMIO_DEVICE_DESC_ऊपरी	0x0a4
// clang-format on

// status register bits, from qemu virtio_config.h
#घोषणा VIRTIO_CONFIG_S_ACKNOWLEDGE 1
#घोषणा VIRTIO_CONFIG_S_DRIVER      2
#घोषणा VIRTIO_CONFIG_S_DRIVER_OK   4
#घोषणा VIRTIO_CONFIG_S_FEATURES_OK 8

// device feature bits
#घोषणा VIRTIO_BLK_F_RO             5  /* Disk is read-only */
#घोषणा VIRTIO_BLK_F_SCSI           7  /* Supports scsi command passthru */
#घोषणा VIRTIO_BLK_F_FLUSH          9  /* Cache flush command supported */
#घोषणा VIRTIO_BLK_F_CONFIG_WCE     11 /* Writeback mode available in config */
#घोषणा VIRTIO_BLK_F_MQ             12 /* support more than one vq */
#घोषणा VIRTIO_F_ANY_LAYOUT         27
#घोषणा VIRTIO_RING_F_INDIRECT_DESC 28
#घोषणा VIRTIO_RING_F_EVENT_IDX     29

// this many virtio descriptors.
// must be a power of two.
#घोषणा अंक_2 8

// a single descriptor, from the spec.
काष्ठा virtq_desc {
  uint64 addr;
  uint32 len;
  uint16 flags;
  uint16 अग्र;
};
#घोषणा VRING_DESC_F_अग्र  1 // chained with another descriptor
#घोषणा VRING_DESC_F_WRITE 2 // device writes (vs read)

// the (entire) avail ring, from the spec.
काष्ठा virtq_avail {
  uint16 flags;     // always zero
  uint16 idx;       // driver will write ring[idx] next
  uint16 ring[अंक_2]; // descriptor numbers of chain heads
  uint16 unused;
};

// one entry in the "used" ring, with which the
// device tells the driver about completed requests.
काष्ठा virtq_used_elem {
  uint32 id; // index of start of completed descriptor chain
  uint32 len;
};

काष्ठा virtq_used {
  uint16 flags; // always zero
  uint16 idx;   // device increments when it adds a ring[] entry
  काष्ठा virtq_used_elem ring[अंक_2];
};

// these are specific to virtio block devices, e.g. disks,
// described in Section 5.2 of the spec.

#घोषणा VIRTIO_BLK_T_IN  0 // read the disk
#घोषणा VIRTIO_BLK_T_OUT 1 // write the disk

// the format of the first descriptor in a disk request.
// to be followed by two more descriptors containing
// the block, and a one-byte status.
काष्ठा virtio_blk_req {
  uint32 type; // VIRTIO_BLK_T_IN or ..._OUT
  uint32 reserved;
  uint64 sector;
};
