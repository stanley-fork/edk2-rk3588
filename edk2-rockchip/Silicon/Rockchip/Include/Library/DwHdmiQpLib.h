/** @file LcdHwLib.h

  This file contains interface functions for LcdHwLib of ArmPlatformPkg

  Copyright (c) 2017, ARM Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __DWHDMIQP_H__
#define __DWHDMIQP_H__

#include <Library/RockchipDisplayLib.h>
#include <Uefi/UefiSpec.h>
#include <Uefi/UefiBaseType.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/RockchipConnectorProtocol.h>
#include <Library/DrmModes.h>

#define EREMOTEIO  121          /* Remote I/O error */
#define EINVAL     22           /* Invalid argument */
#define ETIMEDOUT  110          /* Connection timed out */

/* Main Unit Registers */
#define CORE_ID              0x0
#define VER_NUMBER           0x4
#define VER_TYPE             0x8
#define CONFIG_REG           0xc
#define CONFIG_CEC           BIT(28)
#define CONFIG_AUD_UD        BIT(23)
#define CORE_TIMESTAMP_HHMM  0x14
#define CORE_TIMESTAMP_MMDD  0x18
#define CORE_TIMESTAMP_YYYY  0x1c
/* Reset Manager Registers */
#define GLOBAL_SWRESET_REQUEST               0x40
#define EARCRX_CMDC_SWINIT_P                 BIT(27)
#define AVP_DATAPATH_PACKET_AUDIO_SWINIT_P   BIT(10)
#define AVP_DATAPATH_SWINIT_P                BIT(6)
#define GLOBAL_SWDISABLE                     0x44
#define CEC_SWDISABLE                        BIT(17)
#define AVP_DATAPATH_PACKET_AUDIO_SWDISABLE  BIT(10)
#define AVP_DATAPATH_VIDEO_SWDISABLE         BIT(6)
#define RESET_MANAGER_CONFIG0                0x48
#define RESET_MANAGER_STATUS0                0x50
#define RESET_MANAGER_STATUS1                0x54
#define RESET_MANAGER_STATUS2                0x58
/* Timer Base Registers */
#define TIMER_BASE_CONFIG0  0x80
#define TIMER_BASE_STATUS0  0x84
/* CMU Registers */
#define CMU_CONFIG0          0xa0
#define CMU_CONFIG1          0xa4
#define CMU_CONFIG2          0xa8
#define CMU_CONFIG3          0xac
#define CMU_STATUS           0xb0
#define EARC_BPCLK_OFF       BIT(9)
#define AUDCLK_OFF           BIT(7)
#define LINKQPCLK_OFF        BIT(5)
#define VIDQPCLK_OFF         BIT(3)
#define IPI_CLK_OFF          BIT(1)
#define CMU_IPI_CLK_FREQ     0xb4
#define CMU_VIDQPCLK_FREQ    0xb8
#define CMU_LINKQPCLK_FREQ   0xbc
#define CMU_AUDQPCLK_FREQ    0xc0
#define CMU_EARC_BPCLK_FREQ  0xc4
/* I2CM Registers */
#define I2CM_SM_SCL_CONFIG0          0xe0
#define I2CM_FM_SCL_CONFIG0          0xe4
#define I2CM_CONFIG0                 0xe8
#define I2CM_CONTROL0                0xec
#define I2CM_STATUS0                 0xf0
#define I2CM_INTERFACE_CONTROL0      0xf4
#define I2CM_ADDR                    0xff000
#define I2CM_SLVADDR                 0xfe0
#define I2CM_WR_MASK                 0x1e
#define I2CM_EXT_READ                BIT(4)
#define I2CM_SHORT_READ              BIT(3)
#define I2CM_FM_READ                 BIT(2)
#define I2CM_FM_WRITE                BIT(1)
#define I2CM_FM_EN                   BIT(0)
#define I2CM_INTERFACE_CONTROL1      0xf8
#define I2CM_SEG_PTR                 0x7f80
#define I2CM_SEG_ADDR                0x7f
#define I2CM_INTERFACE_WRDATA_0_3    0xfc
#define I2CM_INTERFACE_WRDATA_4_7    0x100
#define I2CM_INTERFACE_WRDATA_8_11   0x104
#define I2CM_INTERFACE_WRDATA_12_15  0x108
#define I2CM_INTERFACE_RDDATA_0_3    0x10c
#define I2CM_INTERFACE_RDDATA_4_7    0x110
#define I2CM_INTERFACE_RDDATA_8_11   0x114
#define I2CM_INTERFACE_RDDATA_12_15  0x118
/* SCDC Registers */
#define SCDC_CONFIG0             0x140
#define SCDC_I2C_FM_EN           BIT(12)
#define SCDC_UPD_FLAGS_AUTO_CLR  BIT(6)
#define SCDC_UPD_FLAGS_POLL_EN   BIT(4)
#define SCDC_CONTROL0            0x148
#define SCDC_STATUS0             0x150
#define STATUS_UPDATE            BIT(0)
#define FRL_START                BIT(4)
#define FLT_UPDATE               BIT(5)
/* FLT Registers */
#define FLT_CONFIG0   0x160
#define FLT_CONFIG1   0x164
#define FLT_CONFIG2   0x168
#define FLT_CONTROL0  0x170
/*  Main Unit 2 Registers */
#define MAINUNIT_STATUS0  0x180
/* Video Interface Registers */
#define VIDEO_INTERFACE_CONFIG0   0x800
#define VIDEO_INTERFACE_CONFIG1   0x804
#define VIDEO_INTERFACE_CONFIG2   0x808
#define VIDEO_INTERFACE_CONTROL0  0x80c
#define VIDEO_INTERFACE_STATUS0   0x814
/* Video Packing Registers */
#define VIDEO_PACKING_CONFIG0  0x81c
/* Audio Interface Registers */
#define AUDIO_INTERFACE_CONFIG0   0x820
#define AUD_IF_SEL_MSK            0x3
#define AUD_IF_SPDIF              0x2
#define AUD_IF_I2S                0x1
#define AUD_IF_PAI                0x0
#define AUD_FIFO_INIT_ON_OVF_MSK  BIT(2)
#define AUD_FIFO_INIT_ON_OVF_EN   BIT(2)
#define I2S_LINES_EN_MSK          GENMASK(7, 4)
#define I2S_LINES_EN(x)  BIT((x) + 4)
#define I2S_BPCUV_RCV_MSK         BIT(12)
#define I2S_BPCUV_RCV_EN          BIT(12)
#define I2S_BPCUV_RCV_DIS         0
#define SPDIF_LINES_EN            GENMASK(19, 16)
#define AUD_FORMAT_MSK            GENMASK(26, 24)
#define AUD_3DOBA                 (0x7 << 24)
#define AUD_3DASP                 (0x6 << 24)
#define AUD_MSOBA                 (0x5 << 24)
#define AUD_MSASP                 (0x4 << 24)
#define AUD_HBR                   (0x3 << 24)
#define AUD_DST                   (0x2 << 24)
#define AUD_OBA                   (0x1 << 24)
#define AUD_ASP                   (0x0 << 24)
#define AUDIO_INTERFACE_CONFIG1   0x824
#define AUDIO_INTERFACE_CONTROL0  0x82c
#define AUDIO_FIFO_CLR_P          BIT(0)
#define AUDIO_INTERFACE_STATUS0   0x834
/* Frame Composer Registers */
#define FRAME_COMPOSER_CONFIG0   0x840
#define FRAME_COMPOSER_CONFIG1   0x844
#define FRAME_COMPOSER_CONFIG2   0x848
#define FRAME_COMPOSER_CONFIG3   0x84c
#define FRAME_COMPOSER_CONFIG4   0x850
#define FRAME_COMPOSER_CONFIG5   0x854
#define FRAME_COMPOSER_CONFIG6   0x858
#define FRAME_COMPOSER_CONFIG7   0x85c
#define FRAME_COMPOSER_CONFIG8   0x860
#define FRAME_COMPOSER_CONFIG9   0x864
#define KEEPOUT_REKEY_CFG        GENMASK(9, 8)
#define KEEPOUT_REKEY_ALWAYS     (0x2 << 8)
#define FRAME_COMPOSER_CONTROL0  0x86c
/* Video Monitor Registers */
#define VIDEO_MONITOR_CONFIG0  0x880
#define VIDEO_MONITOR_STATUS0  0x884
#define VIDEO_MONITOR_STATUS1  0x888
#define VIDEO_MONITOR_STATUS2  0x88c
#define VIDEO_MONITOR_STATUS3  0x890
#define VIDEO_MONITOR_STATUS4  0x894
#define VIDEO_MONITOR_STATUS5  0x898
#define VIDEO_MONITOR_STATUS6  0x89c
/* HDCP2 Logic Registers */
#define HDCP2LOGIC_CONFIG0       0x8e0
#define HDCP2_BYPASS             BIT(0)
#define HDCP2LOGIC_ESM_GPIO_IN   0x8e4
#define HDCP2LOGIC_ESM_GPIO_OUT  0x8e8
/* HDCP14 Registers */
#define HDCP14_CONFIG0     0x900
#define HDCP14_CONFIG1     0x904
#define HDCP14_CONFIG2     0x908
#define HDCP14_CONFIG3     0x90c
#define HDCP14_KEY_SEED    0x914
#define HDCP14_KEY_H       0x918
#define HDCP14_KEY_L       0x91c
#define HDCP14_KEY_STATUS  0x920
#define HDCP14_AKSV_H      0x924
#define HDCP14_AKSV_L      0x928
#define HDCP14_AN_H        0x92c
#define HDCP14_AN_L        0x930
#define HDCP14_STATUS0     0x934
#define HDCP14_STATUS1     0x938
/* Scrambler Registers */
#define SCRAMB_CONFIG0  0x960
/* Video Configuration Registers */
#define LINK_CONFIG0       0x968
#define OPMODE_FRL_4LANES  BIT(8)
#define OPMODE_DVI         BIT(4)
#define OPMODE_FRL         BIT(0)
/* TMDS FIFO Registers */
#define TMDS_FIFO_CONFIG0   0x970
#define TMDS_FIFO_CONTROL0  0x974
/* FRL RSFEC Registers */
#define FRL_RSFEC_CONFIG0  0xa20
#define FRL_RSFEC_STATUS0  0xa30
/* FRL Packetizer Registers */
#define FRL_PKTZ_CONFIG0   0xa40
#define FRL_PKTZ_CONTROL0  0xa44
#define FRL_PKTZ_CONTROL1  0xa50
#define FRL_PKTZ_STATUS1   0xa54
/* Packet Scheduler Registers */
#define PKTSCHED_CONFIG0                          0xa80
#define PKTSCHED_PRQUEUE0_CONFIG0                 0xa84
#define PKTSCHED_PRQUEUE1_CONFIG0                 0xa88
#define PKTSCHED_PRQUEUE2_CONFIG0                 0xa8c
#define PKTSCHED_PRQUEUE2_CONFIG1                 0xa90
#define PKTSCHED_PRQUEUE2_CONFIG2                 0xa94
#define PKTSCHED_PKT_CONFIG0                      0xa98
#define PKTSCHED_PKT_CONFIG1                      0xa9c
#define PKTSCHED_VSI_FIELDRATE                    BIT(14)
#define PKTSCHED_AVI_FIELDRATE                    BIT(12)
#define PKTSCHED_PKT_CONFIG2                      0xaa0
#define PKTSCHED_PKT_CONFIG3                      0xaa4
#define PKTSCHED_PKT_EN                           0xaa8
#define PKTSCHED_DRMI_TX_EN                       BIT(17)
#define PKTSCHED_AUDI_TX_EN                       BIT(15)
#define PKTSCHED_AVI_TX_EN                        BIT(13)
#define PKTSCHED_VSI_TX_EN                        BIT(12)
#define PKTSCHED_EMP_CVTEM_TX_EN                  BIT(10)
#define PKTSCHED_AMD_TX_EN                        BIT(8)
#define PKTSCHED_GCP_TX_EN                        BIT(3)
#define PKTSCHED_AUDS_TX_EN                       BIT(2)
#define PKTSCHED_ACR_TX_EN                        BIT(1)
#define PKTSCHED_NULL_TX_EN                       BIT(0)
#define PKTSCHED_PKT_CONTROL0                     0xaac
#define PKTSCHED_PKT_SEND                         0xab0
#define PKTSCHED_PKT_STATUS0                      0xab4
#define PKTSCHED_PKT_STATUS1                      0xab8
#define PKT_NULL_CONTENTS0                        0xb00
#define PKT_NULL_CONTENTS1                        0xb04
#define PKT_NULL_CONTENTS2                        0xb08
#define PKT_NULL_CONTENTS3                        0xb0c
#define PKT_NULL_CONTENTS4                        0xb10
#define PKT_NULL_CONTENTS5                        0xb14
#define PKT_NULL_CONTENTS6                        0xb18
#define PKT_NULL_CONTENTS7                        0xb1c
#define PKT_ACP_CONTENTS0                         0xb20
#define PKT_ACP_CONTENTS1                         0xb24
#define PKT_ACP_CONTENTS2                         0xb28
#define PKT_ACP_CONTENTS3                         0xb2c
#define PKT_ACP_CONTENTS4                         0xb30
#define PKT_ACP_CONTENTS5                         0xb34
#define PKT_ACP_CONTENTS6                         0xb38
#define PKT_ACP_CONTENTS7                         0xb3c
#define PKT_ISRC1_CONTENTS0                       0xb40
#define PKT_ISRC1_CONTENTS1                       0xb44
#define PKT_ISRC1_CONTENTS2                       0xb48
#define PKT_ISRC1_CONTENTS3                       0xb4c
#define PKT_ISRC1_CONTENTS4                       0xb50
#define PKT_ISRC1_CONTENTS5                       0xb54
#define PKT_ISRC1_CONTENTS6                       0xb58
#define PKT_ISRC1_CONTENTS7                       0xb5c
#define PKT_ISRC2_CONTENTS0                       0xb60
#define PKT_ISRC2_CONTENTS1                       0xb64
#define PKT_ISRC2_CONTENTS2                       0xb68
#define PKT_ISRC2_CONTENTS3                       0xb6c
#define PKT_ISRC2_CONTENTS4                       0xb70
#define PKT_ISRC2_CONTENTS5                       0xb74
#define PKT_ISRC2_CONTENTS6                       0xb78
#define PKT_ISRC2_CONTENTS7                       0xb7c
#define PKT_GMD_CONTENTS0                         0xb80
#define PKT_GMD_CONTENTS1                         0xb84
#define PKT_GMD_CONTENTS2                         0xb88
#define PKT_GMD_CONTENTS3                         0xb8c
#define PKT_GMD_CONTENTS4                         0xb90
#define PKT_GMD_CONTENTS5                         0xb94
#define PKT_GMD_CONTENTS6                         0xb98
#define PKT_GMD_CONTENTS7                         0xb9c
#define PKT_AMD_CONTENTS0                         0xba0
#define PKT_AMD_CONTENTS1                         0xba4
#define PKT_AMD_CONTENTS2                         0xba8
#define PKT_AMD_CONTENTS3                         0xbac
#define PKT_AMD_CONTENTS4                         0xbb0
#define PKT_AMD_CONTENTS5                         0xbb4
#define PKT_AMD_CONTENTS6                         0xbb8
#define PKT_AMD_CONTENTS7                         0xbbc
#define PKT_VSI_CONTENTS0                         0xbc0
#define PKT_VSI_CONTENTS1                         0xbc4
#define PKT_VSI_CONTENTS2                         0xbc8
#define PKT_VSI_CONTENTS3                         0xbcc
#define PKT_VSI_CONTENTS4                         0xbd0
#define PKT_VSI_CONTENTS5                         0xbd4
#define PKT_VSI_CONTENTS6                         0xbd8
#define PKT_VSI_CONTENTS7                         0xbdc
#define PKT_AVI_CONTENTS0                         0xbe0
#define HDMI_FC_AVICONF0_ACTIVE_FMT_INFO_PRESENT  BIT(4)
#define HDMI_FC_AVICONF0_BAR_DATA_VERT_BAR        0x04
#define HDMI_FC_AVICONF0_BAR_DATA_HORIZ_BAR       0x08
#define HDMI_FC_AVICONF2_IT_CONTENT_VALID         0x80
#define PKT_AVI_CONTENTS1                         0xbe4
#define PKT_AVI_CONTENTS2                         0xbe8
#define PKT_AVI_CONTENTS3                         0xbec
#define PKT_AVI_CONTENTS4                         0xbf0
#define PKT_AVI_CONTENTS5                         0xbf4
#define PKT_AVI_CONTENTS6                         0xbf8
#define PKT_AVI_CONTENTS7                         0xbfc
#define PKT_SPDI_CONTENTS0                        0xc00
#define PKT_SPDI_CONTENTS1                        0xc04
#define PKT_SPDI_CONTENTS2                        0xc08
#define PKT_SPDI_CONTENTS3                        0xc0c
#define PKT_SPDI_CONTENTS4                        0xc10
#define PKT_SPDI_CONTENTS5                        0xc14
#define PKT_SPDI_CONTENTS6                        0xc18
#define PKT_SPDI_CONTENTS7                        0xc1c
#define PKT_AUDI_CONTENTS0                        0xc20
#define PKT_AUDI_CONTENTS1                        0xc24
#define PKT_AUDI_CONTENTS2                        0xc28
#define PKT_AUDI_CONTENTS3                        0xc2c
#define PKT_AUDI_CONTENTS4                        0xc30
#define PKT_AUDI_CONTENTS5                        0xc34
#define PKT_AUDI_CONTENTS6                        0xc38
#define PKT_AUDI_CONTENTS7                        0xc3c
#define PKT_NVI_CONTENTS0                         0xc40
#define PKT_NVI_CONTENTS1                         0xc44
#define PKT_NVI_CONTENTS2                         0xc48
#define PKT_NVI_CONTENTS3                         0xc4c
#define PKT_NVI_CONTENTS4                         0xc50
#define PKT_NVI_CONTENTS5                         0xc54
#define PKT_NVI_CONTENTS6                         0xc58
#define PKT_NVI_CONTENTS7                         0xc5c
#define PKT_DRMI_CONTENTS0                        0xc60
#define PKT_DRMI_CONTENTS1                        0xc64
#define PKT_DRMI_CONTENTS2                        0xc68
#define PKT_DRMI_CONTENTS3                        0xc6c
#define PKT_DRMI_CONTENTS4                        0xc70
#define PKT_DRMI_CONTENTS5                        0xc74
#define PKT_DRMI_CONTENTS6                        0xc78
#define PKT_DRMI_CONTENTS7                        0xc7c
#define PKT_GHDMI1_CONTENTS0                      0xc80
#define PKT_GHDMI1_CONTENTS1                      0xc84
#define PKT_GHDMI1_CONTENTS2                      0xc88
#define PKT_GHDMI1_CONTENTS3                      0xc8c
#define PKT_GHDMI1_CONTENTS4                      0xc90
#define PKT_GHDMI1_CONTENTS5                      0xc94
#define PKT_GHDMI1_CONTENTS6                      0xc98
#define PKT_GHDMI1_CONTENTS7                      0xc9c
#define PKT_GHDMI2_CONTENTS0                      0xca0
#define PKT_GHDMI2_CONTENTS1                      0xca4
#define PKT_GHDMI2_CONTENTS2                      0xca8
#define PKT_GHDMI2_CONTENTS3                      0xcac
#define PKT_GHDMI2_CONTENTS4                      0xcb0
#define PKT_GHDMI2_CONTENTS5                      0xcb4
#define PKT_GHDMI2_CONTENTS6                      0xcb8
#define PKT_GHDMI2_CONTENTS7                      0xcbc
/* EMP Packetizer Registers */
#define PKT_EMP_CONFIG0           0xce0
#define PKT_EMP_CONTROL0          0xcec
#define PKT_EMP_CONTROL1          0xcf0
#define PKT_EMP_CONTROL2          0xcf4
#define PKT_EMP_VTEM_CONTENTS0    0xd00
#define PKT_EMP_VTEM_CONTENTS1    0xd04
#define PKT_EMP_VTEM_CONTENTS2    0xd08
#define PKT_EMP_VTEM_CONTENTS3    0xd0c
#define PKT_EMP_VTEM_CONTENTS4    0xd10
#define PKT_EMP_VTEM_CONTENTS5    0xd14
#define PKT_EMP_VTEM_CONTENTS6    0xd18
#define PKT_EMP_VTEM_CONTENTS7    0xd1c
#define PKT0_EMP_CVTEM_CONTENTS0  0xd20
#define PKT0_EMP_CVTEM_CONTENTS1  0xd24
#define PKT0_EMP_CVTEM_CONTENTS2  0xd28
#define PKT0_EMP_CVTEM_CONTENTS3  0xd2c
#define PKT0_EMP_CVTEM_CONTENTS4  0xd30
#define PKT0_EMP_CVTEM_CONTENTS5  0xd34
#define PKT0_EMP_CVTEM_CONTENTS6  0xd38
#define PKT0_EMP_CVTEM_CONTENTS7  0xd3c
#define PKT1_EMP_CVTEM_CONTENTS0  0xd40
#define PKT1_EMP_CVTEM_CONTENTS1  0xd44
#define PKT1_EMP_CVTEM_CONTENTS2  0xd48
#define PKT1_EMP_CVTEM_CONTENTS3  0xd4c
#define PKT1_EMP_CVTEM_CONTENTS4  0xd50
#define PKT1_EMP_CVTEM_CONTENTS5  0xd54
#define PKT1_EMP_CVTEM_CONTENTS6  0xd58
#define PKT1_EMP_CVTEM_CONTENTS7  0xd5c
#define PKT2_EMP_CVTEM_CONTENTS0  0xd60
#define PKT2_EMP_CVTEM_CONTENTS1  0xd64
#define PKT2_EMP_CVTEM_CONTENTS2  0xd68
#define PKT2_EMP_CVTEM_CONTENTS3  0xd6c
#define PKT2_EMP_CVTEM_CONTENTS4  0xd70
#define PKT2_EMP_CVTEM_CONTENTS5  0xd74
#define PKT2_EMP_CVTEM_CONTENTS6  0xd78
#define PKT2_EMP_CVTEM_CONTENTS7  0xd7c
#define PKT3_EMP_CVTEM_CONTENTS0  0xd80
#define PKT3_EMP_CVTEM_CONTENTS1  0xd84
#define PKT3_EMP_CVTEM_CONTENTS2  0xd88
#define PKT3_EMP_CVTEM_CONTENTS3  0xd8c
#define PKT3_EMP_CVTEM_CONTENTS4  0xd90
#define PKT3_EMP_CVTEM_CONTENTS5  0xd94
#define PKT3_EMP_CVTEM_CONTENTS6  0xd98
#define PKT3_EMP_CVTEM_CONTENTS7  0xd9c
#define PKT4_EMP_CVTEM_CONTENTS0  0xda0
#define PKT4_EMP_CVTEM_CONTENTS1  0xda4
#define PKT4_EMP_CVTEM_CONTENTS2  0xda8
#define PKT4_EMP_CVTEM_CONTENTS3  0xdac
#define PKT4_EMP_CVTEM_CONTENTS4  0xdb0
#define PKT4_EMP_CVTEM_CONTENTS5  0xdb4
#define PKT4_EMP_CVTEM_CONTENTS6  0xdb8
#define PKT4_EMP_CVTEM_CONTENTS7  0xdbc
#define PKT5_EMP_CVTEM_CONTENTS0  0xdc0
#define PKT5_EMP_CVTEM_CONTENTS1  0xdc4
#define PKT5_EMP_CVTEM_CONTENTS2  0xdc8
#define PKT5_EMP_CVTEM_CONTENTS3  0xdcc
#define PKT5_EMP_CVTEM_CONTENTS4  0xdd0
#define PKT5_EMP_CVTEM_CONTENTS5  0xdd4
#define PKT5_EMP_CVTEM_CONTENTS6  0xdd8
#define PKT5_EMP_CVTEM_CONTENTS7  0xddc
/* Audio Packetizer Registers */
#define AUDPKT_CONTROL0              0xe20
#define AUDPKT_CHSTATUS_OVR_EN_MASK  BIT(0)
#define AUDPKT_CHSTATUS_OVR_EN       BIT(0)
#define AUDPKT_CONTROL1              0xe24
#define AUDPKT_ACR_CONTROL0          0xe40
#define AUDPKT_ACR_N_VALUE           0xfffff
#define AUDPKT_ACR_CONTROL1          0xe44
#define AUDPKT_ACR_CTS_OVR_VAL_MSK   GENMASK(23, 4)
#define AUDPKT_ACR_CTS_OVR_VAL(x)  ((x) << 4)
#define AUDPKT_ACR_CTS_OVR_EN_MSK  BIT(1)
#define AUDPKT_ACR_CTS_OVR_EN      BIT(1)
#define AUDPKT_ACR_STATUS0         0xe4c
#define AUDPKT_CHSTATUS_OVR0       0xe60
#define AUDPKT_CHSTATUS_OVR1       0xe64
/* IEC60958 Byte 3: Sampleing frenuency Bits 24 to 27 */
#define AUDPKT_CHSTATUS_SR_MASK           GENMASK(3, 0)
#define AUDPKT_CHSTATUS_SR_22050          0x4
#define AUDPKT_CHSTATUS_SR_24000          0x6
#define AUDPKT_CHSTATUS_SR_32000          0x3
#define AUDPKT_CHSTATUS_SR_44100          0x0
#define AUDPKT_CHSTATUS_SR_48000          0x2
#define AUDPKT_CHSTATUS_SR_88200          0x8
#define AUDPKT_CHSTATUS_SR_96000          0xa
#define AUDPKT_CHSTATUS_SR_176400         0xc
#define AUDPKT_CHSTATUS_SR_192000         0xe
#define AUDPKT_CHSTATUS_SR_768000         0x9
#define AUDPKT_CHSTATUS_SR_NOT_INDICATED  0x1
/* IEC60958 Byte 4: Original Sampleing frenuency Bits 36 to 39 */
#define AUDPKT_CHSTATUS_0SR_MASK           GENMASK(15, 12)
#define AUDPKT_CHSTATUS_OSR_8000           0x6
#define AUDPKT_CHSTATUS_OSR_11025          0xa
#define AUDPKT_CHSTATUS_OSR_12000          0x2
#define AUDPKT_CHSTATUS_OSR_16000          0x8
#define AUDPKT_CHSTATUS_OSR_22050          0xb
#define AUDPKT_CHSTATUS_OSR_24000          0x9
#define AUDPKT_CHSTATUS_OSR_32000          0xc
#define AUDPKT_CHSTATUS_OSR_44100          0xf
#define AUDPKT_CHSTATUS_OSR_48000          0xd
#define AUDPKT_CHSTATUS_OSR_88200          0x7
#define AUDPKT_CHSTATUS_OSR_96000          0x5
#define AUDPKT_CHSTATUS_OSR_176400         0x3
#define AUDPKT_CHSTATUS_OSR_192000         0x1
#define AUDPKT_CHSTATUS_OSR_NOT_INDICATED  0x0
#define AUDPKT_CHSTATUS_OVR2               0xe68
#define AUDPKT_CHSTATUS_OVR3               0xe6c
#define AUDPKT_CHSTATUS_OVR4               0xe70
#define AUDPKT_CHSTATUS_OVR5               0xe74
#define AUDPKT_CHSTATUS_OVR6               0xe78
#define AUDPKT_CHSTATUS_OVR7               0xe7c
#define AUDPKT_CHSTATUS_OVR8               0xe80
#define AUDPKT_CHSTATUS_OVR9               0xe84
#define AUDPKT_CHSTATUS_OVR10              0xe88
#define AUDPKT_CHSTATUS_OVR11              0xe8c
#define AUDPKT_CHSTATUS_OVR12              0xe90
#define AUDPKT_CHSTATUS_OVR13              0xe94
#define AUDPKT_CHSTATUS_OVR14              0xe98
#define AUDPKT_USRDATA_OVR_MSG_GENERIC0    0xea0
#define AUDPKT_USRDATA_OVR_MSG_GENERIC1    0xea4
#define AUDPKT_USRDATA_OVR_MSG_GENERIC2    0xea8
#define AUDPKT_USRDATA_OVR_MSG_GENERIC3    0xeac
#define AUDPKT_USRDATA_OVR_MSG_GENERIC4    0xeb0
#define AUDPKT_USRDATA_OVR_MSG_GENERIC5    0xeb4
#define AUDPKT_USRDATA_OVR_MSG_GENERIC6    0xeb8
#define AUDPKT_USRDATA_OVR_MSG_GENERIC7    0xebc
#define AUDPKT_USRDATA_OVR_MSG_GENERIC8    0xec0
#define AUDPKT_USRDATA_OVR_MSG_GENERIC9    0xec4
#define AUDPKT_USRDATA_OVR_MSG_GENERIC10   0xec8
#define AUDPKT_USRDATA_OVR_MSG_GENERIC11   0xecc
#define AUDPKT_USRDATA_OVR_MSG_GENERIC12   0xed0
#define AUDPKT_USRDATA_OVR_MSG_GENERIC13   0xed4
#define AUDPKT_USRDATA_OVR_MSG_GENERIC14   0xed8
#define AUDPKT_USRDATA_OVR_MSG_GENERIC15   0xedc
#define AUDPKT_USRDATA_OVR_MSG_GENERIC16   0xee0
#define AUDPKT_USRDATA_OVR_MSG_GENERIC17   0xee4
#define AUDPKT_USRDATA_OVR_MSG_GENERIC18   0xee8
#define AUDPKT_USRDATA_OVR_MSG_GENERIC19   0xeec
#define AUDPKT_USRDATA_OVR_MSG_GENERIC20   0xef0
#define AUDPKT_USRDATA_OVR_MSG_GENERIC21   0xef4
#define AUDPKT_USRDATA_OVR_MSG_GENERIC22   0xef8
#define AUDPKT_USRDATA_OVR_MSG_GENERIC23   0xefc
#define AUDPKT_USRDATA_OVR_MSG_GENERIC24   0xf00
#define AUDPKT_USRDATA_OVR_MSG_GENERIC25   0xf04
#define AUDPKT_USRDATA_OVR_MSG_GENERIC26   0xf08
#define AUDPKT_USRDATA_OVR_MSG_GENERIC27   0xf0c
#define AUDPKT_USRDATA_OVR_MSG_GENERIC28   0xf10
#define AUDPKT_USRDATA_OVR_MSG_GENERIC29   0xf14
#define AUDPKT_USRDATA_OVR_MSG_GENERIC30   0xf18
#define AUDPKT_USRDATA_OVR_MSG_GENERIC31   0xf1c
#define AUDPKT_USRDATA_OVR_MSG_GENERIC32   0xf20
#define AUDPKT_VBIT_OVR0                   0xf24
/* CEC Registers */
#define CEC_TX_CONTROL             0x1000
#define CEC_STATUS                 0x1004
#define CEC_CONFIG                 0x1008
#define CEC_ADDR                   0x100c
#define CEC_TX_COUNT               0x1020
#define CEC_TX_DATA3_0             0x1024
#define CEC_TX_DATA7_4             0x1028
#define CEC_TX_DATA11_8            0x102c
#define CEC_TX_DATA15_12           0x1030
#define CEC_RX_COUNT_STATUS        0x1040
#define CEC_RX_DATA3_0             0x1044
#define CEC_RX_DATA7_4             0x1048
#define CEC_RX_DATA11_8            0x104c
#define CEC_RX_DATA15_12           0x1050
#define CEC_LOCK_CONTROL           0x1054
#define CEC_RXQUAL_BITTIME_CONFIG  0x1060
#define CEC_RX_BITTIME_CONFIG      0x1064
#define CEC_TX_BITTIME_CONFIG      0x1068
/* eARC RX CMDC Registers */
#define EARCRX_CMDC_CONFIG0                    0x1800
#define EARCRX_XACTREAD_STOP_CFG               BIT(26)
#define EARCRX_XACTREAD_RETRY_CFG              BIT(25)
#define EARCRX_CMDC_DSCVR_EARCVALID0_TO_DISC1  BIT(24)
#define EARCRX_CMDC_XACT_RESTART_EN            BIT(18)
#define EARCRX_CMDC_CONFIG1                    0x1804
#define EARCRX_CMDC_CONTROL                    0x1808
#define EARCRX_CMDC_HEARTBEAT_LOSS_EN          BIT(4)
#define EARCRX_CMDC_DISCOVERY_EN               BIT(3)
#define EARCRX_CONNECTOR_HPD                   BIT(1)
#define EARCRX_CMDC_WHITELIST0_CONFIG          0x180c
#define EARCRX_CMDC_WHITELIST1_CONFIG          0x1810
#define EARCRX_CMDC_WHITELIST2_CONFIG          0x1814
#define EARCRX_CMDC_WHITELIST3_CONFIG          0x1818
#define EARCRX_CMDC_STATUS                     0x181c
#define EARCRX_CMDC_XACT_INFO                  0x1820
#define EARCRX_CMDC_XACT_ACTION                0x1824
#define EARCRX_CMDC_HEARTBEAT_RXSTAT_SE        0x1828
#define EARCRX_CMDC_HEARTBEAT_STATUS           0x182c
#define EARCRX_CMDC_XACT_WR0                   0x1840
#define EARCRX_CMDC_XACT_WR1                   0x1844
#define EARCRX_CMDC_XACT_WR2                   0x1848
#define EARCRX_CMDC_XACT_WR3                   0x184c
#define EARCRX_CMDC_XACT_WR4                   0x1850
#define EARCRX_CMDC_XACT_WR5                   0x1854
#define EARCRX_CMDC_XACT_WR6                   0x1858
#define EARCRX_CMDC_XACT_WR7                   0x185c
#define EARCRX_CMDC_XACT_WR8                   0x1860
#define EARCRX_CMDC_XACT_WR9                   0x1864
#define EARCRX_CMDC_XACT_WR10                  0x1868
#define EARCRX_CMDC_XACT_WR11                  0x186c
#define EARCRX_CMDC_XACT_WR12                  0x1870
#define EARCRX_CMDC_XACT_WR13                  0x1874
#define EARCRX_CMDC_XACT_WR14                  0x1878
#define EARCRX_CMDC_XACT_WR15                  0x187c
#define EARCRX_CMDC_XACT_WR16                  0x1880
#define EARCRX_CMDC_XACT_WR17                  0x1884
#define EARCRX_CMDC_XACT_WR18                  0x1888
#define EARCRX_CMDC_XACT_WR19                  0x188c
#define EARCRX_CMDC_XACT_WR20                  0x1890
#define EARCRX_CMDC_XACT_WR21                  0x1894
#define EARCRX_CMDC_XACT_WR22                  0x1898
#define EARCRX_CMDC_XACT_WR23                  0x189c
#define EARCRX_CMDC_XACT_WR24                  0x18a0
#define EARCRX_CMDC_XACT_WR25                  0x18a4
#define EARCRX_CMDC_XACT_WR26                  0x18a8
#define EARCRX_CMDC_XACT_WR27                  0x18ac
#define EARCRX_CMDC_XACT_WR28                  0x18b0
#define EARCRX_CMDC_XACT_WR29                  0x18b4
#define EARCRX_CMDC_XACT_WR30                  0x18b8
#define EARCRX_CMDC_XACT_WR31                  0x18bc
#define EARCRX_CMDC_XACT_WR32                  0x18c0
#define EARCRX_CMDC_XACT_WR33                  0x18c4
#define EARCRX_CMDC_XACT_WR34                  0x18c8
#define EARCRX_CMDC_XACT_WR35                  0x18cc
#define EARCRX_CMDC_XACT_WR36                  0x18d0
#define EARCRX_CMDC_XACT_WR37                  0x18d4
#define EARCRX_CMDC_XACT_WR38                  0x18d8
#define EARCRX_CMDC_XACT_WR39                  0x18dc
#define EARCRX_CMDC_XACT_WR40                  0x18e0
#define EARCRX_CMDC_XACT_WR41                  0x18e4
#define EARCRX_CMDC_XACT_WR42                  0x18e8
#define EARCRX_CMDC_XACT_WR43                  0x18ec
#define EARCRX_CMDC_XACT_WR44                  0x18f0
#define EARCRX_CMDC_XACT_WR45                  0x18f4
#define EARCRX_CMDC_XACT_WR46                  0x18f8
#define EARCRX_CMDC_XACT_WR47                  0x18fc
#define EARCRX_CMDC_XACT_WR48                  0x1900
#define EARCRX_CMDC_XACT_WR49                  0x1904
#define EARCRX_CMDC_XACT_WR50                  0x1908
#define EARCRX_CMDC_XACT_WR51                  0x190c
#define EARCRX_CMDC_XACT_WR52                  0x1910
#define EARCRX_CMDC_XACT_WR53                  0x1914
#define EARCRX_CMDC_XACT_WR54                  0x1918
#define EARCRX_CMDC_XACT_WR55                  0x191c
#define EARCRX_CMDC_XACT_WR56                  0x1920
#define EARCRX_CMDC_XACT_WR57                  0x1924
#define EARCRX_CMDC_XACT_WR58                  0x1928
#define EARCRX_CMDC_XACT_WR59                  0x192c
#define EARCRX_CMDC_XACT_WR60                  0x1930
#define EARCRX_CMDC_XACT_WR61                  0x1934
#define EARCRX_CMDC_XACT_WR62                  0x1938
#define EARCRX_CMDC_XACT_WR63                  0x193c
#define EARCRX_CMDC_XACT_WR64                  0x1940
#define EARCRX_CMDC_XACT_RD0                   0x1960
#define EARCRX_CMDC_XACT_RD1                   0x1964
#define EARCRX_CMDC_XACT_RD2                   0x1968
#define EARCRX_CMDC_XACT_RD3                   0x196c
#define EARCRX_CMDC_XACT_RD4                   0x1970
#define EARCRX_CMDC_XACT_RD5                   0x1974
#define EARCRX_CMDC_XACT_RD6                   0x1978
#define EARCRX_CMDC_XACT_RD7                   0x197c
#define EARCRX_CMDC_XACT_RD8                   0x1980
#define EARCRX_CMDC_XACT_RD9                   0x1984
#define EARCRX_CMDC_XACT_RD10                  0x1988
#define EARCRX_CMDC_XACT_RD11                  0x198c
#define EARCRX_CMDC_XACT_RD12                  0x1990
#define EARCRX_CMDC_XACT_RD13                  0x1994
#define EARCRX_CMDC_XACT_RD14                  0x1998
#define EARCRX_CMDC_XACT_RD15                  0x199c
#define EARCRX_CMDC_XACT_RD16                  0x19a0
#define EARCRX_CMDC_XACT_RD17                  0x19a4
#define EARCRX_CMDC_XACT_RD18                  0x19a8
#define EARCRX_CMDC_XACT_RD19                  0x19ac
#define EARCRX_CMDC_XACT_RD20                  0x19b0
#define EARCRX_CMDC_XACT_RD21                  0x19b4
#define EARCRX_CMDC_XACT_RD22                  0x19b8
#define EARCRX_CMDC_XACT_RD23                  0x19bc
#define EARCRX_CMDC_XACT_RD24                  0x19c0
#define EARCRX_CMDC_XACT_RD25                  0x19c4
#define EARCRX_CMDC_XACT_RD26                  0x19c8
#define EARCRX_CMDC_XACT_RD27                  0x19cc
#define EARCRX_CMDC_XACT_RD28                  0x19d0
#define EARCRX_CMDC_XACT_RD29                  0x19d4
#define EARCRX_CMDC_XACT_RD30                  0x19d8
#define EARCRX_CMDC_XACT_RD31                  0x19dc
#define EARCRX_CMDC_XACT_RD32                  0x19e0
#define EARCRX_CMDC_XACT_RD33                  0x19e4
#define EARCRX_CMDC_XACT_RD34                  0x19e8
#define EARCRX_CMDC_XACT_RD35                  0x19ec
#define EARCRX_CMDC_XACT_RD36                  0x19f0
#define EARCRX_CMDC_XACT_RD37                  0x19f4
#define EARCRX_CMDC_XACT_RD38                  0x19f8
#define EARCRX_CMDC_XACT_RD39                  0x19fc
#define EARCRX_CMDC_XACT_RD40                  0x1a00
#define EARCRX_CMDC_XACT_RD41                  0x1a04
#define EARCRX_CMDC_XACT_RD42                  0x1a08
#define EARCRX_CMDC_XACT_RD43                  0x1a0c
#define EARCRX_CMDC_XACT_RD44                  0x1a10
#define EARCRX_CMDC_XACT_RD45                  0x1a14
#define EARCRX_CMDC_XACT_RD46                  0x1a18
#define EARCRX_CMDC_XACT_RD47                  0x1a1c
#define EARCRX_CMDC_XACT_RD48                  0x1a20
#define EARCRX_CMDC_XACT_RD49                  0x1a24
#define EARCRX_CMDC_XACT_RD50                  0x1a28
#define EARCRX_CMDC_XACT_RD51                  0x1a2c
#define EARCRX_CMDC_XACT_RD52                  0x1a30
#define EARCRX_CMDC_XACT_RD53                  0x1a34
#define EARCRX_CMDC_XACT_RD54                  0x1a38
#define EARCRX_CMDC_XACT_RD55                  0x1a3c
#define EARCRX_CMDC_XACT_RD56                  0x1a40
#define EARCRX_CMDC_XACT_RD57                  0x1a44
#define EARCRX_CMDC_XACT_RD58                  0x1a48
#define EARCRX_CMDC_XACT_RD59                  0x1a4c
#define EARCRX_CMDC_XACT_RD60                  0x1a50
#define EARCRX_CMDC_XACT_RD61                  0x1a54
#define EARCRX_CMDC_XACT_RD62                  0x1a58
#define EARCRX_CMDC_XACT_RD63                  0x1a5c
#define EARCRX_CMDC_XACT_RD64                  0x1a60
#define EARCRX_CMDC_SYNC_CONFIG                0x1b00
/* eARC RX DMAC Registers */
#define EARCRX_DMAC_PHY_CONTROL                   0x1c00
#define EARCRX_DMAC_CONFIG                        0x1c08
#define EARCRX_DMAC_CONTROL0                      0x1c0c
#define EARCRX_DMAC_AUDIO_EN                      BIT(1)
#define EARCRX_DMAC_EN                            BIT(0)
#define EARCRX_DMAC_CONTROL1                      0x1c10
#define EARCRX_DMAC_STATUS                        0x1c14
#define EARCRX_DMAC_CHSTATUS0                     0x1c18
#define EARCRX_DMAC_CHSTATUS1                     0x1c1c
#define EARCRX_DMAC_CHSTATUS2                     0x1c20
#define EARCRX_DMAC_CHSTATUS3                     0x1c24
#define EARCRX_DMAC_CHSTATUS4                     0x1c28
#define EARCRX_DMAC_CHSTATUS5                     0x1c2c
#define EARCRX_DMAC_USRDATA_MSG_HDMI_AC0          0x1c30
#define EARCRX_DMAC_USRDATA_MSG_HDMI_AC1          0x1c34
#define EARCRX_DMAC_USRDATA_MSG_HDMI_AC2          0x1c38
#define EARCRX_DMAC_USRDATA_MSG_HDMI_AC3          0x1c3c
#define EARCRX_DMAC_USRDATA_MSG_HDMI_AC4          0x1c40
#define EARCRX_DMAC_USRDATA_MSG_HDMI_AC5          0x1c44
#define EARCRX_DMAC_USRDATA_MSG_HDMI_AC6          0x1c48
#define EARCRX_DMAC_USRDATA_MSG_HDMI_AC7          0x1c4c
#define EARCRX_DMAC_USRDATA_MSG_HDMI_AC8          0x1c50
#define EARCRX_DMAC_USRDATA_MSG_HDMI_AC9          0x1c54
#define EARCRX_DMAC_USRDATA_MSG_HDMI_AC10         0x1c58
#define EARCRX_DMAC_USRDATA_MSG_HDMI_AC11         0x1c5c
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC1_PKT0   0x1c60
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC1_PKT1   0x1c64
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC1_PKT2   0x1c68
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC1_PKT3   0x1c6c
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC1_PKT4   0x1c70
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC1_PKT5   0x1c74
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC1_PKT6   0x1c78
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC1_PKT7   0x1c7c
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC1_PKT8   0x1c80
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC1_PKT9   0x1c84
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC1_PKT10  0x1c88
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC1_PKT11  0x1c8c
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC2_PKT0   0x1c90
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC2_PKT1   0x1c94
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC2_PKT2   0x1c98
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC2_PKT3   0x1c9c
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC2_PKT4   0x1ca0
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC2_PKT5   0x1ca4
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC2_PKT6   0x1ca8
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC2_PKT7   0x1cac
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC2_PKT8   0x1cb0
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC2_PKT9   0x1cb4
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC2_PKT10  0x1cb8
#define EARCRX_DMAC_USRDATA_MSG_HDMI_ISRC2_PKT11  0x1cbc
#define EARCRX_DMAC_USRDATA_MSG_GENERIC0          0x1cc0
#define EARCRX_DMAC_USRDATA_MSG_GENERIC1          0x1cc4
#define EARCRX_DMAC_USRDATA_MSG_GENERIC2          0x1cc8
#define EARCRX_DMAC_USRDATA_MSG_GENERIC3          0x1ccc
#define EARCRX_DMAC_USRDATA_MSG_GENERIC4          0x1cd0
#define EARCRX_DMAC_USRDATA_MSG_GENERIC5          0x1cd4
#define EARCRX_DMAC_USRDATA_MSG_GENERIC6          0x1cd8
#define EARCRX_DMAC_USRDATA_MSG_GENERIC7          0x1cdc
#define EARCRX_DMAC_USRDATA_MSG_GENERIC8          0x1ce0
#define EARCRX_DMAC_USRDATA_MSG_GENERIC9          0x1ce4
#define EARCRX_DMAC_USRDATA_MSG_GENERIC10         0x1ce8
#define EARCRX_DMAC_USRDATA_MSG_GENERIC11         0x1cec
#define EARCRX_DMAC_USRDATA_MSG_GENERIC12         0x1cf0
#define EARCRX_DMAC_USRDATA_MSG_GENERIC13         0x1cf4
#define EARCRX_DMAC_USRDATA_MSG_GENERIC14         0x1cf8
#define EARCRX_DMAC_USRDATA_MSG_GENERIC15         0x1cfc
#define EARCRX_DMAC_USRDATA_MSG_GENERIC16         0x1d00
#define EARCRX_DMAC_USRDATA_MSG_GENERIC17         0x1d04
#define EARCRX_DMAC_USRDATA_MSG_GENERIC18         0x1d08
#define EARCRX_DMAC_USRDATA_MSG_GENERIC19         0x1d0c
#define EARCRX_DMAC_USRDATA_MSG_GENERIC20         0x1d10
#define EARCRX_DMAC_USRDATA_MSG_GENERIC21         0x1d14
#define EARCRX_DMAC_USRDATA_MSG_GENERIC22         0x1d18
#define EARCRX_DMAC_USRDATA_MSG_GENERIC23         0x1d1c
#define EARCRX_DMAC_USRDATA_MSG_GENERIC24         0x1d20
#define EARCRX_DMAC_USRDATA_MSG_GENERIC25         0x1d24
#define EARCRX_DMAC_USRDATA_MSG_GENERIC26         0x1d28
#define EARCRX_DMAC_USRDATA_MSG_GENERIC27         0x1d2c
#define EARCRX_DMAC_USRDATA_MSG_GENERIC28         0x1d30
#define EARCRX_DMAC_USRDATA_MSG_GENERIC29         0x1d34
#define EARCRX_DMAC_USRDATA_MSG_GENERIC30         0x1d38
#define EARCRX_DMAC_USRDATA_MSG_GENERIC31         0x1d3c
#define EARCRX_DMAC_USRDATA_MSG_GENERIC32         0x1d40
#define EARCRX_DMAC_CHSTATUS_STREAMER0            0x1d44
#define EARCRX_DMAC_CHSTATUS_STREAMER1            0x1d48
#define EARCRX_DMAC_CHSTATUS_STREAMER2            0x1d4c
#define EARCRX_DMAC_CHSTATUS_STREAMER3            0x1d50
#define EARCRX_DMAC_CHSTATUS_STREAMER4            0x1d54
#define EARCRX_DMAC_CHSTATUS_STREAMER5            0x1d58
#define EARCRX_DMAC_CHSTATUS_STREAMER6            0x1d5c
#define EARCRX_DMAC_CHSTATUS_STREAMER7            0x1d60
#define EARCRX_DMAC_CHSTATUS_STREAMER8            0x1d64
#define EARCRX_DMAC_CHSTATUS_STREAMER9            0x1d68
#define EARCRX_DMAC_CHSTATUS_STREAMER10           0x1d6c
#define EARCRX_DMAC_CHSTATUS_STREAMER11           0x1d70
#define EARCRX_DMAC_CHSTATUS_STREAMER12           0x1d74
#define EARCRX_DMAC_CHSTATUS_STREAMER13           0x1d78
#define EARCRX_DMAC_CHSTATUS_STREAMER14           0x1d7c
#define EARCRX_DMAC_USRDATA_STREAMER0             0x1d80
/* Main Unit Interrupt Registers */
#define MAIN_INTVEC_INDEX         0x3000
#define MAINUNIT_0_INT_STATUS     0x3010
#define MAINUNIT_0_INT_MASK_N     0x3014
#define MAINUNIT_0_INT_CLEAR      0x3018
#define MAINUNIT_0_INT_FORCE      0x301c
#define MAINUNIT_1_INT_STATUS     0x3020
#define FLT_EXIT_TO_LTSL_IRQ      BIT(22)
#define FLT_EXIT_TO_LTS4_IRQ      BIT(21)
#define FLT_EXIT_TO_LTSP_IRQ      BIT(20)
#define SCDC_NACK_RCVD_IRQ        BIT(12)
#define SCDC_RR_REPLY_STOP_IRQ    BIT(11)
#define SCDC_UPD_FLAGS_CLR_IRQ    BIT(10)
#define SCDC_UPD_FLAGS_CHG_IRQ    BIT(9)
#define SCDC_UPD_FLAGS_RD_IRQ     BIT(8)
#define I2CM_NACK_RCVD_IRQ        BIT(2)
#define I2CM_READ_REQUEST_IRQ     BIT(1)
#define I2CM_OP_DONE_IRQ          BIT(0)
#define MAINUNIT_1_INT_MASK_N     0x3024
#define I2CM_NACK_RCVD_MASK_N     BIT(2)
#define I2CM_READ_REQUEST_MASK_N  BIT(1)
#define I2CM_OP_DONE_MASK_N       BIT(0)
#define MAINUNIT_1_INT_CLEAR      0x3028
#define I2CM_NACK_RCVD_CLEAR      BIT(2)
#define I2CM_READ_REQUEST_CLEAR   BIT(1)
#define I2CM_OP_DONE_CLEAR        BIT(0)
#define MAINUNIT_1_INT_FORCE      0x302c
/* AVPUNIT Interrupt Registers */
#define AVP_INTVEC_INDEX        0x3800
#define AVP_0_INT_STATUS        0x3810
#define AVP_0_INT_MASK_N        0x3814
#define AVP_0_INT_CLEAR         0x3818
#define AVP_0_INT_FORCE         0x381c
#define AVP_1_INT_STATUS        0x3820
#define AVP_1_INT_MASK_N        0x3824
#define HDCP14_AUTH_CHG_MASK_N  BIT(6)
#define AVP_1_INT_CLEAR         0x3828
#define AVP_1_INT_FORCE         0x382c
#define AVP_2_INT_STATUS        0x3830
#define AVP_2_INT_MASK_N        0x3834
#define AVP_2_INT_CLEAR         0x3838
#define AVP_2_INT_FORCE         0x383c
#define AVP_3_INT_STATUS        0x3840
#define AVP_3_INT_MASK_N        0x3844
#define AVP_3_INT_CLEAR         0x3848
#define AVP_3_INT_FORCE         0x384c
#define AVP_4_INT_STATUS        0x3850
#define AVP_4_INT_MASK_N        0x3854
#define AVP_4_INT_CLEAR         0x3858
#define AVP_4_INT_FORCE         0x385c
#define AVP_5_INT_STATUS        0x3860
#define AVP_5_INT_MASK_N        0x3864
#define AVP_5_INT_CLEAR         0x3868
#define AVP_5_INT_FORCE         0x386c
#define AVP_6_INT_STATUS        0x3870
#define AVP_6_INT_MASK_N        0x3874
#define AVP_6_INT_CLEAR         0x3878
#define AVP_6_INT_FORCE         0x387c
/* CEC Interrupt Registers */
#define CEC_INT_STATUS  0x4000
#define CEC_INT_MASK_N  0x4004
#define CEC_INT_CLEAR   0x4008
#define CEC_INT_FORCE   0x400c
/* eARC RX Interrupt Registers  */
#define EARCRX_INTVEC_INDEX                0x4800
#define EARCRX_0_INT_STATUS                0x4810
#define EARCRX_CMDC_DISCOVERY_TIMEOUT_IRQ  BIT(9)
#define EARCRX_CMDC_DISCOVERY_DONE_IRQ     BIT(8)
#define EARCRX_0_INT_MASK_N                0x4814
#define EARCRX_0_INT_CLEAR                 0x4818
#define EARCRX_0_INT_FORCE                 0x481c
#define EARCRX_1_INT_STATUS                0x4820
#define EARCRX_1_INT_MASK_N                0x4824
#define EARCRX_1_INT_CLEAR                 0x4828
#define EARCRX_1_INT_FORCE                 0x482c

