/** @file
  Vop2 DXE Driver, install RK_CRTC_PROTOCOL.

  Copyright (c) 2022 Rockchip Electronics Co. Ltd.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/RockchipDisplayLib.h>
#include <Library/CruLib.h>

#include <Library/MediaBusFormat.h>
#include <Library/DrmModes.h>

#include <Protocol/RockchipCrtcProtocol.h>

#include "Vop2Dxe.h"

STATIC VPS_CONFIG  mVpsConfigs[][VOP2_VP_MAX] = {
  {
    {
      .PlaneMask    = (1 << ROCKCHIP_VOP2_CLUSTER0 | 1 << ROCKCHIP_VOP2_ESMART0),
      .PrimaryPlane = ROCKCHIP_VOP2_ESMART0,
    },
    {
      .PlaneMask    = (1 << ROCKCHIP_VOP2_CLUSTER1 | 1 << ROCKCHIP_VOP2_ESMART1),
      .PrimaryPlane = ROCKCHIP_VOP2_ESMART1,
    },
    {
      .PlaneMask    = (1 << ROCKCHIP_VOP2_CLUSTER2 | 1 << ROCKCHIP_VOP2_ESMART2),
      .PrimaryPlane = ROCKCHIP_VOP2_ESMART2,
    },
    {
      .PlaneMask    = (1 << ROCKCHIP_VOP2_CLUSTER3 | 1 << ROCKCHIP_VOP2_ESMART3),
      .PrimaryPlane = ROCKCHIP_VOP2_ESMART3,
    },
  },
  {
    {
      .PlaneMask    = 0,
      .PrimaryPlane = 0,
    },
    {
      .PlaneMask = (1 << ROCKCHIP_VOP2_CLUSTER0 | 1 << ROCKCHIP_VOP2_ESMART0 |
        1 << ROCKCHIP_VOP2_CLUSTER1 | 1 << ROCKCHIP_VOP2_ESMART1),
      .PrimaryPlane = ROCKCHIP_VOP2_ESMART1,
    },
    {
      .PlaneMask = (1 << ROCKCHIP_VOP2_CLUSTER2 | 1 << ROCKCHIP_VOP2_ESMART2 |
        1 << ROCKCHIP_VOP2_CLUSTER3 | 1 << ROCKCHIP_VOP2_ESMART3),
      .PrimaryPlane = ROCKCHIP_VOP2_ESMART2,
    },
    {
      .PlaneMask    = 0,
      .PrimaryPlane = 0,
    },
  },
};

STATIC UINT32  mVpsConfigsSize = ARRAY_SIZE (mVpsConfigs);

STATIC CONST INT32  mSinTable[] = {
  0x00000000, 0x023be165, 0x04779632, 0x06b2f1d2, 0x08edc7b6, 0x0b27eb5c,
  0x0d61304d, 0x0f996a26, 0x11d06c96, 0x14060b67, 0x163a1a7d, 0x186c6ddd,
  0x1a9cd9ac, 0x1ccb3236, 0x1ef74bf2, 0x2120fb82, 0x234815ba, 0x256c6f9e,
  0x278dde6e, 0x29ac379f, 0x2bc750e8, 0x2ddf003f, 0x2ff31bdd, 0x32037a44,
  0x340ff241, 0x36185aee, 0x381c8bb5, 0x3a1c5c56, 0x3c17a4e7, 0x3e0e3ddb,
  0x3fffffff, 0x41ecc483, 0x43d464fa, 0x45b6bb5d, 0x4793a20f, 0x496af3e1,
  0x4b3c8c11, 0x4d084650, 0x4ecdfec6, 0x508d9210, 0x5246dd48, 0x53f9be04,
  0x55a6125a, 0x574bb8e5, 0x58ea90c2, 0x5a827999, 0x5c135399, 0x5d9cff82,
  0x5f1f5ea0, 0x609a52d1, 0x620dbe8a, 0x637984d3, 0x64dd894f, 0x6639b039,
  0x678dde6d, 0x68d9f963, 0x6a1de735, 0x6b598ea1, 0x6c8cd70a, 0x6db7a879,
  0x6ed9eba0, 0x6ff389de, 0x71046d3c, 0x720c8074, 0x730baeec, 0x7401e4bf,
  0x74ef0ebb, 0x75d31a5f, 0x76adf5e5, 0x777f903b, 0x7847d908, 0x7906c0af,
  0x79bc384c, 0x7a6831b8, 0x7b0a9f8c, 0x7ba3751c, 0x7c32a67c, 0x7cb82884,
  0x7d33f0c8, 0x7da5f5a3, 0x7e0e2e31, 0x7e6c924f, 0x7ec11aa3, 0x7f0bc095,
  0x7f4c7e52, 0x7f834ecf, 0x7fb02dc4, 0x7fd317b3, 0x7fec09e1, 0x7ffb025e,
  0x7fffffff
};

STATIC VOP2_VP_PLANE_MASK  mVpPlaneMaskRK3588[VOP2_VP_MAX][VOP2_VP_MAX] = {
  { /* one display policy */
    {/* main display */
      .PrimaryPlaneId   = ROCKCHIP_VOP2_ESMART0,
      .AttachedLayersNr = 8,
      .AttachedLayers   = {
        ROCKCHIP_VOP2_CLUSTER0, ROCKCHIP_VOP2_ESMART0,
        ROCKCHIP_VOP2_CLUSTER1, ROCKCHIP_VOP2_ESMART1,
        ROCKCHIP_VOP2_CLUSTER2, ROCKCHIP_VOP2_ESMART2,
        ROCKCHIP_VOP2_CLUSTER3, ROCKCHIP_VOP2_ESMART3
      },
    },
    { /* second display */ },
    { /* third  display */ },
    { /* fourth display */ },
  },

  { /* two display policy */
    {/* main display */
      .PrimaryPlaneId   = ROCKCHIP_VOP2_ESMART0,
      .AttachedLayersNr = 4,
      .AttachedLayers   = {
        ROCKCHIP_VOP2_CLUSTER0, ROCKCHIP_VOP2_ESMART0,
        ROCKCHIP_VOP2_CLUSTER1, ROCKCHIP_VOP2_ESMART1
      },
    },

    {/* second display */
      .PrimaryPlaneId   = ROCKCHIP_VOP2_ESMART2,
      .AttachedLayersNr = 4,
      .AttachedLayers   = {
        ROCKCHIP_VOP2_CLUSTER2, ROCKCHIP_VOP2_ESMART2,
        ROCKCHIP_VOP2_CLUSTER3, ROCKCHIP_VOP2_ESMART3
      },
    },
    { /* third  display */ },
    { /* fourth display */ },
  },

  { /* three display policy */
    {/* main display */
      .PrimaryPlaneId   = ROCKCHIP_VOP2_ESMART0,
      .AttachedLayersNr = 3,
      .AttachedLayers   = {
        ROCKCHIP_VOP2_CLUSTER0, ROCKCHIP_VOP2_ESMART0,
        ROCKCHIP_VOP2_CLUSTER1
      },
    },

    {/* second display */
      .PrimaryPlaneId   = ROCKCHIP_VOP2_ESMART1,
      .AttachedLayersNr = 3,
      .AttachedLayers   = {
        ROCKCHIP_VOP2_CLUSTER2, ROCKCHIP_VOP2_ESMART1,
        ROCKCHIP_VOP2_CLUSTER3
      },
    },

    {/* third  display */
      .PrimaryPlaneId   = ROCKCHIP_VOP2_ESMART2,
      .AttachedLayersNr = 2,
      .AttachedLayers   = { ROCKCHIP_VOP2_ESMART2, ROCKCHIP_VOP2_ESMART3  },
    },

    { /* fourth display */ },
  },

  { /* four display policy */
    {/* main display */
      .PrimaryPlaneId   = ROCKCHIP_VOP2_ESMART0,
      .AttachedLayersNr = 2,
      .AttachedLayers   = { ROCKCHIP_VOP2_CLUSTER0, ROCKCHIP_VOP2_ESMART0 },
    },

    {/* second display */
      .PrimaryPlaneId   = ROCKCHIP_VOP2_ESMART1,
      .AttachedLayersNr = 2,
      .AttachedLayers   = { ROCKCHIP_VOP2_CLUSTER1, ROCKCHIP_VOP2_ESMART1 },
    },

    {/* third  display */
      .PrimaryPlaneId   = ROCKCHIP_VOP2_ESMART2,
      .AttachedLayersNr = 2,
      .AttachedLayers   = { ROCKCHIP_VOP2_CLUSTER2, ROCKCHIP_VOP2_ESMART2 },
    },

    {/* fourth display */
      .PrimaryPlaneId   = ROCKCHIP_VOP2_ESMART3,
      .AttachedLayersNr = 2,
      .AttachedLayers   = { ROCKCHIP_VOP2_CLUSTER3, ROCKCHIP_VOP2_ESMART3 },
    },
  },
};

STATIC VOP2_VP_DATA  mVpDataRK3588[4] = {
  {
    .Feature       = VOP_FEATURE_OUTPUT_10BIT,
    .PreScanMaxDly = 42,
    .MaxDclk       = 600000,
    .MaxOutput     = { 7680, 4320 },
  },
  {
    .Feature       = VOP_FEATURE_OUTPUT_10BIT,
    .PreScanMaxDly = 40,
    .MaxDclk       = 600000,
    .MaxOutput     = { 4096, 2304 },
  },
  {
    .Feature       = VOP_FEATURE_OUTPUT_10BIT,
    .PreScanMaxDly = 52,
    .MaxDclk       = 600000,
    .MaxOutput     = { 4096, 2304 },
  },
  {
    .Feature       = 0,
    .PreScanMaxDly = 52,
    .MaxDclk       = 200000,
    .MaxOutput     = { 1920, 1080 },
  },
};

static VOP2_POWER_DOMAIN_DATA  mCluster0PdDataRK3588 = {
  .PdEnShift         = RK3588_CLUSTER0_PD_EN_SHIFT,
  .PdStatusShift     = RK3588_CLUSTER0_PD_STATUS_SHIFT,
  .PmuStatusShift    = RK3588_PD_CLUSTER0_PWR_STAT_SHIFI,
  .BisrEnStatusShift = RK3588_PD_CLUSTER0_REPAIR_EN_SHIFT,
};

static VOP2_POWER_DOMAIN_DATA  mCluster1PdDataRK3588 = {
  .PdEnShift         = RK3588_CLUSTER1_PD_EN_SHIFT,
  .PdStatusShift     = RK3588_CLUSTER1_PD_STATUS_SHIFT,
  .PmuStatusShift    = RK3588_PD_CLUSTER1_PWR_STAT_SHIFI,
  .BisrEnStatusShift = RK3588_PD_CLUSTER1_REPAIR_EN_SHIFT,
  .ParentPdData      = &mCluster0PdDataRK3588,
};

static VOP2_POWER_DOMAIN_DATA  mCluster2PdDataRK3588 = {
  .PdEnShift         = RK3588_CLUSTER2_PD_EN_SHIFT,
  .PdStatusShift     = RK3588_CLUSTER2_PD_STATUS_SHIFT,
  .PmuStatusShift    = RK3588_PD_CLUSTER2_PWR_STAT_SHIFI,
  .BisrEnStatusShift = RK3588_PD_CLUSTER2_REPAIR_EN_SHIFT,
  .ParentPdData      = &mCluster0PdDataRK3588,
};

static VOP2_POWER_DOMAIN_DATA  mCluster3PdDataRK3588 = {
  .PdEnShift         = RK3588_CLUSTER3_PD_EN_SHIFT,
  .PdStatusShift     = RK3588_CLUSTER3_PD_STATUS_SHIFT,
  .PmuStatusShift    = RK3588_PD_CLUSTER3_PWR_STAT_SHIFI,
  .BisrEnStatusShift = RK3588_PD_CLUSTER3_REPAIR_EN_SHIFT,
  .ParentPdData      = &mCluster0PdDataRK3588,
};

static VOP2_POWER_DOMAIN_DATA  mEsmartPdDataRK3588 = {
  .PdEnShift         = RK3588_ESMART_PD_EN_SHIFT,
  .PdStatusShift     = RK3588_ESMART_PD_STATUS_SHIFT,
  .PmuStatusShift    = RK3588_PD_ESMART_PWR_STAT_SHIFI,
  .BisrEnStatusShift = RK3588_PD_ESMART_REPAIR_EN_SHIFT,
};

static VOP2_POWER_DOMAIN_DATA  mDsc8KPdDataRK3588 = {
  .PdEnShift         = RK3588_DSC_8K_PD_EN_SHIFT,
  .PdStatusShift     = RK3588_DSC_8K_PD_STATUS_SHIFT,
  .PmuStatusShift    = RK3588_PD_DSC_8K_PWR_STAT_SHIFI,
  .BisrEnStatusShift = RK3588_PD_DSC_8K_REPAIR_EN_SHIFT,
};

static VOP2_POWER_DOMAIN_DATA  mDsc4KPdDataRK3588 = {
  .PdEnShift         = RK3588_DSC_4K_PD_EN_SHIFT,
  .PdStatusShift     = RK3588_DSC_4K_PD_STATUS_SHIFT,
  .PmuStatusShift    = RK3588_PD_DSC_4K_PWR_STAT_SHIFI,
  .BisrEnStatusShift = RK3588_PD_DSC_4K_REPAIR_EN_SHIFT,
};

static VOP2_WIN_DATA  mWinDataRK3588[8] = {
  {
    .Name             = "Cluster0",
    .PhysID           = ROCKCHIP_VOP2_CLUSTER0,
    .Type             = CLUSTER_LAYER,
    .WinSelPortOffset = 0,
    .LayerSelWinID    = 0,
    .RegOffset        = 0,
    .PdData           = &mCluster0PdDataRK3588,
  },

  {
    .Name             = "Cluster1",
    .PhysID           = ROCKCHIP_VOP2_CLUSTER1,
    .Type             = CLUSTER_LAYER,
    .WinSelPortOffset = 1,
    .LayerSelWinID    = 1,
    .RegOffset        = 0x200,
    .PdData           = &mCluster1PdDataRK3588,
  },

  {
    .Name             = "Cluster2",
    .PhysID           = ROCKCHIP_VOP2_CLUSTER2,
    .Type             = CLUSTER_LAYER,
    .WinSelPortOffset = 2,
    .LayerSelWinID    = 4,
    .RegOffset        = 0x400,
    .PdData           = &mCluster2PdDataRK3588,
  },

  {
    .Name             = "Cluster3",
    .PhysID           = ROCKCHIP_VOP2_CLUSTER3,
    .Type             = CLUSTER_LAYER,
    .WinSelPortOffset = 3,
    .LayerSelWinID    = 5,
    .RegOffset        = 0x600,
    .PdData           = &mCluster3PdDataRK3588,
  },

  {
    .Name             = "Esmart0",
    .PhysID           = ROCKCHIP_VOP2_ESMART0,
    .Type             = ESMART_LAYER,
    .WinSelPortOffset = 4,
    .LayerSelWinID    = 2,
    .RegOffset        = 0,
    .PdData           = &mEsmartPdDataRK3588,
  },

  {
    .Name             = "Esmart1",
    .PhysID           = ROCKCHIP_VOP2_ESMART1,
    .Type             = ESMART_LAYER,
    .WinSelPortOffset = 5,
    .LayerSelWinID    = 3,
    .RegOffset        = 0x200,
    .PdData           = &mEsmartPdDataRK3588,
  },

  {
    .Name             = "Esmart2",
    .PhysID           = ROCKCHIP_VOP2_ESMART2,
    .Type             = ESMART_LAYER,
    .WinSelPortOffset = 6,
    .LayerSelWinID    = 6,
    .RegOffset        = 0x400,
    .PdData           = &mEsmartPdDataRK3588,
  },

  {
    .Name             = "Esmart3",
    .PhysID           = ROCKCHIP_VOP2_ESMART3,
    .Type             = ESMART_LAYER,
    .WinSelPortOffset = 7,
    .LayerSelWinID    = 7,
    .RegOffset        = 0x600,
    .PdData           = &mEsmartPdDataRK3588,
  },
};

