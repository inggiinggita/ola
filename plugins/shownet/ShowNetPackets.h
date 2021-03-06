/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * ShowNetPackets.h
 * Datagram definitions for the ShowNet protocol.
 * Copyright (C) 2005-2009 Simon Newton
 */

#ifndef PLUGINS_SHOWNET_SHOWNETPACKETS_H_
#define PLUGINS_SHOWNET_SHOWNETPACKETS_H_

/**
 * Some reference info:
 * https://code.google.com/p/open-lighting/issues/detail?id=218
 */

#include "ola/BaseTypes.h"

namespace ola {
namespace plugin {
namespace shownet {

enum { SHOWNET_MAC_LENGTH = 6 };
enum { SHOWNET_NAME_LENGTH = 9 };
enum { SHOWNET_SPARE_LENGTH = 22 };

// Assume this is 512.
enum { SHOWNET_DMX_DATA_LENGTH = 512 };

enum { SHOWNET_COMPRESSED_DATA_LENGTH = 1269 };

enum ShowNetPacketType {
  DMX_PACKET = 0x202f,
  COMPRESSED_DMX_PACKET = 0x808f,
};

// The old style Shownet DMX packet. Type 0x202f . Apparently this isn't used
// much.
struct shownet_dmx_s {
  uint16_t port;
  uint16_t slot_length;
  uint16_t pool_size;
  uint16_t h_slot;
  uint32_t sequence;
  uint8_t priority;  // 0 = not used
  uint8_t universe;  // 0 = not used
  uint16_t spare[SHOWNET_SPARE_LENGTH];
  uint8_t dmx_data[SHOWNET_DMX_DATA_LENGTH];
} __attribute__((packed));

typedef struct shownet_dmx_s shownet_dmx;

// The 'new' style, compressed shownet packet. Type 0x808f
// Each packet can contain up to 4 'blocks' of DMX data.
struct shownet_compressed_dmx_s {
  uint16_t netSlot[4];       // start channel of each slot (hSlot)
  uint16_t slotSize[4];      // size of each slot
  uint16_t indexBlock[5];    // index into data of each slot
  uint16_t sequence;         // not used in n21+ code.
  uint8_t priority;          // not used in n21+ code, 0 = not used
  uint8_t universe;          // not used in n21+ code, 0 = not used
  uint8_t pass[2];           // something to do with the channels that have
                             // passwords. PasswordNumChans ?
  char name[SHOWNET_NAME_LENGTH];  // name of console
  uint8_t data[SHOWNET_COMPRESSED_DATA_LENGTH];  // RLE data.
} __attribute__((packed));

typedef struct shownet_compressed_dmx_s shownet_compressed_dmx;

// The union of all packets.
typedef struct {
  uint16_t type;   // packet type
  uint8_t  ip[4];  // ip of sender
  union {
    shownet_dmx dmx;
    shownet_compressed_dmx compressed_dmx;
  } data;
} shownet_packet;

}  // namespace shownet
}  // namespace plugin
}  // namespace ola
#endif  // PLUGINS_SHOWNET_SHOWNETPACKETS_H_