/* SCDC Registers */
#define SCDC_SINK_VERSION    0x01
#define SCDC_SOURCE_VERSION  0x02

#define SCDC_UPDATE_0           0x10
#define SCDC_READ_REQUEST_TEST  BIT(2)
#define SCDC_CED_UPDATE         BIT(1)
#define SCDC_STATUS_UPDATE      BIT(0)
#define SCDC_UPDATE_1           0x11

#define SCDC_TMDS_CONFIG                 0x20
#define SCDC_TMDS_BIT_CLOCK_RATIO_BY_40  BIT(1)
#define SCDC_TMDS_BIT_CLOCK_RATIO_BY_10  (0 << 1)
#define SCDC_SCRAMBLING_ENABLE           BIT(0)
#define SCDC_SCRAMBLER_STATUS            0x21
#define SCDC_SCRAMBLING_STATUS           BIT(0)

#define SCDC_CONFIG_0             0x30
#define SCDC_READ_REQUEST_ENABLE  BIT(0)

#define SCDC_STATUS_FLAGS_0  0x40
#define SCDC_CH2_LOCK        BIT(3)
#define SCDC_CH1_LOCK        BIT(2)
#define SCDC_CH0_LOCK        BIT(1)
#define SCDC_CH_LOCK_MASK    (SCDC_CH2_LOCK | SCDC_CH1_LOCK | SCDC_CH0_LOCK)
#define SCDC_CLOCK_DETECT    BIT(0)
#define SCDC_STATUS_FLAGS_1  0x41