STATIC VOP2_DSC_DATA  mDscDataRK3588[] = {
  {
    .id                   = ROCKCHIP_VOP2_DSC_8K,
    .PdData               = &mDsc8KPdDataRK3588,
    .max_slice_num        = 8,
    .max_linebuf_depth    = 11,
    .min_bits_per_pixel   = 8,
    .dsc_txp_clk_src_name = "dsc_8k_txp_clk_src",
    .dsc_txp_clk_name     = "dsc_8k_txp_clk",
    .dsc_pxl_clk_name     = "dsc_8k_pxl_clk",
    .dsc_cds_clk_name     = "dsc_8k_cds_clk",
  },

  {
    .id                   = ROCKCHIP_VOP2_DSC_4K,
    .PdData               = &mDsc4KPdDataRK3588,
    .max_slice_num        = 2,
    .max_linebuf_depth    = 11,
    .min_bits_per_pixel   = 8,
    .dsc_txp_clk_src_name = "dsc_4k_txp_clk_src",
    .dsc_txp_clk_name     = "dsc_4k_txp_clk",
    .dsc_pxl_clk_name     = "dsc_4k_pxl_clk",
    .dsc_cds_clk_name     = "dsc_4k_cds_clk",
  },
};

STATIC VOP2_DATA  mVop2RK3588 = {
  .Version = VOP_VERSION_RK3588,
  .NrVps   = 4,
  .VpData  = mVpDataRK3588,
  .WinData = mWinDataRK3588,
  .DscData = mDscDataRK3588,

  /*
    .plane_table = rk3588_plane_table,
  */
  .PlaneMask = mVpPlaneMaskRK3588[0],
  .NrLayers  = 8,
  .NrMixers  = 7,
  .NrGammas  = 4,
  .NrDscs    = 2,
};

STATIC UINT8  RK3588Vop2VpPrimaryPlaneOrder[VOP2_VP_MAX] = {
  ROCKCHIP_VOP2_ESMART0,
  ROCKCHIP_VOP2_ESMART1,
  ROCKCHIP_VOP2_ESMART2,
  ROCKCHIP_VOP2_ESMART3,
};

STATIC UINT8  RK3568Vop2VpPrimaryPlaneOrder[VOP2_VP_MAX] = {
  ROCKCHIP_VOP2_SMART0,
  ROCKCHIP_VOP2_SMART1,
  ROCKCHIP_VOP2_ESMART1,
};

STATIC UINT32  mRegsBackup[RK3568_MAX_REG] = { 0 };

STATIC VOP2  *RockchipVop2;

INLINE
UINT32
LogCalculate (
  IN UINT32  Input
  )
{
  INT32  Count = -1;

  if (Input == 0) {
    return 0;
  }

  while (Input) {
    Input >>= 1;
    Count++;
  }

  return Count;
}

INLINE
VOID
Vop2MaskWrite (
  IN  UINTN    Address,
  IN  UINT32   Offset,
  IN  UINT32   Mask,
  IN  UINT32   Shift,
  IN  UINT32   Value,
  IN  BOOLEAN  WriteMask
  )
{
  UINT32  CachedVal;

  if (!Mask) {
    return;
  }

  if (WriteMask) {
    Value = ((Value & Mask) << Shift) | (Mask << (Shift + 16));
  } else {
    CachedVal = mRegsBackup[Offset >> 2];

    Value                    = (CachedVal & ~(Mask << Shift)) | ((Value & Mask) << Shift);
    mRegsBackup[Offset >> 2] = Value;
  }

  MmioWrite32 (Address + Offset, Value);
}

INLINE
VOID
Vop2Writel (
  IN UINTN   Address,
  IN UINT32  Offset,
  IN UINT32  Value
  )
{
  MmioWrite32 (Address + Offset, Value);
  mRegsBackup[Offset >> 2] = Value;
}

INLINE
VOID
Vop2GrfWrite (
  IN  UINTN   Address,
  IN  UINT32  Offset,
  IN  UINT32  Mask,
  IN  UINT32  Shift,
  IN  UINT32  Value
  )
{
  UINT32  TempVal = 0;

  TempVal = (Value << Shift) | (Mask << (Shift + 16));
  MmioWrite32 (Address + Offset, TempVal);
}

INLINE
UINT32
Vop2GrfRead (
  IN  UINTN   Address,
  IN  UINT32  Offset,
  IN  UINT32  Mask,
  IN  UINT32  Shift
  )
{
  return (MmioRead32 (Address + Offset) >> Shift) & Mask;
}

INLINE
UINT32
GenericHWeight32 (
  IN  UINT32  W
  )
{
  UINT32  Res = (W & 0x55555555) + ((W >> 1) & 0x55555555);

  Res = (Res & 0x33333333) + ((Res >> 2) & 0x33333333);
  Res = (Res & 0x0F0F0F0F) + ((Res >> 4) & 0x0F0F0F0F);
  Res = (Res & 0x00FF00FF) + ((Res >> 8) & 0x00FF00FF);
  return (Res & 0x0000FFFF) + ((Res >> 16) & 0x0000FFFF);
}

INLINE
INT32
FFS (
  int  x
  )
{
  int  r = 1;

  if (!x) {
    return 0;
  }

  if (!(x & 0xffff)) {
    x >>= 16;
    r  += 16;
  }

  if (!(x & 0xff)) {
    x >>= 8;
    r  += 8;
  }

  if (!(x & 0xf)) {
    x >>= 4;
    r  += 4;
  }

  if (!(x & 3)) {
    x >>= 2;
    r  += 2;
  }

  if (!(x & 1)) {
    r += 1;
  }

  return r;
}

STATIC
INT32
Vop2GetPrimaryPlane (
  OUT VOP2    *Vop2,
  IN  UINT32  PlaneMask
  )
{
  UINT32  i = 0;
  UINT8   *Vop2VpPrimaryPlaneOrder;
  UINT8   DefaultPrimaryPlane;

  if (Vop2->Version == VOP_VERSION_RK3588) {
    Vop2VpPrimaryPlaneOrder = RK3588Vop2VpPrimaryPlaneOrder;
    DefaultPrimaryPlane     = ROCKCHIP_VOP2_ESMART0;
  } else {
    Vop2VpPrimaryPlaneOrder = RK3568Vop2VpPrimaryPlaneOrder;
    DefaultPrimaryPlane     = ROCKCHIP_VOP2_SMART0;
  }

  for (i = 0; i < Vop2->Data->NrVps; i++) {
    if (PlaneMask & BIT (Vop2VpPrimaryPlaneOrder[i])) {
      return Vop2VpPrimaryPlaneOrder[i];
    }
  }

  return DefaultPrimaryPlane;
}

STATIC
CHAR8 *
GetPlaneName (
  IN UINT32  PlaneID
  )
{
  CHAR8  *Name = NULL;

  switch (PlaneID) {
    case ROCKCHIP_VOP2_CLUSTER0:
      Name = "Cluster0";
      break;
    case ROCKCHIP_VOP2_CLUSTER1:
      Name = "Cluster1";
      break;
    case ROCKCHIP_VOP2_ESMART0:
      Name = "Esmart0";
      break;
    case ROCKCHIP_VOP2_ESMART1:
      Name = "Esmart1";
      break;
    case ROCKCHIP_VOP2_SMART0:
      Name = "Smart0";
      break;
    case ROCKCHIP_VOP2_SMART1:
      Name = "Smart1";
      break;
    case ROCKCHIP_VOP2_CLUSTER2:
      Name = "Cluster2";
      break;
    case ROCKCHIP_VOP2_CLUSTER3:
      Name = "Cluster3";
      break;
    case ROCKCHIP_VOP2_ESMART2:
      Name = "Esmart2";
      break;
    case ROCKCHIP_VOP2_ESMART3:
      Name = "Esmart3";
      break;
  }

  return Name;
}

VOID
Vop2DumpRegisters (
  OUT DISPLAY_STATE  *DisplayState,
  OUT VOP2_WIN_DATA  *WinData
  )
{
  CRTC_STATE  *CrtcState     = &DisplayState->CrtcState;
  VOP2        *Vop2          = CrtcState->Private;
  UINT32      VPOffset       = CrtcState->CrtcID * 0x100;
  UINT32      WinOffset      = 0;
  UINT8       PrimaryPlaneID = 0;
  INT32       i              = 0;
  UINT32      Reg            = 0;

  /* sys registers */
  Reg = RK3588_VOP2_REG_BASE;
  DEBUG ((DEBUG_WARN, "SYS:"));
  for (i = 0; i < 0x100; i += 4) {
    if (i % 0x10 == 0) {
      DEBUG ((DEBUG_WARN, "\n"));
      DEBUG ((DEBUG_WARN, "%x:", Reg + i));
    }

    DEBUG ((DEBUG_WARN, " %08x", MmioRead32 (Reg + i)));
  }

  DEBUG ((DEBUG_WARN, "\n"));
  DEBUG ((DEBUG_WARN, "\n"));

  /* ovl registers */
  Reg = RK3588_VOP2_REG_BASE + RK3568_OVL_CTRL;
  DEBUG ((DEBUG_WARN, "OVL:"));
  for (i = 0; i < 0x100; i += 4) {
    if (i % 0x10 == 0) {
      DEBUG ((DEBUG_WARN, "\n"));
      DEBUG ((DEBUG_WARN, "%x:", Reg + i));
    }

    DEBUG ((DEBUG_WARN, " %08x", MmioRead32 (Reg + i)));
  }

  DEBUG ((DEBUG_WARN, "\n"));
  DEBUG ((DEBUG_WARN, "\n"));

  /* hdr registers */
  Reg = RK3588_VOP2_REG_BASE + 0x2000;
  DEBUG ((DEBUG_WARN, "HDR:"));
  for (i = 0; i < 0x40; i += 4) {
    if (i % 0x10 == 0) {
      DEBUG ((DEBUG_WARN, "\n"));
      DEBUG ((DEBUG_WARN, "%x:", Reg + i));
    }

    DEBUG ((DEBUG_WARN, " %08x", MmioRead32 (Reg + i)));
  }

  DEBUG ((DEBUG_WARN, "\n"));
  DEBUG ((DEBUG_WARN, "\n"));

  /* vp registers */
  Reg = RK3588_VOP2_REG_BASE + RK3568_VP0_DSP_CTRL + VPOffset;
  DEBUG ((DEBUG_WARN, "VP%d:", CrtcState->CrtcID));
  for (i = 0; i < 0x100; i += 4) {
    if (i % 0x10 == 0) {
      DEBUG ((DEBUG_WARN, "\n"));
      DEBUG ((DEBUG_WARN, "%x:", Reg + i));
    }

    DEBUG ((DEBUG_WARN, " %08x", MmioRead32 (Reg + i)));
  }

  DEBUG ((DEBUG_WARN, "\n"));
  DEBUG ((DEBUG_WARN, "\n"));

  /* plane registers */
  if (WinData) {
    WinOffset = WinData->RegOffset;
    if (WinData->Type == CLUSTER_LAYER) {
      Reg = RK3588_VOP2_REG_BASE + RK3568_CLUSTER0_WIN0_CTRL0 + WinOffset;
    } else {
      Reg = RK3588_VOP2_REG_BASE + RK3568_ESMART0_CTRL0 + WinOffset;
    }

    PrimaryPlaneID = Vop2->VpPlaneMask[CrtcState->CrtcID].PrimaryPlaneId;

    DEBUG ((DEBUG_WARN, "%a:", GetPlaneName (PrimaryPlaneID)));
    for (i = 0; i < 0x100; i += 4) {
      if (i % 0x10 == 0) {
        DEBUG ((DEBUG_WARN, "\n"));
        DEBUG ((DEBUG_WARN, "%x:", Reg + i));
      }

      DEBUG ((DEBUG_WARN, " %08x", MmioRead32 (Reg + i)));
    }

    DEBUG ((DEBUG_WARN, "\n"));
    DEBUG ((DEBUG_WARN, "\n"));
  }
}

STATIC
VOP2_WIN_DATA *
Vop2FindWinByPhysID (
  OUT VOP2   *Vop2,
  IN  INT32  PhysID
  )
{
  INT32  i = 0;

  for (i = 0; i < Vop2->Data->NrLayers; i++) {
    if (Vop2->Data->WinData[i].PhysID == PhysID) {
      return &Vop2->Data->WinData[i];
    }
  }

  return NULL;
}

INLINE
BOOLEAN
IsHotPlugDevices (
  IN  INT32  OutputType
  )
{
  switch (OutputType) {
    case DRM_MODE_CONNECTOR_HDMIA:
    case DRM_MODE_CONNECTOR_HDMIB:
    case DRM_MODE_CONNECTOR_TV:
    case DRM_MODE_CONNECTOR_DisplayPort:
    case DRM_MODE_CONNECTOR_VGA:
    case DRM_MODE_CONNECTOR_Unknown:
      return TRUE;
    default:
      return FALSE;
  }
}

