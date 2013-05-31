/*
 * tusb_descriptors.h
 *
 *  Created on: Nov 26, 2012
 *      Author: hathachtware License Agreement (BSD License)
 * Copyright (c) 2012, hathach (tinyusb.org)All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the tinyUSB stack
 */

#ifndef _TUSB_DESCRIPTORS_H_
#define _TUSB_DESCRIPTORS_H_

#include "tusb.h"

#define TUSB_CFG_DEVICE_STRING_MANUFACTURER   "tinyUSB"
#define TUSB_CFG_DEVICE_STRING_PRODUCT        "Device Keyboard"
#define TUSB_CFG_DEVICE_STRING_SERIAL         "1234"
#define TUSB_CFG_DEVICE_VENDORID              0x1FC9 // NXP
//#define TUSB_CFG_DEVICE_PRODUCTID

/* USB Serial uses the MCUs unique 128-bit chip ID via an IAP call = 32 hex chars */
#define USB_STRING_SERIAL_LEN     32

#define USB_STRING_LEN(n) (2 + ((n)<<1))

typedef ATTR_PACKED_STRUCT(struct)
{
  tusb_descriptor_header_t LangID;
  uint16_t strLangID[1];

  tusb_descriptor_header_t Manufacturer;
  uint16_t strManufacturer[sizeof(TUSB_CFG_DEVICE_STRING_MANUFACTURER)-1]; // exclude null-character

  tusb_descriptor_header_t Product;
  uint16_t strProduct[sizeof(TUSB_CFG_DEVICE_STRING_PRODUCT)-1]; // exclude null-character

  tusb_descriptor_header_t Serial;
  uint16_t strSerial[sizeof(TUSB_CFG_DEVICE_STRING_SERIAL)-1];
} app_descriptor_string_t;

// USB Interface Assosication Descriptor
#define  USB_DEVICE_CLASS_IAD        USB_DEVICE_CLASS_MISCELLANEOUS
#define  USB_DEVICE_SUBCLASS_IAD     0x02
#define  USB_DEVICE_PROTOCOL_IAD     0x01

///////////////////////////////////////////////////////////////////////
// Interface Assosication Descriptor if device is CDC + other class
#define IAD_DESC_REQUIRED ( defined(TUSB_CFG_DEVICE_CDC) && (DEVICE_CLASS_HID) )

#ifdef TUSB_CFG_DEVICE_CDC
  #define INTERFACES_OF_CDC           2
#else
  #define INTERFACES_OF_CDC           0
#endif

#if TUSB_CFG_DEVICE_HID_KEYBOARD
  #define INTERFACES_OF_HID_KEYBOARD  1
#else
  #define INTERFACES_OF_HID_KEYBOARD  0
#endif

#if TUSB_CFG_DEVICE_HID_MOUSE
  #define INTERFACES_OF_HID_MOUSE     1
#else
  #define INTERFACES_OF_HID_MOUSE     0
#endif

#if TUSB_CFG_HOST_HID_GENERIC
  #define INTERFACES_OF_HID_GENERIC   1
#else
  #define INTERFACES_OF_HID_GENERIC   0
#endif

#if CFG_USB_MASS_STORAGE
  #define INTERFACES_OF_MASS_STORAGE  2
#else
  #define INTERFACES_OF_MASS_STORAGE  0
#endif

#define INTERFACE_INDEX_CDC           0
#define INTERFACE_INDEX_HID_KEYBOARD (INTERFACE_INDEX_CDC          + INTERFACES_OF_CDC          )
#define INTERFACE_INDEX_HID_MOUSE    (INTERFACE_INDEX_HID_KEYBOARD + INTERFACES_OF_HID_KEYBOARD )
#define INTERFACE_INDEX_HID_GENERIC  (INTERFACE_INDEX_HID_MOUSE    + INTERFACES_OF_HID_MOUSE    )
#define INTERFACE_INDEX_MASS_STORAGE (INTERFACE_INDEX_HID_GENERIC  + INTERFACES_OF_HID_GENERIC  )

#define TOTAL_INTEFACES              (INTERFACES_OF_CDC + INTERFACES_OF_HID_KEYBOARD + INTERFACES_OF_HID_MOUSE + INTERFACES_OF_HID_GENERIC + INTERFACES_OF_MASS_STORAGE)

// Bitmap: MassStorage | Generic | Mouse | Key | CDC
#define PRODUCTID_BITMAP(interface, n)  ( (INTERFACES_OF_##interface ? 1 : 0) << (n) )
#define USB_PRODUCT_ID                  (0x2000 | ( PRODUCTID_BITMAP(CDC, 0) | PRODUCTID_BITMAP(HID_KEYBOARD, 1) | PRODUCTID_BITMAP(HID_MOUSE, 2) | \
                                                    PRODUCTID_BITMAP(HID_GENERIC, 3) | PRODUCTID_BITMAP(MASS_STORAGE, 4) ) )

///////////////////////////////////////////////////////////////////////
typedef ATTR_PACKED_STRUCT(struct)
{
  tusb_descriptor_configuration_t                configuration;

#if IAD_DESC_REQUIRED
  tusb_descriptor_interface_association_t        CDC_IAD;
#endif

#if TUSB_CFG_DEVICE_CDC
  //CDC - Serial
  //CDC Control Interface
  tusb_descriptor_interface_t                    CDC_CCI_Interface;
  CDC_HEADER_DESCRIPTOR                       CDC_Header;
  CDC_ABSTRACT_CONTROL_MANAGEMENT_DESCRIPTOR  CDC_ACM;
  CDC_UNION_1SLAVE_DESCRIPTOR                 CDC_Union;
  tusb_descriptor_endpoint_t                     CDC_NotificationEndpoint;

  //CDC Data Interface
  tusb_descriptor_interface_t                    CDC_DCI_Interface;
  tusb_descriptor_endpoint_t                     CDC_DataOutEndpoint;
  tusb_descriptor_endpoint_t                     CDC_DataInEndpoint;
#endif

  //------------- HID Keyboard -------------//
#if TUSB_CFG_DEVICE_HID_KEYBOARD
  tusb_descriptor_interface_t                    keyboard_interface;
  tusb_hid_descriptor_hid_t                      keyboard_hid;
  tusb_descriptor_endpoint_t                     keyboard_endpoint;
#endif

//------------- HID Mouse -------------//
#if TUSB_CFG_DEVICE_HID_MOUSE
  tusb_descriptor_interface_t                    mouse_interface;
  tusb_hid_descriptor_hid_t                      mouse_hid;
  tusb_descriptor_endpoint_t                     mouse_endpoint;
#endif

  uint8_t                                        null_termination; // NXP rom driver requires this to work
} app_descriptor_configuration_t;

extern const tusb_descriptor_device_t app_tusb_desc_device;
extern const app_descriptor_configuration_t app_tusb_desc_configuration;
extern const app_descriptor_string_t app_tusb_desc_strings;

extern const uint8_t app_tusb_keyboard_desc_report[];

//extern const uint8_t HID_MouseReportDescriptor[];

#endif