#define SCDC_ERR_DET_0_L       0x50
#define SCDC_ERR_DET_0_H       0x51
#define SCDC_ERR_DET_1_L       0x52
#define SCDC_ERR_DET_1_H       0x53
#define SCDC_ERR_DET_2_L       0x54
#define SCDC_ERR_DET_2_H       0x55
#define SCDC_CHANNEL_VALID     BIT(7)
#define SCDC_ERR_DET_CHECKSUM  0x56

#define SCDC_TEST_CONFIG_0      0xc0
#define SCDC_TEST_READ_REQUEST  BIT(7)
#define SCDC_TEST_READ_REQUEST_DELAY(x)  ((x) & 0x7f)

#define SCDC_MANUFACTURER_IEEE_OUI       0xd0
#define SCDC_MANUFACTURER_IEEE_OUI_SIZE  3
#define SCDC_DEVICE_ID                   0xd3
#define SCDC_DEVICE_ID_SIZE              8
#define SCDC_DEVICE_HARDWARE_REVISION    0xdb
#define SCDC_DEVICE_HARDWARE_REVISION_MAJOR(x)  (((x) >> 4) & 0xf)
#define SCDC_DEVICE_HARDWARE_REVISION_MINOR(x)  (((x) >> 0) & 0xf)
#define SCDC_DEVICE_SOFTWARE_MAJOR_REVISION  0xdc
#define SCDC_DEVICE_SOFTWARE_MINOR_REVISION  0xdd