STATIC
VOID
Vop2GlobalInitial (
  OUT DISPLAY_STATE  *DisplayState
  )
{
  CRTC_STATE              *CrtcState = &DisplayState->CrtcState;
  VOP2                    *Vop2      = CrtcState->Private;
  ROCKCHIP_CRTC_PROTOCOL  *Crtc      = (ROCKCHIP_CRTC_PROTOCOL *)CrtcState->Crtc;
  VOP2_WIN_DATA           *WinData;
  VOP2_VP_PLANE_MASK      *PlaneMask;
  UINT32                  BakIndex;
  UINT32                  i = 0, j = 0;
  UINT32                  LayerNr = 0;
  UINT8                   Shift = 0, TotalUsedLayer = 0;
  INT32                   PortMux    = 0;
  INT32                   LayerPhyID = 0;

  if (Vop2->GlobalInit) {
    return;
  }

  /* open the vop global pd */
  MmioWrite32 (0xfd8d8150, 0xffff0000);
  MicroSecondDelay (10);

  if ((DisplayState->VpsConfigModeID < 0) || (DisplayState->VpsConfigModeID >= mVpsConfigsSize)) {
    INT32  MainVpIndex = -1;
    INT32  ActiveVpNum = 0;

    for (i = 0; i < Vop2->Data->NrVps; i++) {
      if (Crtc->Vps[i].Enable) {
        ActiveVpNum++;
      }
    }

    PlaneMask  = Vop2->Data->PlaneMask;
    PlaneMask += (ActiveVpNum - 1) * VOP2_VP_MAX;

    /* find the first unplug devices and set it as main display */
    for (i = 0; i < Vop2->Data->NrVps; i++) {
      if (!IsHotPlugDevices (Crtc->Vps[i].OutputType)) {
        Vop2->VpPlaneMask[i] = PlaneMask[0];
        MainVpIndex          = i;
        break;
      }
    }

    /* if no find unplug devices, use vp0 as main display */
    if (MainVpIndex < 0) {
      ActiveVpNum          = 0;
      Vop2->VpPlaneMask[0] = PlaneMask[0];
    }

    /* plane_mask[0] store main display, so we from plane_mask[1] */
    j = 1;

    /* init other display except main display */
    for (i = 0; i < Vop2->Data->NrVps; i++) {
      if ((i == MainVpIndex) || !Crtc->Vps[i].Enable) {
        continue;
      }

      Vop2->VpPlaneMask[i] = PlaneMask[j++];
    }

    for (i = 0; i < Vop2->Data->NrVps; i++) {
      LayerNr = Vop2->VpPlaneMask[i].AttachedLayersNr;
      for (j = 0; j < LayerNr; j++) {
        LayerPhyID                      = Vop2->VpPlaneMask[i].AttachedLayers[j];
        Vop2->VpPlaneMask[i].PlaneMask |= BIT (LayerPhyID);
      }
    }
  } else {
    UINT32  PlaneMask;
    UINT32  PrimaryPlaneID;

    for (i = 0; i < Vop2->Data->NrVps; i++) {
      PlaneMask                             = mVpsConfigs[DisplayState->VpsConfigModeID][i].PlaneMask;
      Vop2->VpPlaneMask[i].PlaneMask        = PlaneMask;
      LayerNr                               = GenericHWeight32 (PlaneMask);
      Vop2->VpPlaneMask[i].AttachedLayersNr = LayerNr;
      PrimaryPlaneID                        = Vop2GetPrimaryPlane (Vop2, PlaneMask);
      Vop2->VpPlaneMask[i].PrimaryPlaneId   = PrimaryPlaneID;
      Vop2->VpPlaneMask[i].PlaneMask        = PlaneMask;
      for (j = 0; j < LayerNr; j++) {
        Vop2->VpPlaneMask[i].AttachedLayers[j] = FFS (PlaneMask) - 1;
        PlaneMask                             &= ~BIT(Vop2->VpPlaneMask[i].AttachedLayers[j]);
      }
    }
  }

  /* open the vop plane pd(esmart) */
  /* status checkout --- todo */
  MmioWrite32 (0xfdd90034, 0x00000000);
  MicroSecondDelay (10);

  /* vop2 regs backup */
  for (BakIndex = 0; BakIndex < (RK3568_MAX_REG >> 2); BakIndex++) {
    mRegsBackup[BakIndex] = MmioRead32 (Vop2->BaseAddress + 4 * BakIndex);
  }

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_OVL_CTRL,
    EN_MASK,
    OVL_PORT_MUX_REG_DONE_IMD_SHIFT,
    1,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_DSP_IF_POL,
    EN_MASK,
    IF_CTRL_REG_DONE_IMD_SHIFT,
    1,
    FALSE
    );

  for (i = 0; i < Vop2->Data->NrVps; i++) {
    DEBUG ((DEBUG_INIT, "vp%d have layer nr:%d[", i, Vop2->VpPlaneMask[i].AttachedLayersNr));
    for (j = 0; j < Vop2->VpPlaneMask[i].AttachedLayersNr; j++) {
      DEBUG ((DEBUG_INIT, "%d ", Vop2->VpPlaneMask[i].AttachedLayers[j]));
    }

    DEBUG ((DEBUG_INIT, "], primary plane: %d\n", Vop2->VpPlaneMask[i].PrimaryPlaneId));
  }

  Shift = 0;
  /* layer sel win id */
  for (i = 0; i < Vop2->Data->NrVps; i++) {
    LayerNr = Vop2->VpPlaneMask[i].AttachedLayersNr;
    for (j = 0; j < LayerNr; j++) {
      LayerPhyID = Vop2->VpPlaneMask[i].AttachedLayers[j];
      WinData    = Vop2FindWinByPhysID (Vop2, LayerPhyID);
      Vop2MaskWrite (
        Vop2->BaseAddress,
        RK3568_OVL_LAYER_SEL,
        LAYER_SEL_MASK,
        Shift,
        WinData->LayerSelWinID,
        FALSE
        );
      Shift += 4;
    }
  }

  /* win sel port */
  for (i = 0; i < Vop2->Data->NrVps; i++) {
    LayerNr = Vop2->VpPlaneMask[i].AttachedLayersNr;
    for (j = 0; j < LayerNr; j++) {
      if (!Vop2->VpPlaneMask[i].AttachedLayers[j]) {
        continue;
      }

      LayerPhyID = Vop2->VpPlaneMask[i].AttachedLayers[j];
      WinData    = Vop2FindWinByPhysID (Vop2, LayerPhyID);
      Shift      = WinData->WinSelPortOffset * 2;
      Vop2MaskWrite (
        Vop2->BaseAddress,
        RK3568_OVL_PORT_SEL,
        LAYER_SEL_PORT_MASK,
        LAYER_SEL_PORT_SHIFT + Shift,
        i,
        FALSE
        );
    }
  }

  /*
   * port mux config
   */
  for (i = 0; i < Vop2->Data->NrVps; i++) {
    Shift = i * 4;
    if (Vop2->VpPlaneMask[i].AttachedLayersNr) {
      TotalUsedLayer += Vop2->VpPlaneMask[i].AttachedLayersNr;
      PortMux         = TotalUsedLayer - 1;
    } else {
      PortMux = 8;
    }

    if (i == (Vop2->Data->NrVps - 1)) {
      PortMux = Vop2->Data->NrMixers;
    }

    Crtc->Vps[i].BgOvlDly = (Vop2->Data->NrMixers - PortMux) << 1;
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_OVL_PORT_SEL,
      PORT_MUX_MASK,
      PORT_MUX_SHIFT + Shift,
      PortMux,
      FALSE
      );
  }

  Vop2->GlobalInit = TRUE;
}

STATIC
UINT64
Vop2CalcDclk (
  IN UINT32  ChildClk,
  IN UINT32  MaxDclk
  )
{
  if (ChildClk * 4 <= MaxDclk) {
    return ChildClk * 4;
  } else if (ChildClk * 2 <= MaxDclk) {
    return ChildClk * 2;
  } else if (ChildClk <= MaxDclk) {
    return ChildClk;
  } else {
    return 0;
  }
}

STATIC
UINT64
Vop2CalcCruConfig (
  IN  DISPLAY_STATE  *DisplayState,
  OUT UINT32         *DclkCoreDiv,
  OUT UINT32         *DclkOutDiv,
  OUT UINT32         *IfPixclkDiv,
  OUT UINT32         *IfDclkDiv
  )
{
  CONNECTOR_STATE   *ConnectorState = &DisplayState->ConnectorState;
  DRM_DISPLAY_MODE  *DisplayMode    = &ConnectorState->DisplayMode;
  CRTC_STATE        *CrtcState      = &DisplayState->CrtcState;
  VOP2              *Vop2           = CrtcState->Private;
  UINT64            VPixclk         = DisplayMode->Clock;
  UINT64            DclkCoreRate    = VPixclk >> 2;
  UINT64            DclkRate        = VPixclk;
  UINT64            IfDclkRate;
  UINT64            IfPixclkRate;
  UINT64            DclkOutRate;
  INT32             OutputType = ConnectorState->Type;
  INT32             OutputMode = ConnectorState->OutputMode;
  UINT8             K          = 1;

  if (ConnectorState->OutputFlags & ROCKCHIP_OUTPUT_DUAL_CHANNEL_LEFT_RIGHT_MODE &&
      (OutputMode == ROCKCHIP_OUT_MODE_YUV420))
  {
    DEBUG ((DEBUG_ERROR, "Dual channel and YUV420 can't work together\n"));
    return -RETURN_INVALID_PARAMETER;
  }

  if (ConnectorState->OutputFlags & ROCKCHIP_OUTPUT_DUAL_CHANNEL_LEFT_RIGHT_MODE ||
      (OutputMode == ROCKCHIP_OUT_MODE_YUV420))
  {
    K = 2;
  }

  if (OutputType == DRM_MODE_CONNECTOR_HDMIA) {
    /*
     * K = 2: dclk_core = if_pixclk_rate > if_dclk_rate
     * K = 1: dclk_core = hdmie_edp_dclk > if_pixclk_rate
     */
    if (ConnectorState->OutputFlags & ROCKCHIP_OUTPUT_DUAL_CHANNEL_LEFT_RIGHT_MODE ||
        (OutputMode == ROCKCHIP_OUT_MODE_YUV420))
    {
      DclkRate = DclkRate >> 1;
      K        = 2;
    }

    IfPixclkRate = (DclkCoreRate << 1) / K;
    IfDclkRate   = DclkCoreRate / K;

    if (VPixclk > VOP2_MAX_DCLK_RATE) {
      DclkRate = Vop2CalcDclk (DclkCoreRate, Vop2->Data->VpData->MaxDclk);
    }

    if (!DclkRate) {
      DEBUG ((
        DEBUG_ERROR,
        "DP IfPixclkRate out of range(MaxDclk: %d KHZ, DclkCore: %lld KHZ)\n",
        Vop2->Data->VpData->MaxDclk,
        IfPixclkRate
        ));
      return -RETURN_INVALID_PARAMETER;
    }

    *IfPixclkDiv = DclkRate / IfPixclkRate;
    *IfDclkDiv   = DclkRate / IfDclkRate;
    *DclkCoreDiv = DclkRate / DclkCoreRate;
    DEBUG ((
      DEBUG_INFO,
      "DclkRate:%lu,IfPixclkDiv;%d,IfDclkDiv:%d\n",
      DclkRate,
      *IfPixclkDiv,
      *IfDclkDiv
      ));
  } else if (OutputType == DRM_MODE_CONNECTOR_eDP) {
    /* edp_pixclk = edp_dclk > dclk_core */
    IfPixclkRate = VPixclk / K;
    IfDclkRate   = VPixclk / K;
    DclkRate     = IfPixclkRate * K;
    *DclkCoreDiv = DclkRate / DclkCoreRate;
    *IfPixclkDiv = DclkRate / IfPixclkRate;
    *IfDclkDiv   = *IfPixclkDiv;
  } else if (OutputType == DRM_MODE_CONNECTOR_DisplayPort) {
    DclkOutRate = VPixclk >> 2;
    DclkOutRate = DclkOutRate / K;

    DclkRate = Vop2CalcDclk (
                 DclkOutRate,
                 Vop2->Data->VpData->MaxDclk
                 );
    if (!DclkRate) {
      DEBUG ((
        DEBUG_ERROR,
        "DP dclk_core out of range(max_dclk: %d KHZ, dclk_core: %ld KHZ)\n",
        Vop2->Data->VpData->MaxDclk,
        DclkCoreRate
        ));
      return EFI_INVALID_PARAMETER;
    }

    *DclkOutDiv  = DclkRate / DclkOutRate;
    *DclkCoreDiv = DclkRate / DclkCoreRate;
  } else if (OutputType == DRM_MODE_CONNECTOR_DSI) {
    if (ConnectorState->OutputFlags & ROCKCHIP_OUTPUT_DUAL_CHANNEL_LEFT_RIGHT_MODE) {
      K = 2;
    }

    if (CrtcState->dsc_enable) {
      /* dsc output is 96bit, dsi input is 192 bit */
      IfPixclkRate = CrtcState->dsc_cds_clk_rate >> 1;
    } else {
      IfPixclkRate = DclkCoreRate / K;
    }

    /* dclk_core = dclk_out * K = if_pixclk * K = VPixclk / 4 */
    DclkOutRate = DclkCoreRate / K;
    /* DclkRate = N * DclkCoreRate N = (1,2,4 ), we get a little factor here */
    DclkRate = Vop2CalcDclk (
                 DclkOutRate,
                 Vop2->Data->VpData->MaxDclk
                 );
    if (!DclkRate) {
      DEBUG ((
        DEBUG_INFO,
        "MIPI dclk out of range(max_dclk: %d KHZ, DclkRate: %ld KHZ)\n",
        Vop2->Data->VpData->MaxDclk,
        DclkRate
        ));
      return -EINVAL;
    }

    if (CrtcState->dsc_enable) {
      DclkRate /= CrtcState->dsc_slice_num;
    }

    *DclkOutDiv  = DclkRate / DclkOutRate;
    *DclkCoreDiv = DclkRate / DclkCoreRate;
    *IfPixclkDiv = 1;       /*mipi pixclk == dclk_out*/
    if (CrtcState->dsc_enable) {
      *IfPixclkDiv = DclkOutRate * 1000LL / IfPixclkRate;
    }
  }

  *IfPixclkDiv = LogCalculate (*IfPixclkDiv);
  *IfDclkDiv   = LogCalculate (*IfDclkDiv);
  *DclkCoreDiv = LogCalculate (*DclkCoreDiv);
  *DclkOutDiv  = LogCalculate (*DclkOutDiv);

  DEBUG ((DEBUG_INFO, "[VOP2](CalcCru)DclkRate     = %ld(divide: %d)\n", DclkRate, *DclkOutDiv));
  DEBUG ((DEBUG_INFO, "               DclkCoreRate = %ld(divide: %d)\n", DclkCoreRate, *DclkCoreDiv));
  DEBUG ((DEBUG_INFO, "               IfDclkRate   = %ld(divide: %d)\n", IfDclkRate, *IfDclkDiv));
  DEBUG ((DEBUG_INFO, "               IfPixclkRate = %ld(divide: %d)\n", IfPixclkRate, *IfPixclkDiv));

  return DclkRate;
}

STATIC
EFI_STATUS
Vop2CalcDscClk (
  OUT DISPLAY_STATE  *DisplayState
  )
{
  CRTC_STATE        *CrtcState      = &DisplayState->CrtcState;
  CONNECTOR_STATE   *ConnectorState = &DisplayState->ConnectorState;
  DRM_DISPLAY_MODE  *DisplayMode    = &ConnectorState->DisplayMode;
  UINT64            VPixclk         = DisplayMode->CrtcClock * 1000LL; /* video timing pixclk */
  UINT8             K               = 1;

  if (ConnectorState->OutputFlags & ROCKCHIP_OUTPUT_DUAL_CHANNEL_LEFT_RIGHT_MODE) {
    K = 2;
  }

  CrtcState->dsc_txp_clk_rate = VPixclk;
  do_div (CrtcState->dsc_txp_clk_rate, (CrtcState->dsc_pixel_num * K));

  CrtcState->dsc_pxl_clk_rate = VPixclk;
  do_div (CrtcState->dsc_pxl_clk_rate, (CrtcState->dsc_slice_num * K));

  /* dsc_cds = crtc_clock / (cds_dat_width / bits_per_pixel)
    * cds_dat_width = 96;
    * bits_per_pixel = [8-12];
    * As cds clk is div from txp clk and only support 1/2/4 div,
    * so when txp_clk is equal to VPixclk, we set dsc_cds = crtc_clock / 4,
    * otherwise dsc_cds = crtc_clock / 8;
    */
  CrtcState->dsc_cds_clk_rate = VPixclk / (CrtcState->dsc_txp_clk_rate == VPixclk ? 4 : 8);

  return 0;
}