#define SCDC_MANUFACTURER_SPECIFIC       0xde
#define SCDC_MANUFACTURER_SPECIFIC_SIZE  34

#define HDMI0TX_PHY_BASE  0xFED60000
#define HDMI1TX_PHY_BASE  0xFED70000

#define HDPTXPHY0_GRF_BASE  0xFD5E0000
#define HDPTXPHY1_GRF_BASE  0xFD5E4000

#define PMU1CRU_SOFTRST_CON03  0xA0C
#define PMU1CRU_SOFTRST_CON04  0xA10

#define RK3588_SYS_GRF_BASE  0xFD58C000
#define RK3588_VO1_GRF_BASE  0xFD5A8000

/* Rockchip Htx Phy */

struct RockchipHdptxPhyHdmi {
  UINT32    Id;
};

struct DwHdmiQpI2c {
  UINT8      SlaveReg;
  BOOLEAN    IsSegment;
  BOOLEAN    IsRegAddr;
};

struct DwHdmiQpDevice {
  UINT32                         Signature;
  ROCKCHIP_CONNECTOR_PROTOCOL    Connector;
  UINT32                         Id;
  UINTN                          Base;
  UINT32                         OutputInterface;
  BOOLEAN                        ForceHpd;
  UINT8                          SignalingMode;
  struct DwHdmiQpI2c             I2c;

  struct RockchipHdptxPhyHdmi    HdptxPhy;
};

#define DW_HDMI_QP_SIGNATURE  SIGNATURE_32 ('D', 'W', 'h', 'D')

#define DW_HDMI_QP_FROM_CONNECTOR_PROTOCOL(a) \
  CR (a, struct DwHdmiQpDevice, Connector, DW_HDMI_QP_SIGNATURE)

struct i2c_msg {
  UINT16    addr;
  UINT16    flags;
  #define I2C_M_RD            0x0001    /* guaranteed to be 0x0001! */
  #define I2C_M_TEN           0x0010    /* use only if I2C_FUNC_10BIT_ADDR */
  #define I2C_M_DMA_SAFE      0x0200    /* use only in kernel space */
  #define I2C_M_RECV_LEN      0x0400    /* use only if I2C_FUNC_SMBUS_READ_BLOCK_DATA */
  #define I2C_M_NO_RD_ACK     0x0800    /* use only if I2C_FUNC_PROTOCOL_MANGLING */
  #define I2C_M_IGNORE_NAK    0x1000    /* use only if I2C_FUNC_PROTOCOL_MANGLING */
  #define I2C_M_REV_DIR_ADDR  0x2000    /* use only if I2C_FUNC_PROTOCOL_MANGLING */
  #define I2C_M_NOSTART       0x4000    /* use only if I2C_FUNC_NOSTART */
  #define I2C_M_STOP          0x8000    /* use only if I2C_FUNC_PROTOCOL_MANGLING */
  UINT16    len;
  UINT8     *buf;
};