STATIC
UINT32
Vop2IfConfig (
  OUT DISPLAY_STATE  *DisplayState,
  OUT VOP2           *Vop2
  )
{
  CRTC_STATE                    *CrtcState      = &DisplayState->CrtcState;
  CONNECTOR_STATE               *ConnectorState = &DisplayState->ConnectorState;
  DRM_DISPLAY_MODE              *DisplayMode    = &ConnectorState->DisplayMode;
  struct rockchip_dsc_sink_cap  *dsc_sink_cap   = &CrtcState->dsc_sink_cap;
  UINT32                        VPOffset        = CrtcState->CrtcID * 0x100;
  UINT32                        OutputIf        = ConnectorState->OutputInterface;
  UINT32                        DclkCoreDiv     = 0;
  UINT32                        DclkOutDiv      = 0;
  UINT32                        IfPixclkDiv     = 0;
  UINT32                        IfDclkDiv       = 0;
  UINT32                        DclkRate;
  UINT32                        Val;

  if (OutputIf & (VOP_OUTPUT_IF_HDMI0 | VOP_OUTPUT_IF_HDMI1)) {
    Val  = (DisplayMode->Flags & DRM_MODE_FLAG_NHSYNC) ? BIT (HSYNC_POSITIVE) : 0;
    Val |= (DisplayMode->Flags & DRM_MODE_FLAG_NVSYNC) ? BIT (VSYNC_POSITIVE) : 0;
  } else {
    Val  = (DisplayMode->Flags & DRM_MODE_FLAG_NHSYNC) ? 0 : BIT (HSYNC_POSITIVE);
    Val |= (DisplayMode->Flags & DRM_MODE_FLAG_NVSYNC) ? 0 : BIT (VSYNC_POSITIVE);
  }

  if (CrtcState->dsc_enable) {
    UINT32  K = 1;

    if (!Vop2->Data->NrDscs) {
      DEBUG ((DEBUG_ERROR, "Unsupported DSC\n"));
      return 0;
    }

    if (ConnectorState->OutputFlags & ROCKCHIP_OUTPUT_DUAL_CHANNEL_LEFT_RIGHT_MODE) {
      K = 2;
    }

    CrtcState->dsc_id        = OutputIf & (VOP_OUTPUT_IF_MIPI0 | VOP_OUTPUT_IF_HDMI0) ? 0 : 1;
    CrtcState->dsc_slice_num = DisplayMode->CrtcHDisplay / dsc_sink_cap->slice_width / K;
    CrtcState->dsc_pixel_num = CrtcState->dsc_slice_num > 4 ? 4 : CrtcState->dsc_slice_num;

    Vop2CalcDscClk (DisplayState);
    DEBUG ((
      DEBUG_INFO,
      "Enable DSC%d slice:%dx%d, slice num:%d\n",
      CrtcState->dsc_id,
      dsc_sink_cap->slice_width,
      dsc_sink_cap->slice_height,
      CrtcState->dsc_slice_num
      ));
  }

  DclkRate               = Vop2CalcCruConfig (DisplayState, &DclkCoreDiv, &DclkOutDiv, &IfPixclkDiv, &IfDclkDiv);
  CrtcState->DclkCoreDiv = DclkCoreDiv;
  CrtcState->DclkOutDiv  = DclkOutDiv;

  if (OutputIf & VOP_OUTPUT_IF_MIPI0) {
    if (CrtcState->CrtcID == 2) {
      Val = 0;
    } else {
      Val = 1;
    }

    if (ConnectorState->OutputFlags & ROCKCHIP_OUTPUT_MIPI_DS_MODE) {
      Vop2MaskWrite (
        Vop2->BaseAddress,
        RK3568_DSP_IF_CTRL,
        EN_MASK,
        RK3588_MIPI_DSI0_MODE_SEL_SHIFT,
        1,
        FALSE
        );
    }

    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      EN_MASK,
      RK3588_MIPI0_EN_SHIFT,
      1,
      FALSE
      );
    Vop2MaskWrite (Vop2->BaseAddress, RK3568_DSP_IF_EN, 1, RK3588_MIPI0_MUX_SHIFT, Val, FALSE);
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_CTRL,
      3,
      MIPI0_PIXCLK_DIV_SHIFT,
      IfPixclkDiv,
      FALSE
      );

    if (ConnectorState->hold_mode) {
      Vop2MaskWrite (
        Vop2->BaseAddress,
        RK3568_VP0_MIPI_CTRL + VPOffset,
        EN_MASK,
        EDPI_TE_EN,
        TRUE,
        FALSE
        );
      Vop2MaskWrite (
        Vop2->BaseAddress,
        RK3568_VP0_MIPI_CTRL + VPOffset,
        EN_MASK,
        EDPI_WMS_HOLD_EN,
        1,
        FALSE
        );
    }
  }

  if (OutputIf & VOP_OUTPUT_IF_MIPI1) {
    if (CrtcState->CrtcID == 2) {
      Val = 0;
    } else if (CrtcState->CrtcID == 3) {
      Val = 1;
    } else {
      Val = 3; /*VP1*/
    }

    if (ConnectorState->OutputFlags & ROCKCHIP_OUTPUT_MIPI_DS_MODE) {
      Vop2MaskWrite (
        Vop2->BaseAddress,
        RK3568_DSP_IF_CTRL,
        EN_MASK,
        RK3588_MIPI_DSI1_MODE_SEL_SHIFT,
        1,
        FALSE
        );
    }

    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      EN_MASK,
      RK3588_MIPI1_EN_SHIFT,
      1,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      IF_MUX_MASK,
      MIPI1_MUX_SHIFT,
      Val,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_CTRL,
      3,
      MIPI1_PIXCLK_DIV_SHIFT,
      IfPixclkDiv,
      FALSE
      );

    if (ConnectorState->hold_mode) {
      Vop2MaskWrite (
        Vop2->BaseAddress,
        RK3568_VP0_MIPI_CTRL + VPOffset,
        EN_MASK,
        EDPI_TE_EN,
        TRUE,
        FALSE
        );
      Vop2MaskWrite (
        Vop2->BaseAddress,
        RK3568_VP0_MIPI_CTRL + VPOffset,
        EN_MASK,
        EDPI_WMS_HOLD_EN,
        1,
        FALSE
        );
    }
  }

  if (OutputIf & VOP_OUTPUT_IF_EDP0) {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      EN_MASK,
      RK3588_EDP0_EN_SHIFT,
      1,
      FALSE
      );
    /* temp eDP0 fixed vp2 */
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      IF_MUX_MASK,
      RK3588_HDMI_EDP0_MUX_SHIFT,
      CrtcState->CrtcID,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_CTRL,
      0x3,
      HDMI_EDP0_DCLK_DIV_SHIFT,
      IfDclkDiv,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_CTRL,
      0x3,
      HDMI_EDP0_PIXCLK_DIV_SHIFT,
      IfPixclkDiv,
      FALSE
      );
    Vop2GrfWrite (
      RK3588_VOP_GRF_BASE,
      RK3588_GRF_VOP_CON2,
      EN_MASK,
      RK3588_GRF_EDP0_ENABLE_SHIFT,
      1
      );
  }

  if (OutputIf & VOP_OUTPUT_IF_EDP1) {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      EN_MASK,
      RK3588_EDP1_EN_SHIFT,
      1,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      IF_MUX_MASK,
      RK3588_HDMI_EDP1_MUX_SHIFT,
      CrtcState->CrtcID,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_CTRL,
      0x3,
      HDMI_EDP1_DCLK_DIV_SHIFT,
      IfDclkDiv,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_CTRL,
      0x3,
      HDMI_EDP1_PIXCLK_DIV_SHIFT,
      IfPixclkDiv,
      FALSE
      );
    Vop2GrfWrite (
      RK3588_VOP_GRF_BASE,
      RK3588_GRF_VOP_CON2,
      EN_MASK,
      RK3588_GRF_EDP1_ENABLE_SHIFT,
      1
      );
  }

  if (OutputIf & VOP_OUTPUT_IF_HDMI0) {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      EN_MASK,
      RK3588_HDMI0_EN_SHIFT,
      1,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      IF_MUX_MASK,
      RK3588_HDMI_EDP0_MUX_SHIFT,
      CrtcState->CrtcID,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_CTRL,
      0x3,
      HDMI_EDP0_DCLK_DIV_SHIFT,
      IfDclkDiv,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_CTRL,
      0x3,
      HDMI_EDP0_PIXCLK_DIV_SHIFT,
      IfPixclkDiv,
      FALSE
      );
    Vop2GrfWrite (
      RK3588_VOP_GRF_BASE,
      RK3588_GRF_VOP_CON2,
      EN_MASK,
      RK3588_GRF_HDMITX0_ENABLE_SHIFT,
      1
      );
    Vop2GrfWrite (
      RK3588_VO1_GRF_BASE,
      RK3588_GRF_VO1_CON0,
      HDMI_SYNC_POL_MASK,
      HDMI0_SYNC_POL_SHIFT,
      Val
      );
  }

  if (OutputIf & VOP_OUTPUT_IF_HDMI1) {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      EN_MASK,
      RK3588_HDMI1_EN_SHIFT,
      1,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      IF_MUX_MASK,
      RK3588_HDMI_EDP1_MUX_SHIFT,
      CrtcState->CrtcID,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_CTRL,
      0x3,
      HDMI_EDP1_DCLK_DIV_SHIFT,
      IfDclkDiv,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_CTRL,
      0x3,
      HDMI_EDP1_PIXCLK_DIV_SHIFT,
      IfPixclkDiv,
      FALSE
      );
    Vop2GrfWrite (
      RK3588_VOP_GRF_BASE,
      RK3588_GRF_VOP_CON2,
      EN_MASK,
      RK3588_GRF_HDMITX1_ENABLE_SHIFT,
      1
      );
    Vop2GrfWrite (
      RK3588_VO1_GRF_BASE,
      RK3588_GRF_VO1_CON0,
      HDMI_SYNC_POL_MASK,
      HDMI1_SYNC_POL_SHIFT,
      Val
      );
  }

  if (OutputIf & VOP_OUTPUT_IF_DP0) {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      EN_MASK,
      RK3588_DP0_EN_SHIFT,
      1,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      IF_MUX_MASK,
      RK3588_DP0_MUX_SHIFT,
      CrtcState->CrtcID,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_POL,
      RK3588_IF_PIN_POL_MASK,
      RK3588_DP0_PIN_POL_SHIFT,
      Val,
      FALSE
      );
  }

  if (OutputIf & VOP_OUTPUT_IF_DP1) {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      EN_MASK,
      RK3588_DP1_EN_SHIFT,
      1,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_EN,
      IF_MUX_MASK,
      RK3588_DP1_MUX_SHIFT,
      CrtcState->CrtcID,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_DSP_IF_POL,
      RK3588_IF_PIN_POL_MASK,
      RK3588_DP1_PIN_POL_SHIFT,
      Val,
      FALSE
      );
  }

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3588_VP0_CLK_CTRL + VPOffset,
    0x3,
    DCLK_CORE_DIV_SHIFT,
    DclkCoreDiv,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3588_VP0_CLK_CTRL + VPOffset,
    0x3,
    DCLK_OUT_DIV_SHIFT,
    DclkOutDiv,
    FALSE
    );

  return DclkRate;
}

STATIC
BOOLEAN
IsUVSwap (
  IN UINT32  BusFormat,
  IN UINT32  OutputMode
  )
{
  /*
   * FIXME:
   *
   * There is no media type for YUV444 output,
   * so when out_mode is AAAA or P888, assume output is YUV444 on
   * yuv format.
   *
   * From H/W testing, YUV444 mode need a rb swap.
   */
  if (((BusFormat == MEDIA_BUS_FMT_YUV8_1X24) ||
       (BusFormat == MEDIA_BUS_FMT_YUV10_1X30)) &&
      ((OutputMode == ROCKCHIP_OUT_MODE_AAAA) ||
       (OutputMode == ROCKCHIP_OUT_MODE_P888)))
  {
    return TRUE;
  } else {
    return FALSE;
  }
}

STATIC
BOOLEAN
IsYUVOutput (
  IN UINT32  BusFormat
  )
{
  switch (BusFormat) {
    case MEDIA_BUS_FMT_YUV8_1X24:
    case MEDIA_BUS_FMT_YUV10_1X30:
    case MEDIA_BUS_FMT_UYYVYY8_0_5X24:
    case MEDIA_BUS_FMT_UYYVYY10_0_5X30:
    case MEDIA_BUS_FMT_YUYV8_2X8:
    case MEDIA_BUS_FMT_YVYU8_2X8:
    case MEDIA_BUS_FMT_UYVY8_2X8:
    case MEDIA_BUS_FMT_VYUY8_2X8:
    case MEDIA_BUS_FMT_YUYV8_1X16:
    case MEDIA_BUS_FMT_YVYU8_1X16:
    case MEDIA_BUS_FMT_UYVY8_1X16:
    case MEDIA_BUS_FMT_VYUY8_1X16:
      return TRUE;
    default:
      return FALSE;
  }
}

INLINE
UINT32
Vop2ConvertCSCMode (
  IN UINT32  CSCMode
  )
{
  switch (CSCMode) {
    case V4L2_COLORSPACE_SMPTE170M:
    case V4L2_COLORSPACE_470_SYSTEM_M:
    case V4L2_COLORSPACE_470_SYSTEM_BG:
      return CSC_BT601L;
    case V4L2_COLORSPACE_REC709:
    case V4L2_COLORSPACE_SMPTE240M:
    case V4L2_COLORSPACE_DEFAULT:
      return CSC_BT709L;
    case V4L2_COLORSPACE_JPEG:
      return CSC_BT601F;
    case V4L2_COLORSPACE_BT2020:
      return CSC_BT2020;
    default:
      return CSC_BT709L;
  }
}

INLINE
INT32
Interpolate (
  IN INT32  X1,
  IN INT32  Y1,
  IN INT32  X2,
  IN INT32  Y2,
  IN INT32  X
  )
{
  return Y1 + (Y2 - Y1) * (X - X1) / (X2 - X1);
}

INLINE
INT32
__Fixp_Sin32 (
  IN UINT32  Degrees
  )
{
  INT32    Ret;
  BOOLEAN  Negative = FALSE;

  if (Degrees > 180) {
    Negative = TRUE;
    Degrees -= 180;
  }

  if (Degrees > 90) {
    Degrees = 180 - Degrees;
  }

  Ret = mSinTable[Degrees];

  return Negative ? -Ret : Ret;
}

INLINE
INT32
FixpSin32 (
  IN UINT32  Degrees
  )
{
  Degrees = (Degrees % 360 + 360) % 360;

  return __Fixp_Sin32 (Degrees);
}

#define FixpCos32(v)  FixpSin32((v) + 90)
#define FixpSin16(v)  (FixpSin32(v) >> 16)
#define FixpCos16(v)  (FixpCos32(v) >> 16)