EFI_STATUS
DwHdmiQpConnectorPreInit (
  OUT ROCKCHIP_CONNECTOR_PROTOCOL  *This,
  OUT DISPLAY_STATE                *DisplayState
  );

EFI_STATUS
DwHdmiQpConnectorInit (
  OUT ROCKCHIP_CONNECTOR_PROTOCOL  *This,
  OUT DISPLAY_STATE                *DisplayState
  );

EFI_STATUS
DwHdmiQpConnectorGetEdid (
  OUT ROCKCHIP_CONNECTOR_PROTOCOL  *This,
  OUT DISPLAY_STATE                *DisplayState
  );

EFI_STATUS
DwHdmiQpConnectorEnable (
  OUT ROCKCHIP_CONNECTOR_PROTOCOL  *This,
  OUT DISPLAY_STATE                *DisplayState
  );

EFI_STATUS
DwHdmiQpConnectorDisable (
  OUT ROCKCHIP_CONNECTOR_PROTOCOL  *This,
  OUT DISPLAY_STATE                *DisplayState
  );

EFI_STATUS
DwHdmiQpConnectorDetect (
  OUT ROCKCHIP_CONNECTOR_PROTOCOL  *This,
  OUT DISPLAY_STATE                *DisplayState
  );

EFI_STATUS
HdptxRopllTmdsModeConfig (
  OUT struct RockchipHdptxPhyHdmi  *Hdptx,
  IN  UINT32                       BitRate
  );

EFI_STATUS
HdptxRopllCmnConfig (
  OUT struct RockchipHdptxPhyHdmi  *Hdptx,
  IN  UINT32                       BitRate
  );

#endif