STATIC
VOID
Vop2TVConfigUpdate (
  OUT DISPLAY_STATE  *DisplayState,
  OUT VOP2           *Vop2
  )
{
  CONNECTOR_STATE  *ConnectorState = &DisplayState->ConnectorState;
  CRTC_STATE       *CrtcState      = &DisplayState->CrtcState;
  BASE_BCSH_INFO   *BCSHInfo;
  UINT32           Brightness, Contrast, Saturation, Hue, SinHue, CosHue;
  BOOLEAN          BCSHEnable = FALSE;
  BOOLEAN          R2YEnable  = FALSE;
  BOOLEAN          Y2REnable  = FALSE;
  UINT32           VPOffset   = CrtcState->CrtcID * 0x100;
  UINT32           PostCSCMode;

  /* base2_disp_info --- todo */
  if (!ConnectorState->DispInfo) {
    return;
  }

  BCSHInfo = &ConnectorState->DispInfo->BCSHInfo;
  if (!BCSHInfo) {
    return;
  }

  if ((BCSHInfo->Brightness != 50) ||
      (BCSHInfo->Contrast != 50) ||
      (BCSHInfo->Saturation != 50) ||
      (BCSHInfo->Hue != 50))
  {
    BCSHEnable = TRUE;
  }

  if (BCSHEnable) {
    if (!CrtcState->YUVOverlay) {
      R2YEnable = TRUE;
    }

    if (!IsYUVOutput (ConnectorState->BusFormat)) {
      Y2REnable = TRUE;
    }
  } else {
    if (!CrtcState->YUVOverlay && IsYUVOutput (ConnectorState->BusFormat)) {
      R2YEnable = TRUE;
    }

    if (CrtcState->YUVOverlay && !IsYUVOutput (ConnectorState->BusFormat)) {
      Y2REnable = TRUE;
    }
  }

  PostCSCMode = Vop2ConvertCSCMode (ConnectorState->ColorSpace);

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_BCSH_CTRL + VPOffset,
    BCSH_CTRL_R2Y_MASK,
    BCSH_CTRL_R2Y_SHIFT,
    R2YEnable,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_BCSH_CTRL + VPOffset,
    BCSH_CTRL_Y2R_MASK,
    BCSH_CTRL_Y2R_SHIFT,
    Y2REnable,
    FALSE
    );

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_BCSH_CTRL + VPOffset,
    BCSH_CTRL_R2Y_CSC_MODE_MASK,
    BCSH_CTRL_R2Y_CSC_MODE_SHIFT,
    PostCSCMode,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_BCSH_CTRL + VPOffset,
    BCSH_CTRL_Y2R_CSC_MODE_MASK,
    BCSH_CTRL_Y2R_CSC_MODE_SHIFT,
    PostCSCMode,
    FALSE
    );
  if (!BCSHEnable) {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_VP0_BCSH_COLOR + VPOffset,
      BCSH_EN_MASK,
      BCSH_EN_SHIFT,
      0,
      FALSE
      );
    return;
  }

  if (CrtcState->Feature & VOP_FEATURE_OUTPUT_10BIT) {
    Brightness = Interpolate (0, -128, 100, 127, BCSHInfo->Brightness);
  } else {
    Brightness = Interpolate (0, -32, 100, 31, BCSHInfo->Brightness);
  }

  Contrast   = Interpolate (0, 0, 100, 511, BCSHInfo->Contrast);
  Saturation = Interpolate (0, 0, 100, 511, BCSHInfo->Saturation);
  Hue        = Interpolate (0, -30, 100, 30, BCSHInfo->Hue);

  /*
   *  a:[-30~0):
   *    sin_hue = 0x100 - sin(a)*256;
   *    cos_hue = cos(a)*256;
   *  a:[0~30]
   *    sin_hue = sin(a)*256;
   *    cos_hue = cos(a)*256;
   */
  SinHue = FixpSin32 (Hue) >> 23;
  CosHue = FixpCos32 (Hue) >> 23;

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_BCSH_BCS + VPOffset,
    BCSH_BRIGHTNESS_MASK,
    BCSH_BRIGHTNESS_SHIFT,
    Brightness,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_BCSH_BCS + VPOffset,
    BCSH_CONTRAST_MASK,
    BCSH_CONTRAST_SHIFT,
    Contrast,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_BCSH_BCS + VPOffset,
    BCSH_SATURATION_MASK,
    BCSH_SATURATION_SHIFT,
    Saturation,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_BCSH_H + VPOffset,
    BCSH_SIN_HUE_MASK,
    BCSH_SIN_HUE_SHIFT,
    SinHue,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_BCSH_H + VPOffset,
    BCSH_COS_HUE_MASK,
    BCSH_COS_HUE_SHIFT,
    CosHue,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_BCSH_BCS + VPOffset,
    BCSH_OUT_MODE_MASK,
    BCSH_OUT_MODE_SHIFT,
    BCSH_OUT_MODE_NORMAL_VIDEO,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_BCSH_COLOR + VPOffset,
    BCSH_EN_MASK,
    BCSH_EN_SHIFT,
    1,
    FALSE
    );
}

INLINE
UINT16
SclCalScale2 (
  IN  INT32  Src,
  IN  INT32  Dst
  )
{
  return ((Src - 1) << 12) / (Dst - 1);
}

STATIC
VOID
Vop2PostConfig (
  OUT DISPLAY_STATE  *DisplayState,
  OUT VOP2           *Vop2
  )
{
  CONNECTOR_STATE         *ConnectorState = &DisplayState->ConnectorState;
  CRTC_STATE              *CrtcState      = &DisplayState->CrtcState;
  ROCKCHIP_CRTC_PROTOCOL  *Crtc           = (ROCKCHIP_CRTC_PROTOCOL *)CrtcState->Crtc;
  DRM_DISPLAY_MODE        *Mode           = &ConnectorState->DisplayMode;
  UINT32                  VPOffset        = CrtcState->CrtcID * 0x100;
  UINT16                  HDisplay        = Mode->CrtcHDisplay;
  UINT16                  HActStart       = Mode->CrtcHTotal - Mode->CrtcHSyncStart;
  UINT16                  VDisplay        = Mode->CrtcVDisplay;
  UINT16                  VTotal          = Mode->CrtcVTotal;
  UINT16                  VActStart       = Mode->CrtcVTotal - Mode->CrtcVSyncStart;
  UINT16                  HSize           = HDisplay * (ConnectorState->OverScan.LeftMargin +
                                                        ConnectorState->OverScan.RightMargin) /200;
  UINT16  VSize = VDisplay * (ConnectorState->OverScan.TopMargin +
                              ConnectorState->OverScan.BottomMargin) /200;
  UINT16  HActEnd, VActEnd;
  UINT32  Val;

  UINT32  BgOvlDly, BgDly, PreScanDly;
  UINT16  HSyncLen = Mode->CrtcHSyncEnd - Mode->CrtcHSyncStart;

  HSize = ROUNDDOWN (HSize, 2);
  VSize = ROUNDDOWN (VSize, 2);

  HActStart += HDisplay * (100 - ConnectorState->OverScan.LeftMargin) / 200;
  HActEnd    = HActStart + HSize;
  Val        = HActStart << 16;
  Val       |= HActEnd;
  Vop2Writel (Vop2->BaseAddress, RK3568_VP0_POST_DSP_HACT_INFO + VPOffset, Val);

  VActStart += VDisplay * (100 - ConnectorState->OverScan.TopMargin) / 200;
  VActEnd    = VActStart + VSize;
  Val        = VActStart << 16;
  Val       |= VActEnd;
  Vop2Writel (Vop2->BaseAddress, RK3568_VP0_POST_DSP_VACT_INFO + VPOffset, Val);

  Val  = SclCalScale2 (VDisplay, VSize) << 16;
  Val |= SclCalScale2 (VDisplay, VSize);
  Vop2Writel (Vop2->BaseAddress, RK3568_VP0_POST_SCL_FACTOR_YRGB + VPOffset, Val);

  #define POST_HORIZONTAL_SCALEDOWN_EN(x)  ((x) << 0)
  #define POST_VERTICAL_SCALEDOWN_EN(x)    ((x) << 1)
  Vop2Writel (
    Vop2->BaseAddress,
    RK3568_VP0_POST_SCL_CTRL + VPOffset,
    POST_HORIZONTAL_SCALEDOWN_EN (HDisplay != HSize) |
    POST_VERTICAL_SCALEDOWN_EN (VDisplay != VSize)
    );

  if (Mode->Flags & DRM_MODE_FLAG_INTERLACE) {
    UINT16  VActStartF1 = VTotal + VActStart + 1;
    UINT16  VActEndF1   = VActStartF1 + VDisplay;

    Val = VActStartF1 << 16 | VActEndF1;
    Vop2Writel (Vop2->BaseAddress, RK3568_VP0_POST_DSP_VACT_INFO_F1 + VPOffset, Val);
  }

  BgOvlDly   = Crtc->Vps[CrtcState->CrtcID].BgOvlDly;
  BgDly      = Vop2->Data->VpData[CrtcState->CrtcID].PreScanMaxDly;
  BgDly     -= BgOvlDly;
  PreScanDly = BgDly + (HDisplay >> 1) - 1;
  if ((Vop2->Version == VOP_VERSION_RK3588) && (HSyncLen < 8)) {
    HSyncLen = 8;
  }

  PreScanDly = (PreScanDly << 16) | HSyncLen;
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_BG_MIX_CTRL + CrtcState->CrtcID * 4,
    BG_MIX_CTRL_MASK,
    BG_MIX_CTRL_SHIFT,
    BgDly,
    FALSE
    );
  Vop2Writel (Vop2->BaseAddress, RK3568_VP0_PRE_SCAN_HTIMING + VPOffset, PreScanDly);

  MmioWrite32 (0xfdd906e8, 0x34000000);
}

STATIC
INT32
Vop2WaitPowerDomainOn (
  OUT VOP2                    *Vop2,
  OUT VOP2_POWER_DOMAIN_DATA  *PdData
  )
{
  UINT32   Val      = 0;
  BOOLEAN  IsBisrEn = FALSE;

  IsBisrEn = Vop2GrfRead (Vop2->SysPmu, RK3588_PMU_BISR_CON3, EN_MASK, PdData->BisrEnStatusShift);
  if (IsBisrEn) {
    return readl_poll_timeout (
             Vop2->SysPmu + RK3588_PMU_BISR_STATUS5,
             Val,
             ((Val >> PdData->PmuStatusShift) & 0x1),
             50 * 1000
             );
  } else {
    return readl_poll_timeout (
             Vop2->BaseAddress + RK3568_SYS_STATUS0,
             Val,
             !((Val >> PdData->PdStatusShift) & 0x1),
             50 * 1000
             );
  }
}

STATIC
EFI_STATUS
Vop2PowerDomainOn (
  OUT VOP2                    *Vop2,
  OUT VOP2_POWER_DOMAIN_DATA  *PdData
  )
{
  INT32  Ret = 0;

  if (!PdData) {
    return 0;
  }

  if (PdData->ParentPdData) {
    Ret = Vop2PowerDomainOn (Vop2, PdData->ParentPdData);
    if (Ret) {
      DEBUG ((DEBUG_ERROR, "can't open parent power domain\n"));
      return EFI_INVALID_PARAMETER;
    }
  }

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_SYS_PD_CTRL,
    EN_MASK,
    PdData->PdEnShift,
    0,
    FALSE
    );
  Ret = Vop2WaitPowerDomainOn (Vop2, PdData);
  if (Ret) {
    DEBUG ((DEBUG_ERROR, "wait vop2 power domain timeout\n"));
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
Vop2SetClk (
  IN UINT32  CrtcId,
  IN UINT64  Rate
  )
{
  /* only support VP2 for now */
  ASSERT (CrtcId == 2);

  /* CRU_MODE_CON00: set clock modes */
  MmioWrite32 (CRU_BASE + 0x0280, 0xFFFF0155);

  HAL_CRU_ClkSetFreq (DCLK_VOP2_SRC, Rate);

  DEBUG ((
    DEBUG_INFO,
    "%a: V0PLL=%lu, DCLK_VOP2_SRC=%lu\n",
    __func__,
    HAL_CRU_ClkGetFreq (PLL_V0PLL),
    HAL_CRU_ClkGetFreq (DCLK_VOP2_SRC)
    ));

  return EFI_SUCCESS;
}

EFI_STATUS
Vop2PreInit (
  IN  ROCKCHIP_CRTC_PROTOCOL  *This,
  OUT DISPLAY_STATE           *DisplayState
  )
{
  CRTC_STATE  *CrtcState = &DisplayState->CrtcState;

  if (!RockchipVop2) {
    RockchipVop2              = AllocatePool (sizeof (*RockchipVop2));
    RockchipVop2->BaseAddress = RK3588_VOP2_REG_BASE;
    RockchipVop2->SysPmu      = SYS_PMU_BASE;
    RockchipVop2->Version     = mVop2RK3588.Version;
    RockchipVop2->Data        = &mVop2RK3588;
    RockchipVop2->GlobalInit  = FALSE;
    ZeroMem (RockchipVop2->VpPlaneMask, sizeof (VOP2_VP_PLANE_MASK) * VOP2_VP_MAX);
  }

  CrtcState->Private   = RockchipVop2;
  CrtcState->MaxOutput = RockchipVop2->Data->VpData[CrtcState->CrtcID].MaxOutput;
  CrtcState->Feature   = RockchipVop2->Data->VpData[CrtcState->CrtcID].Feature;

  /* fix the plane mask about */
  Vop2GlobalInitial (DisplayState);

  return EFI_SUCCESS;
}

#define OUTPUT_IF_IS_HDMI(x)  ((x) & (VOP_OUTPUT_IF_HDMI0 | VOP_OUTPUT_IF_HDMI1))
#define OUTPUT_IF_IS_DP(x)    ((x) & (VOP_OUTPUT_IF_DP0 | VOP_OUTPUT_IF_DP1))

STATIC
VOID
Vop2PostColorSwap (
  OUT DISPLAY_STATE  *DisplayState,
  OUT VOP2           *Vop2
  )
{
  CRTC_STATE       *CrtcState      = &DisplayState->CrtcState;
  CONNECTOR_STATE  *ConnectorState = &DisplayState->ConnectorState;
  UINT32           VPOffset        = CrtcState->CrtcID * 0x100;
  UINT32           OutputIf        = ConnectorState->OutputInterface;
  UINT32           DateSwap        = 0;

  if (IsUVSwap (ConnectorState->BusFormat, ConnectorState->OutputMode)) {
    DateSwap = DSP_RB_SWAP;
  }

  if ((Vop2->Version == VOP_VERSION_RK3588) &&
      (OUTPUT_IF_IS_HDMI (OutputIf) || OUTPUT_IF_IS_DP (OutputIf)) &&
      ((ConnectorState->BusFormat == MEDIA_BUS_FMT_YUV8_1X24) ||
       (ConnectorState->BusFormat == MEDIA_BUS_FMT_YUV10_1X30)))
  {
    DateSwap |= DSP_RG_SWAP;
  }

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_DSP_CTRL + VPOffset,
    DATA_SWAP_MASK,
    DATA_SWAP_SHIFT,
    DateSwap,
    FALSE
    );
}

STATIC
VOID
Vop2CalcDscCruCfg (
  OUT DISPLAY_STATE  *DisplayState,
  OUT UINT32         *dsc_txp_clk_div,
  OUT UINT32         *dsc_pxl_clk_div,
  OUT UINT32         *dsc_cds_clk_div,
  IN UINT64          dclk_rate
  )
{
  CRTC_STATE  *CrtcState = &DisplayState->CrtcState;

  *dsc_txp_clk_div = dclk_rate / CrtcState->dsc_txp_clk_rate;
  *dsc_pxl_clk_div = dclk_rate / CrtcState->dsc_pxl_clk_rate;
  *dsc_cds_clk_div = dclk_rate / CrtcState->dsc_cds_clk_rate;

  *dsc_txp_clk_div = LogCalculate (*dsc_txp_clk_div);
  *dsc_pxl_clk_div = LogCalculate (*dsc_pxl_clk_div);
  *dsc_cds_clk_div = LogCalculate (*dsc_cds_clk_div);
}

STATIC
VOID
Vop2LoadPps (
  OUT DISPLAY_STATE  *DisplayState,
  OUT VOP2           *Vop2,
  IN UINT8           dsc_id
  )
{
  CRTC_STATE                            *CrtcState = &DisplayState->CrtcState;
  struct drm_dsc_picture_parameter_set  *pps       = &CrtcState->pps;
  struct drm_dsc_picture_parameter_set  config_pps;
  const VOP2_DSC_DATA                   *dsc_data           = &Vop2->Data->DscData[dsc_id];
  UINT32                                *pps_val            = (UINT32 *)&config_pps;
  UINT32                                decoder_regs_offset = (dsc_id * 0x100);
  UINT32                                i                   = 0;

  memcpy (&config_pps, pps, sizeof (config_pps));

  if ((config_pps.pps_3 & 0xf) > dsc_data->max_linebuf_depth) {
    config_pps.pps_3 &= 0xf0;
    config_pps.pps_3 |= dsc_data->max_linebuf_depth;
    DEBUG ((
      DEBUG_INFO,
      "DSC%d max_linebuf_depth is: %d, current set value is: %d\n",
      dsc_id,
      dsc_data->max_linebuf_depth,
      config_pps.pps_3 & 0xf
      ));
  }

  for (i = 0; i < DSC_NUM_BUF_RANGES; i++) {
    config_pps.rc_range_parameters[i] =
      (pps->rc_range_parameters[i] >> 3 & 0x1f) |
      ((pps->rc_range_parameters[i] >> 14 & 0x3) << 5) |
      ((pps->rc_range_parameters[i] >> 0 & 0x7) << 7) |
      ((pps->rc_range_parameters[i] >> 8 & 0x3f) << 10);
  }

  for (i = 0; i < ROCKCHIP_DSC_PPS_SIZE_BYTE / 4; i++) {
    Vop2Writel (Vop2->BaseAddress, RK3588_DSC_8K_PPS0_3 + decoder_regs_offset + i * 4, *pps_val++);
  }
}

STATIC
VOID
Vop2DscEnable (
  OUT DISPLAY_STATE  *DisplayState,
  OUT VOP2           *Vop2,
  IN UINT8           dsc_id,
  IN UINT64          dclk_rate
  )
{
  CONNECTOR_STATE               *ConnectorState     = &DisplayState->ConnectorState;
  DRM_DISPLAY_MODE              *Mode               = &ConnectorState->DisplayMode;
  CRTC_STATE                    *CrtcState          = &DisplayState->CrtcState;
  struct rockchip_dsc_sink_cap  *dsc_sink_cap       = &CrtcState->dsc_sink_cap;
  const VOP2_DSC_DATA           *dsc_data           = &Vop2->Data->DscData[dsc_id];
  BOOLEAN                       mipi_ds_mode        = FALSE;
  UINT8                         dsc_interface_mode  = 0;
  UINT16                        hsync_len           = Mode->CrtcHSyncEnd - Mode->CrtcHSyncStart;
  UINT16                        hdisplay            = Mode->CrtcHDisplay;
  UINT16                        htotal              = Mode->CrtcHTotal;
  UINT16                        hact_st             = Mode->CrtcHTotal - Mode->CrtcHSyncStart;
  UINT16                        vdisplay            = Mode->CrtcVDisplay;
  UINT16                        vtotal              = Mode->CrtcVTotal;
  UINT16                        vsync_len           = Mode->CrtcVSyncEnd - Mode->CrtcVSyncStart;
  UINT16                        vact_st             = Mode->CrtcVTotal - Mode->CrtcVSyncStart;
  UINT16                        vact_end            = vact_st + vdisplay;
  UINT32                        ctrl_regs_offset    = (dsc_id * 0x30);
  UINT32                        decoder_regs_offset = (dsc_id * 0x100);
  UINT32                        dsc_txp_clk_div     = 0;
  UINT32                        dsc_pxl_clk_div     = 0;
  UINT32                        dsc_cds_clk_div     = 0;
  UINT32                        val                 = 0;

  if (!Vop2->Data->NrDscs) {
    DEBUG ((DEBUG_ERROR, "Unsupported DSC\n"));
    return;
  }

  if (CrtcState->dsc_slice_num > dsc_data->max_slice_num) {
    DEBUG ((
      DEBUG_INFO,
      "DSC%d supported max slice is: %d, current is: %d\n",
      dsc_data->id,
      dsc_data->max_slice_num,
      CrtcState->dsc_slice_num
      ));
  }

  if (dsc_data->PdData) {
    if (Vop2PowerDomainOn (Vop2, dsc_data->PdData)) {
      DEBUG ((DEBUG_ERROR, "open dsc%d pd fail\n", dsc_id));
    }
  }

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3588_DSC_8K_INIT_DLY + ctrl_regs_offset,
    EN_MASK,
    SCAN_TIMING_PARA_IMD_EN_SHIFT,
    1,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3588_DSC_8K_SYS_CTRL + ctrl_regs_offset,
    DSC_PORT_SEL_MASK,
    DSC_PORT_SEL_SHIFT,
    CrtcState->CrtcID,
    FALSE
    );
  if (ConnectorState->OutputInterface & (VOP_OUTPUT_IF_HDMI0 | VOP_OUTPUT_IF_HDMI1)) {
    dsc_interface_mode = VOP_DSC_IF_HDMI;
  } else {
    mipi_ds_mode = !!(ConnectorState->OutputFlags & ROCKCHIP_OUTPUT_MIPI_DS_MODE);
    if (mipi_ds_mode) {
      dsc_interface_mode = VOP_DSC_IF_MIPI_DS_MODE;
    } else {
      dsc_interface_mode = VOP_DSC_IF_MIPI_VIDEO_MODE;
    }
  }

  if (ConnectorState->OutputFlags & ROCKCHIP_OUTPUT_DUAL_CHANNEL_LEFT_RIGHT_MODE) {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3588_DSC_8K_SYS_CTRL + ctrl_regs_offset,
      DSC_MAN_MODE_MASK,
      DSC_MAN_MODE_SHIFT,
      0,
      FALSE
      );
  } else {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3588_DSC_8K_SYS_CTRL + ctrl_regs_offset,
      DSC_MAN_MODE_MASK,
      DSC_MAN_MODE_SHIFT,
      1,
      FALSE
      );
  }

  Vop2CalcDscCruCfg (DisplayState, &dsc_txp_clk_div, &dsc_pxl_clk_div, &dsc_cds_clk_div, dclk_rate);

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3588_DSC_8K_SYS_CTRL + ctrl_regs_offset,
    DSC_INTERFACE_MODE_MASK,
    DSC_INTERFACE_MODE_SHIFT,
    dsc_interface_mode,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3588_DSC_8K_SYS_CTRL + ctrl_regs_offset,
    DSC_PIXEL_NUM_MASK,
    DSC_PIXEL_NUM_SHIFT,
    CrtcState->dsc_pixel_num >> 1,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3588_DSC_8K_SYS_CTRL + ctrl_regs_offset,
    DSC_TXP_CLK_DIV_MASK,
    DSC_TXP_CLK_DIV_SHIFT,
    dsc_txp_clk_div,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3588_DSC_8K_SYS_CTRL + ctrl_regs_offset,
    DSC_PXL_CLK_DIV_MASK,
    DSC_PXL_CLK_DIV_SHIFT,
    dsc_pxl_clk_div,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3588_DSC_8K_SYS_CTRL + ctrl_regs_offset,
    DSC_CDS_CLK_DIV_MASK,
    DSC_CDS_CLK_DIV_SHIFT,
    dsc_cds_clk_div,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3588_DSC_8K_SYS_CTRL + ctrl_regs_offset,
    EN_MASK,
    DSC_SCAN_EN_SHIFT,
    !mipi_ds_mode,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3588_DSC_8K_SYS_CTRL + ctrl_regs_offset,
    DSC_CDS_CLK_DIV_MASK,
    DSC_HALT_EN_SHIFT,
    mipi_ds_mode,
    FALSE
    );

  if (!mipi_ds_mode) {
    UINT16  dsc_hsync, dsc_htotal, dsc_hact_st, dsc_hact_end;
    UINT32  target_bpp = dsc_sink_cap->target_bits_per_pixel_x16;
    UINT64  dsc_cds_rate = CrtcState->dsc_cds_clk_rate;
    UINT32  v_pixclk_mhz = Mode->CrtcClock / 1000; /* video timing pixclk */
    UINT32  dly_num, dsc_cds_rate_mhz, val = 0;
    UINT32  k = 1;

    if (ConnectorState->OutputFlags & ROCKCHIP_OUTPUT_DUAL_CHANNEL_LEFT_RIGHT_MODE) {
      k = 2;
    }

    if (target_bpp >> 4 < dsc_data->min_bits_per_pixel) {
      DEBUG ((DEBUG_WARN, "Unsupported bpp less than: %d\n", dsc_data->min_bits_per_pixel));
    }

    /*
      * dly_num = delay_line_num * T(one-line) / T (dsc_cds)
      * T (one-line) = 1/v_pixclk_mhz * htotal = htotal/v_pixclk_mhz
      * T (dsc_cds) = 1 / dsc_cds_rate_mhz
      *
      * HDMI:
      * delay_line_num: according the pps initial_xmit_delay to adjust vop dsc delay
      *                 delay_line_num = 4 - BPP / 8
      *                                = (64 - target_bpp / 8) / 16
      * dly_num = htotal * dsc_cds_rate_mhz / v_pixclk_mhz * (64 - target_bpp / 8) / 16;
      *
      * MIPI DSI[4320 and 9216 is buffer size for DSC]:
      * DSC0:delay_line_num = 4320 * 8 / slince_num / chunk_size;
      * delay_line_num = delay_line_num > 5 ? 5 : delay_line_num;
      * DSC1:delay_line_num = 9216 * 2 / slince_num / chunk_size;
      * delay_line_num = delay_line_num > 5 ? 5 : delay_line_num;
      * dly_num = htotal * dsc_cds_rate_mhz / v_pixclk_mhz * delay_line_num
      */
    do_div (dsc_cds_rate, 1000000); /* hz to Mhz */
    dsc_cds_rate_mhz = dsc_cds_rate;
    dsc_hsync        = hsync_len / 2;
    if (dsc_interface_mode == VOP_DSC_IF_HDMI) {
      dly_num = htotal * dsc_cds_rate_mhz / v_pixclk_mhz * (64 - target_bpp / 8) / 16;
    } else {
      UINT32  dsc_buf_size   = dsc_id == 0 ? 4320 * 8 : 9216 * 2;
      UINT32  delay_line_num = dsc_buf_size / CrtcState->dsc_slice_num /
                               be16_to_cpu (CrtcState->pps.chunk_size);

      delay_line_num = delay_line_num > 5 ? 5 : delay_line_num;
      dly_num        = htotal * dsc_cds_rate_mhz / v_pixclk_mhz * delay_line_num;

      /* The dsc mipi video mode dsc_hsync minimum size is 8 pixels */
      if (dsc_hsync < 8) {
        dsc_hsync = 8;
      }
    }

    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3588_DSC_8K_INIT_DLY + ctrl_regs_offset,
      DSC_INIT_DLY_MODE_MASK,
      DSC_INIT_DLY_MODE_SHIFT,
      0,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3588_DSC_8K_INIT_DLY + ctrl_regs_offset,
      DSC_INIT_DLY_NUM_MASK,
      DSC_INIT_DLY_NUM_SHIFT,
      dly_num,
      FALSE
      );

    /*
      * htotal / dclk_core = dsc_htotal /cds_clk
      *
      * dclk_core = DCLK / (1 << dclk_core->div_val)
      * cds_clk = txp_clk / (1 << dsc_cds_clk->div_val)
      * txp_clk = DCLK / (1 << dsc_txp_clk->div_val)
      *
      * dsc_htotal = htotal * (1 << dclk_core->div_val) /
      *              ((1 << dsc_txp_clk->div_val) * (1 << dsc_cds_clk->div_val))
      */
    dsc_htotal = htotal * (1 << CrtcState->DclkCoreDiv) /
                 ((1 << dsc_txp_clk_div) * (1 << dsc_cds_clk_div));
    val = dsc_htotal << 16 | dsc_hsync;
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3588_DSC_8K_HTOTAL_HS_END + ctrl_regs_offset,
      DSC_HTOTAL_PW_MASK,
      DSC_HTOTAL_PW_SHIFT,
      val,
      FALSE
      );

    dsc_hact_st  = hact_st / 2;
    dsc_hact_end = (hdisplay / k * target_bpp >> 4) / 24 + dsc_hact_st;
    val          = dsc_hact_end << 16 | dsc_hact_st;
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3588_DSC_8K_HACT_ST_END + ctrl_regs_offset,
      DSC_HACT_ST_END_MASK,
      DSC_HACT_ST_END_SHIFT,
      val,
      FALSE
      );

    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3588_DSC_8K_VTOTAL_VS_END + ctrl_regs_offset,
      DSC_VTOTAL_PW_MASK,
      DSC_VTOTAL_PW_SHIFT,
      vtotal << 16 | vsync_len,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3588_DSC_8K_VACT_ST_END + ctrl_regs_offset,
      DSC_VACT_ST_END_MASK,
      DSC_VACT_ST_END_SHIFT,
      vact_end << 16 | vact_st,
      FALSE
      );
  }

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3588_DSC_8K_RST + ctrl_regs_offset,
    RST_DEASSERT_MASK,
    RST_DEASSERT_SHIFT,
    1,
    FALSE
    );
  udelay (10);

  val |= DSC_CTRL0_DEF_CON | (LogCalculate (CrtcState->dsc_slice_num) << DSC_NSLC_SHIFT) |
         ((dsc_sink_cap->version_minor == 2 ? 1 : 0) << DSC_IFEP_SHIFT);
  Vop2Writel (Vop2->BaseAddress, RK3588_DSC_8K_CTRL0 + decoder_regs_offset, val);

  Vop2LoadPps (DisplayState, Vop2, dsc_id);

  val |= (1 << DSC_PPS_UPD_SHIFT);
  Vop2Writel (Vop2->BaseAddress, RK3588_DSC_8K_CTRL0 + decoder_regs_offset, val);

  DEBUG ((
    DEBUG_INFO,
    "DSC%d: txp:%lld div:%d, pxl:%lld div:%d, dsc:%lld div:%d\n",
    dsc_id,
    CrtcState->dsc_txp_clk_rate,
    dsc_txp_clk_div,
    CrtcState->dsc_pxl_clk_rate,
    dsc_pxl_clk_div,
    CrtcState->dsc_cds_clk_rate,
    dsc_cds_clk_div
    ));
}

STATIC
VOID
Vop2ModeFixup (
  OUT DISPLAY_STATE  *DisplayState
  )
{
  CONNECTOR_STATE   *ConnectorState = &DisplayState->ConnectorState;
  CRTC_STATE        *CrtcState      = &DisplayState->CrtcState;
  DRM_DISPLAY_MODE  *Mode           = &ConnectorState->DisplayMode;

  /*
   * HActive of the video timing must be 4-pixel aligned.
   */
  if (Mode->CrtcHDisplay % 4 != 0) {
    UINT32  OldHDisplay = Mode->CrtcHDisplay;
    UINT32  AlignOffset = 4 - (Mode->CrtcHDisplay % 4);

    Mode->CrtcHDisplay   += AlignOffset;
    Mode->CrtcHSyncStart += AlignOffset;
    Mode->CrtcHSyncEnd   += AlignOffset;
    Mode->CrtcHTotal     += AlignOffset;

    DEBUG ((
      DEBUG_WARN,
      "WARN: VP%d: HActive must be 4-pixel aligned: %u -> %u\n",
      CrtcState->CrtcID,
      OldHDisplay,
      Mode->CrtcHDisplay
      ));
  }
}

EFI_STATUS
Vop2Init (
  IN  ROCKCHIP_CRTC_PROTOCOL  *This,
  OUT DISPLAY_STATE           *DisplayState
  )
{
  CONNECTOR_STATE   *ConnectorState = &DisplayState->ConnectorState;
  DRM_DISPLAY_MODE  *Mode = &ConnectorState->DisplayMode;
  CRTC_STATE        *CrtcState = &DisplayState->CrtcState;
  VOP2              *Vop2 = CrtcState->Private;
  UINT32            VPOffset = CrtcState->CrtcID * 0x100;
  UINT32            LineFlagOffset = CrtcState->CrtcID * 0x4;
  UINT16            HSyncLen, HDisplay, HTotal, HActStart, HActEnd;
  UINT16            VSyncLen, VDisplay, VTotal, VActStart, VActEnd;
  UINT32            Val, ActEnd;
  UINT8             DitherDownEn;
  UINT8             PreDitherDownEn;
  BOOLEAN           YUVOverlay;
  UINT64            DclkRate;

  Vop2ModeFixup (DisplayState);

  HSyncLen  = Mode->CrtcHSyncEnd - Mode->CrtcHSyncStart;
  HDisplay  = Mode->CrtcHDisplay;
  HTotal    = Mode->CrtcHTotal;
  HActStart = Mode->CrtcHTotal - Mode->CrtcHSyncStart;
  HActEnd   = HActStart + HDisplay;

  VSyncLen  = Mode->CrtcVSyncEnd - Mode->CrtcVSyncStart;
  VDisplay  = Mode->CrtcVDisplay;
  VTotal    = Mode->CrtcVTotal;
  VActStart = Mode->CrtcVTotal - Mode->CrtcVSyncStart;
  VActEnd   = VActStart + VDisplay;

  DEBUG ((
    DEBUG_INIT,
    "[INIT]VOP update mode to: %dx%d%a%d, type: %a for VP%d\n",
    Mode->HDisplay,
    Mode->VDisplay,
    Mode->Flags & DRM_MODE_FLAG_INTERLACE ? "i" : "p",
    Mode->VRefresh,
    GetVopOutputIfName (ConnectorState->OutputInterface),
    CrtcState->CrtcID
    ));

  DclkRate = Vop2IfConfig (DisplayState, Vop2);

  if ((ConnectorState->OutputMode == ROCKCHIP_OUT_MODE_AAAA) &&
      !(CrtcState->Feature & VOP_FEATURE_OUTPUT_10BIT))
  {
    ConnectorState->OutputMode = ROCKCHIP_OUT_MODE_P888;
  }

  Vop2PostColorSwap (DisplayState, Vop2);

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_DSP_CTRL + VPOffset,
    OUT_MODE_MASK,
    OUT_MODE_SHIFT,
    ConnectorState->OutputMode,
    FALSE
    );

  switch (ConnectorState->BusFormat) {
    case MEDIA_BUS_FMT_RGB565_1X16:
      DitherDownEn = 1;
      break;
    case MEDIA_BUS_FMT_RGB666_1X18:
    case MEDIA_BUS_FMT_RGB666_1X24_CPADHI:
    case MEDIA_BUS_FMT_RGB666_1X7X3_SPWG:
    case MEDIA_BUS_FMT_RGB666_1X7X3_JEIDA:
      DitherDownEn = 1;
      break;
    case MEDIA_BUS_FMT_YUV8_1X24:
    case MEDIA_BUS_FMT_UYYVYY8_0_5X24:
      DitherDownEn    = 0;
      PreDitherDownEn = 1;
      break;
    case MEDIA_BUS_FMT_YUV10_1X30:
    case MEDIA_BUS_FMT_UYYVYY10_0_5X30:
    case MEDIA_BUS_FMT_RGB888_1X24:
    case MEDIA_BUS_FMT_RGB888_1X7X4_SPWG:
    case MEDIA_BUS_FMT_RGB888_1X7X4_JEIDA:
    default:
      DitherDownEn    = 0;
      PreDitherDownEn = 0;
      break;
  }

  if (ConnectorState->OutputMode == ROCKCHIP_OUT_MODE_AAAA) {
    PreDitherDownEn = 0;
  } else {
    PreDitherDownEn = 1;
  }

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_DSP_CTRL + VPOffset,
    EN_MASK,
    DITHER_DOWN_EN_SHIFT,
    DitherDownEn,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_DSP_CTRL + VPOffset,
    EN_MASK,
    PRE_DITHER_DOWN_EN_SHIFT,
    PreDitherDownEn,
    FALSE
    );

  YUVOverlay = IsYUVOutput (ConnectorState->BusFormat) ? 1 : 0;
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_OVL_CTRL,
    EN_MASK,
    CrtcState->CrtcID,
    YUVOverlay,
    FALSE
    );
  CrtcState->YUVOverlay = YUVOverlay;

  Vop2Writel (
    Vop2->BaseAddress,
    RK3568_VP0_DSP_HTOTAL_HS_END + VPOffset,
    (HTotal << 16) | HSyncLen
    );
  Val  = HActStart << 16;
  Val |= HActEnd;

  Vop2Writel (Vop2->BaseAddress, RK3568_VP0_DSP_HACT_ST_END + VPOffset, Val);
  Val  = VActStart << 16;
  Val |= VActEnd;
  Vop2Writel (Vop2->BaseAddress, RK3568_VP0_DSP_VACT_ST_END + VPOffset, Val);
  if (Mode->Flags & DRM_MODE_FLAG_INTERLACE) {
    UINT16  VActStartF1 = VTotal + VActStart + 1;
    UINT16  VActEndF1   = VActStartF1 + VDisplay;

    Val = VActStartF1 << 16 | VActEndF1;
    Vop2Writel (Vop2->BaseAddress, RK3568_VP0_DSP_VACT_ST_END_F1 + VPOffset, Val);
    Val = VTotal << 16 | (VTotal + VSyncLen);
    Vop2Writel (Vop2->BaseAddress, RK3568_VP0_DSP_VS_ST_END_F1 + VPOffset, Val);

    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_VP0_DSP_CTRL + VPOffset,
      EN_MASK,
      INTERLACE_EN_SHIFT,
      1,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_VP0_DSP_CTRL + VPOffset,
      EN_MASK,
      DSP_FILED_POL,
      1,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_VP0_DSP_CTRL + VPOffset,
      EN_MASK,
      P2I_EN_SHIFT,
      1,
      FALSE
      );
    VTotal += VTotal + 1;
    ActEnd  = VActEndF1;
  } else {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_VP0_DSP_CTRL + VPOffset,
      EN_MASK,
      INTERLACE_EN_SHIFT,
      0,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_VP0_DSP_CTRL + VPOffset,
      EN_MASK,
      P2I_EN_SHIFT,
      0,
      FALSE
      );
    ActEnd = VActEnd;
  }

  Vop2Writel (
    Vop2->BaseAddress,
    RK3568_VP0_DSP_VTOTAL_VS_END + VPOffset,
    (VTotal << 16) | VSyncLen
    );

  if (Vop2->Version == VOP_VERSION_RK3568) {
    if (Mode->Flags & DRM_MODE_FLAG_DBLCLK ||
        ConnectorState->OutputInterface & VOP_OUTPUT_IF_BT656)
    {
      Vop2MaskWrite (
        Vop2->BaseAddress,
        RK3568_VP0_DSP_CTRL + VPOffset,
        EN_MASK,
        CORE_DCLK_DIV_EN_SHIFT,
        1,
        FALSE
        );
    } else {
      Vop2MaskWrite (
        Vop2->BaseAddress,
        RK3568_VP0_DSP_CTRL + VPOffset,
        EN_MASK,
        CORE_DCLK_DIV_EN_SHIFT,
        0,
        FALSE
        );
    }
  }

  if (ConnectorState->OutputMode == ROCKCHIP_OUT_MODE_YUV420) {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_VP0_MIPI_CTRL + VPOffset,
      DCLK_DIV2_MASK,
      DCLK_DIV2_SHIFT,
      0x3,
      FALSE
      );
  } else {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_VP0_MIPI_CTRL + VPOffset,
      DCLK_DIV2_MASK,
      DCLK_DIV2_SHIFT,
      0x0,
      FALSE
      );
  }

  if (YUVOverlay) {
    Val = 0x20010200;
  } else {
    Val = 0;
  }

  Vop2Writel (Vop2->BaseAddress, RK3568_VP0_DSP_BG + VPOffset, 0);

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_DSP_CTRL + VPOffset,
    EN_MASK,
    POST_DSP_OUT_R2Y_SHIFT,
    YUVOverlay,
    FALSE
    );

  Vop2TVConfigUpdate (DisplayState, Vop2);
  Vop2PostConfig (DisplayState, Vop2);

  if (CrtcState->dsc_enable) {
    if (ConnectorState->OutputFlags & ROCKCHIP_OUTPUT_DUAL_CHANNEL_LEFT_RIGHT_MODE) {
      Vop2DscEnable (DisplayState, Vop2, 0, DclkRate * 1000LL);
      Vop2DscEnable (DisplayState, Vop2, 1, DclkRate * 1000LL);
    } else {
      Vop2DscEnable (DisplayState, Vop2, CrtcState->dsc_id, DclkRate * 1000LL);
    }
  }

  /*
   * Switch VP DCLK source to the HDMI PHY PLL when HDMI output is enabled,
   * as it's more accurate and necessary for most modes up to 4K @ 60 Hz.
   */
  /* todo: only support VP2 for now */
  ASSERT (CrtcState->CrtcID == 2);
  if (HAL_CRU_ClkGetMux (DCLK_VOP2) == DCLK_VOP2_SEL_DCLK_VOP2_SRC) {
    if (ConnectorState->OutputInterface & VOP_OUTPUT_IF_HDMI0) {
      HAL_CRU_ClkSetMux (DCLK_VOP2, DCLK_VOP2_SEL_CLK_HDMIPHY_PIXEL0_O);
    } else if (ConnectorState->OutputInterface & VOP_OUTPUT_IF_HDMI1) {
      HAL_CRU_ClkSetMux (DCLK_VOP2, DCLK_VOP2_SEL_CLK_HDMIPHY_PIXEL1_O);
    } else {
      Vop2SetClk (CrtcState->CrtcID, DclkRate * 1000);
    }
  }

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_SYS_CTRL_LINE_FLAG0 + LineFlagOffset,
    LINE_FLAG_NUM_MASK,
    RK3568_DSP_LINE_FLAG_NUM0_SHIFT,
    ActEnd,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_SYS_CTRL_LINE_FLAG0 + LineFlagOffset,
    LINE_FLAG_NUM_MASK,
    RK3568_DSP_LINE_FLAG_NUM0_SHIFT,
    ActEnd,
    FALSE
    );

  return EFI_SUCCESS;
}

INLINE
SCALE_MODE
GetSclMode (
  IN  UINT32  Src,
  IN  UINT32  Dst
  )
{
  if (Src < Dst) {
    return SCALE_UP;
  } else if (Src > Dst) {
    return SCALE_DOWN;
  }

  return SCALE_NONE;
}

/*
 * bli_sd_factor = (src - 1) / (dst - 1) << 12;
 * avg_sd_factor:
 * bli_su_factor:
 * bic_su_factor:
 * = (src - 1) / (dst - 1) << 16;
 *
 * gt2 enable: dst get one line from two line of the src
 * gt4 enable: dst get one line from four line of the src.
 *
 */
#define VOP2_BILI_SCL_DN(src, dst)  (((src - 1) << 12) / (dst - 1))
#define VOP2_COMMON_SCL(src, dst)   (((src - 1) << 16) / (dst - 1))

#define VOP2_BILI_SCL_FAC_CHECK(src, dst, fac)   \
                                (fac * (dst - 1) >> 12 < (src - 1))
#define VOP2_COMMON_SCL_FAC_CHECK(src, dst, fac) \
                                (fac * (dst - 1) >> 16 < (src - 1))

STATIC
UINT16
Vop2ScaleFactor (
  IN  SCALE_MODE            Mode,
  IN  VOP2_SCALE_DOWN_MODE  FilterMode,
  IN  UINT32                Src,
  IN  UINT32                Dst
  )
{
  UINT32  Factor = 0;
  INT32   i      = 0;

  if (Mode == SCALE_NONE) {
    return 0;
  }

  if ((Mode == SCALE_DOWN) && (FilterMode == VOP2_SCALE_DOWN_BIL)) {
    Factor = VOP2_BILI_SCL_DN (Src, Dst);
    for (i = 0; i < 100; i++) {
      if (VOP2_BILI_SCL_FAC_CHECK (Src, Dst, Factor)) {
        break;
      }

      Factor -= 1;
      DEBUG ((DEBUG_INFO, "down fac cali: src:%d, dst:%d, fac:0x%x\n", Src, Dst, Factor));
    }
  } else {
    for (i = 0; i < 100; i++) {
      if (VOP2_COMMON_SCL_FAC_CHECK (Src, Dst, Factor)) {
        break;
      }

      Factor -= 1;
      DEBUG ((DEBUG_INFO, "up fac cali:  src:%d, dst:%d, fac:0x%x\n", Src, Dst, Factor));
    }
  }

  return Factor;
}

STATIC
VOID
Vop2SetupScale (
  OUT VOP2           *Vop2,
  OUT VOP2_WIN_DATA  *WinData,
  IN  UINT32         SrcW,
  IN  UINT32         SrcH,
  IN  UINT32         DstW,
  IN  UINT32         DstH
  )
{
  UINT16                YrgbHorSclMode, YrgbVerSclMode;
  UINT16                HsclFilterMode, VsclFilterMode;
  UINT8                 GT2 = 0, GT4 = 0;
  UINT32                XFac = 0, YFac = 0;
  VOP2_SCALE_UP_MODE    HSUFilterMode = VOP2_SCALE_UP_BIC;
  VOP2_SCALE_DOWN_MODE  HSDFilterMode = VOP2_SCALE_DOWN_BIL;
  VOP2_SCALE_UP_MODE    VSUFilterMode = VOP2_SCALE_UP_BIL;
  VOP2_SCALE_DOWN_MODE  VSDFilterMode = VOP2_SCALE_DOWN_BIL;
  UINT32                WinOffset     = WinData->RegOffset;

  if (SrcH > (4 * DstH)) {
    GT4 = 1;
  } else if (SrcH >= (2 * DstH)) {
    GT2 = 1;
  }

  if (GT4) {
    SrcH >>= 2;
  } else if (GT2) {
    SrcH >>= 1;
  }

  YrgbHorSclMode = GetSclMode (SrcW, DstW);
  YrgbVerSclMode = GetSclMode (SrcH, DstH);

  if (YrgbHorSclMode == SCALE_UP) {
    HsclFilterMode = HSUFilterMode;
  } else {
    HsclFilterMode = HSDFilterMode;
  }

  if (YrgbVerSclMode == SCALE_UP) {
    VsclFilterMode = VSUFilterMode;
  } else {
    VsclFilterMode = VSDFilterMode;
  }

  /*
   * RK3568 VOP Esmart/Smart DstW should be even pixel
   * at scale down mode
   */
  if ((YrgbHorSclMode == SCALE_DOWN) && (DstW & 0x1)) {
    DEBUG ((DEBUG_INFO, "win dst_w[%d] should align as 2 pixel\n", DstW));
    DstW += 1;
  }

  XFac = Vop2ScaleFactor (YrgbHorSclMode, HsclFilterMode, SrcW, DstW);
  YFac = Vop2ScaleFactor (YrgbVerSclMode, VsclFilterMode, SrcH, DstH);

  if (WinData->Type == CLUSTER_LAYER) {
    Vop2Writel (Vop2->BaseAddress, RK3568_CLUSTER0_WIN0_SCL_FACTOR_YRGB + WinOffset, YFac << 16 | XFac);
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_CLUSTER0_WIN0_CTRL1 + WinOffset,
      YRGB_GT2_MASK,
      CLUSTER_YRGB_GT2_SHIFT,
      GT2,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_CLUSTER0_WIN0_CTRL1 + WinOffset,
      YRGB_GT4_MASK,
      CLUSTER_YRGB_GT4_SHIFT,
      GT4,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_CLUSTER0_WIN0_CTRL1 + WinOffset,
      YRGB_XSCL_MODE_MASK,
      CLUSTER_YRGB_XSCL_MODE_SHIFT,
      YrgbHorSclMode,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_CLUSTER0_WIN0_CTRL1 + WinOffset,
      YRGB_YSCL_MODE_MASK,
      CLUSTER_YRGB_YSCL_MODE_SHIFT,
      YrgbVerSclMode,
      FALSE
      );
  } else {
    Vop2Writel (Vop2->BaseAddress, RK3568_ESMART0_REGION0_SCL_FACTOR_YRGB + WinOffset, YFac << 16 | XFac);
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_ESMART0_REGION0_CTRL + WinOffset,
      YRGB_GT2_MASK,
      YRGB_GT2_SHIFT,
      GT2,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_ESMART0_REGION0_CTRL + WinOffset,
      YRGB_GT4_MASK,
      YRGB_GT4_SHIFT,
      GT4,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_ESMART0_REGION0_SCL_CTRL + WinOffset,
      YRGB_XSCL_MODE_MASK,
      YRGB_XSCL_MODE_SHIFT,
      YrgbHorSclMode,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_ESMART0_REGION0_SCL_CTRL + WinOffset,
      YRGB_YSCL_MODE_MASK,
      YRGB_YSCL_MODE_SHIFT,
      YrgbVerSclMode,
      FALSE
      );

    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_ESMART0_REGION0_SCL_CTRL + WinOffset,
      YRGB_XSCL_FILTER_MODE_MASK,
      YRGB_XSCL_FILTER_MODE_SHIFT,
      HsclFilterMode,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3568_ESMART0_REGION0_SCL_CTRL + WinOffset,
      YRGB_YSCL_FILTER_MODE_MASK,
      YRGB_YSCL_FILTER_MODE_SHIFT,
      VsclFilterMode,
      FALSE
      );
  }
}

STATIC
VOID
Vop2SetClusterWin (
  OUT DISPLAY_STATE  *DisplayState,
  OUT VOP2_WIN_DATA  *WinData
  )
{
  CRTC_STATE        *CrtcState      = &DisplayState->CrtcState;
  CONNECTOR_STATE   *ConnectorState = &DisplayState->ConnectorState;
  DRM_DISPLAY_MODE  *Mode           = &ConnectorState->DisplayMode;
  VOP2              *Vop2           = CrtcState->Private;
  INT32             SrcW            = CrtcState->SrcW;
  INT32             SrcH            = CrtcState->SrcH;
  INT32             CrtcW           = CrtcState->CrtcW;
  INT32             CrtcH           = CrtcState->CrtcH;
  INT32             CrtcX           = CrtcState->CrtcX;
  INT32             CrtcY           = CrtcState->CrtcY;
  INT32             XVirtual        = CrtcState->XVirtual;
  INT32             YMirror         = 0;
  INT32             CSCMode;
  UINT32            ActInfo, DspInfo, DspSt, DspStx, DspSty;
  UINT32            WinOffset = WinData->RegOffset;
  UINT32            CfgDone   = CFG_DONE_EN | BIT (CrtcState->CrtcID) | (BIT (CrtcState->CrtcID) << 16);

  ActInfo  = (SrcH - 1) << 16;
  ActInfo |= (SrcW - 1) & 0xffff;

  DspInfo  = (CrtcH - 1) << 16;
  DspInfo |= (CrtcW - 1) & 0xffff;

  DspStx = CrtcX;
  DspSty = CrtcY;
  DspSt  = DspSty << 16 | (DspStx & 0xffff);

  if (Mode->Flags & DRM_MODE_FLAG_YMIRROR) {
    YMirror = 1;
  } else {
    YMirror = 0;
  }

  Vop2SetupScale (Vop2, WinData, SrcW, SrcH, CrtcW, CrtcH);

  if (YMirror) {
    DEBUG ((DEBUG_WARN, "WARN: y mirror is unsupported by cluster window\n"));
  }

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_CLUSTER0_WIN0_CTRL0 + WinOffset,
    WIN_FORMAT_MASK,
    WIN_FORMAT_SHIFT,
    CrtcState->Format,
    FALSE
    );
  Vop2Writel (Vop2->BaseAddress, RK3568_CLUSTER0_WIN0_VIR + WinOffset, XVirtual);
  Vop2Writel (Vop2->BaseAddress, RK3568_CLUSTER0_WIN0_YRGB_MST + WinOffset, CrtcState->DMAAddress);

  Vop2Writel (Vop2->BaseAddress, RK3568_CLUSTER0_WIN0_ACT_INFO + WinOffset, ActInfo);
  Vop2Writel (Vop2->BaseAddress, RK3568_CLUSTER0_WIN0_DSP_INFO + WinOffset, DspInfo);
  Vop2Writel (Vop2->BaseAddress, RK3568_CLUSTER0_WIN0_DSP_ST + WinOffset, DspSt);

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_CLUSTER0_WIN0_CTRL0 + WinOffset,
    EN_MASK,
    WIN_EN_SHIFT,
    1,
    FALSE
    );

  CSCMode = Vop2ConvertCSCMode (ConnectorState->ColorSpace);
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_CLUSTER0_WIN0_CTRL0 + WinOffset,
    EN_MASK,
    CLUSTER_RGB2YUV_EN_SHIFT,
    IsYUVOutput (ConnectorState->BusFormat),
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_CLUSTER0_WIN0_CTRL0 + WinOffset,
    CSC_MODE_MASK,
    CLUSTER_CSC_MODE_SHIFT,
    CSCMode,
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_CLUSTER0_CTRL + WinOffset,
    EN_MASK,
    CLUSTER_EN_SHIFT,
    1,
    FALSE
    );

  Vop2Writel (Vop2->BaseAddress, RK3568_REG_CFG_DONE, CfgDone);
}

STATIC
VOID
Vop2SetSmartWin (
  OUT DISPLAY_STATE  *DisplayState,
  OUT VOP2_WIN_DATA  *WinData
  )
{
  CRTC_STATE        *CrtcState      = &DisplayState->CrtcState;
  CONNECTOR_STATE   *ConnectorState = &DisplayState->ConnectorState;
  DRM_DISPLAY_MODE  *Mode           = &ConnectorState->DisplayMode;
  VOP2              *Vop2           = CrtcState->Private;
  INT32             SrcW            = CrtcState->SrcW;
  INT32             SrcH            = CrtcState->SrcH;
  INT32             CrtcW           = CrtcState->CrtcW;
  INT32             CrtcH           = CrtcState->CrtcH;
  INT32             CrtcX           = CrtcState->CrtcX;
  INT32             CrtcY           = CrtcState->CrtcY;
  INT32             XVirtual        = CrtcState->XVirtual;
  INT32             YMirror         = 0;
  INT32             CSCMode;
  UINT32            ActInfo, DspInfo, DspSt, DspStx, DspSty;
  UINT32            WinOffset = WinData->RegOffset;
  UINT32            CfgDone   = CFG_DONE_EN | BIT (CrtcState->CrtcID) | (BIT (CrtcState->CrtcID) << 16);

  /*
   * This is workaround solution for IC design:
   * esmart can't support scale down when actual_w % 16 == 1.
   */
  if ((SrcW > CrtcW) && ((SrcW & 0xf) == 1)) {
    DEBUG ((DEBUG_WARN, "WARN: vp%d unsupported act_w[%d] mode 16 = 1 when scale down\n", CrtcState->CrtcID, SrcW));
    SrcW -= 1;
  }

  ActInfo  = (SrcH - 1) << 16;
  ActInfo |= (SrcW - 1) & 0xffff;

  DspInfo  = (CrtcH - 1) << 16;
  DspInfo |= (CrtcW - 1) & 0xffff;

  DspStx = CrtcX;
  DspSty = CrtcY;
  DspSt  = DspSty << 16 | (DspStx & 0xffff);

  if (Mode->Flags & DRM_MODE_FLAG_YMIRROR) {
    YMirror = 1;
  } else {
    YMirror = 0;
  }

  Vop2SetupScale (Vop2, WinData, SrcW, SrcH, CrtcW, CrtcH);

  if (YMirror) {
    CrtcState->DMAAddress += (SrcH - 1) * XVirtual * 4;
  }

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_ESMART0_CTRL1 + WinOffset,
    EN_MASK,
    YMIRROR_EN_SHIFT,
    YMirror,
    FALSE
    );

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_ESMART0_REGION0_CTRL + WinOffset,
    WIN_FORMAT_MASK,
    WIN_FORMAT_SHIFT,
    CrtcState->Format,
    FALSE
    );
  Vop2Writel (Vop2->BaseAddress, RK3568_ESMART0_REGION0_VIR + WinOffset, XVirtual);
  Vop2Writel (Vop2->BaseAddress, RK3568_ESMART0_REGION0_YRGB_MST + WinOffset, CrtcState->DMAAddress);

  Vop2Writel (Vop2->BaseAddress, RK3568_ESMART0_REGION0_ACT_INFO + WinOffset, ActInfo);
  Vop2Writel (Vop2->BaseAddress, RK3568_ESMART0_REGION0_DSP_INFO + WinOffset, DspInfo);
  Vop2Writel (Vop2->BaseAddress, RK3568_ESMART0_REGION0_DSP_ST + WinOffset, DspSt);

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_ESMART0_REGION0_CTRL + WinOffset,
    EN_MASK,
    WIN_EN_SHIFT,
    1,
    FALSE
    );

  CSCMode = Vop2ConvertCSCMode (ConnectorState->ColorSpace);
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_ESMART0_CTRL0 + WinOffset,
    EN_MASK,
    RGB2YUV_EN_SHIFT,
    IsYUVOutput (ConnectorState->BusFormat),
    FALSE
    );
  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_ESMART0_CTRL0 + WinOffset,
    CSC_MODE_MASK,
    CSC_MODE_SHIFT,
    CSCMode,
    FALSE
    );

  Vop2Writel (Vop2->BaseAddress, RK3568_REG_CFG_DONE, CfgDone);
}

EFI_STATUS
Vop2SetPlane (
  IN  ROCKCHIP_CRTC_PROTOCOL  *This,
  OUT DISPLAY_STATE           *DisplayState
  )
{
  CRTC_STATE     *CrtcState = &DisplayState->CrtcState;
  VOP2           *Vop2      = CrtcState->Private;
  VOP2_WIN_DATA  *WinData;
  UINT8          PrimaryPlaneID = Vop2->VpPlaneMask[CrtcState->CrtcID].PrimaryPlaneId;

  if (CrtcState->CrtcW > CrtcState->MaxOutput.Width) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: output w[%d] exceeded max width[%d]\n",
      CrtcState->CrtcW,
      CrtcState->MaxOutput.Width
      ));
    return -EFI_INVALID_PARAMETER;
  }

  WinData = Vop2FindWinByPhysID (Vop2, PrimaryPlaneID);
  if (!WinData) {
    DEBUG ((DEBUG_ERROR, "invalid win id %d\n", PrimaryPlaneID));
    return -EFI_INVALID_PARAMETER;
  }

  if (WinData->Type == CLUSTER_LAYER) {
    Vop2SetClusterWin (DisplayState, WinData);
  } else {
    Vop2SetSmartWin (DisplayState, WinData);
  }

  DEBUG ((
    DEBUG_INFO,
    "VOP VP%d enable %a[%dx%d->%dx%d@%dx%d] fmt[%d] addr[0x%x]\n",
    CrtcState->CrtcID,
    GetPlaneName (PrimaryPlaneID),
    CrtcState->SrcW,
    CrtcState->SrcH,
    CrtcState->CrtcW,
    CrtcState->CrtcH,
    CrtcState->CrtcX,
    CrtcState->CrtcY,
    CrtcState->Format,
    CrtcState->DMAAddress
    ));

  return EFI_SUCCESS;
}

STATIC
VOID
Vop2DscCfgDone (
  IN  ROCKCHIP_CRTC_PROTOCOL  *This,
  OUT DISPLAY_STATE           *DisplayState
  )
{
  CONNECTOR_STATE  *ConnectorState  = &DisplayState->ConnectorState;
  CRTC_STATE       *CrtcState       = &DisplayState->CrtcState;
  VOP2             *Vop2            = CrtcState->Private;
  UINT8            dsc_id           = CrtcState->dsc_id;
  UINT32           ctrl_regs_offset = (dsc_id * 0x30);

  if (ConnectorState->OutputFlags & ROCKCHIP_OUTPUT_DUAL_CHANNEL_LEFT_RIGHT_MODE) {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3588_DSC_8K_CFG_DONE,
      EN_MASK,
      DSC_CFG_DONE_SHIFT,
      1,
      FALSE
      );
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3588_DSC_8K_CFG_DONE + 0x30,
      EN_MASK,
      DSC_CFG_DONE_SHIFT,
      1,
      FALSE
      );
  } else {
    Vop2MaskWrite (
      Vop2->BaseAddress,
      RK3588_DSC_8K_CFG_DONE + ctrl_regs_offset,
      EN_MASK,
      DSC_CFG_DONE_SHIFT,
      1,
      FALSE
      );
  }
}

EFI_STATUS
Vop2Enable (
  IN  ROCKCHIP_CRTC_PROTOCOL  *This,
  OUT DISPLAY_STATE           *DisplayState
  )
{
  CRTC_STATE  *CrtcState = &DisplayState->CrtcState;
  VOP2        *Vop2      = CrtcState->Private;
  UINT32      VPOffset   = CrtcState->CrtcID * 0x100;
  UINT32      CfgDone    = CFG_DONE_EN | BIT (CrtcState->CrtcID) | (BIT (CrtcState->CrtcID) << 16);

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_DSP_CTRL + VPOffset,
    EN_MASK,
    STANDBY_EN_SHIFT,
    0,
    FALSE
    );
  Vop2Writel (Vop2->BaseAddress, RK3568_REG_CFG_DONE, CfgDone);

  if (CrtcState->dsc_enable) {
    Vop2DscCfgDone (This, DisplayState);
  }

 #ifdef DEBUG_DUMP_REG
  Vop2DumpRegisters (DisplayState, Vop2FindWinByPhysID (Vop2, Vop2->VpPlaneMask[CrtcState->CrtcID].PrimaryPlaneId));
 #endif

  return EFI_SUCCESS;
}

EFI_STATUS
Vop2Disable (
  IN  ROCKCHIP_CRTC_PROTOCOL  *This,
  OUT DISPLAY_STATE           *DisplayState
  )
{
  CRTC_STATE  *CrtcState = &DisplayState->CrtcState;
  VOP2        *Vop2      = CrtcState->Private;
  UINT32      VPOffset   = CrtcState->CrtcID * 0x100;
  UINT32      CfgDone    = CFG_DONE_EN | BIT (CrtcState->CrtcID) | (BIT (CrtcState->CrtcID) << 16);

  Vop2MaskWrite (
    Vop2->BaseAddress,
    RK3568_VP0_DSP_CTRL + VPOffset,
    EN_MASK,
    STANDBY_EN_SHIFT,
    1,
    FALSE
    );
  Vop2Writel (Vop2->BaseAddress, RK3568_REG_CFG_DONE, CfgDone);

  return EFI_SUCCESS;
}

STATIC ROCKCHIP_CRTC_PROTOCOL  mVop2 = {
  &mVop2RK3588,
  Vop2PreInit,
  Vop2Init,
  NULL,
  Vop2SetPlane,
  NULL,
  Vop2Enable,
  Vop2Disable,
  NULL,
  {
  },
  { },
  FALSE,
  FALSE,
  FALSE
};

EFI_STATUS
Vop2DxeInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  Handle = NULL;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gRockchipCrtcProtocolGuid,
                  &mVop2,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
