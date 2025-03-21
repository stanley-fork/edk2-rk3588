/** @file
 *
 *  Copyright (c) 2024-2025, Mario Bălănică <mariobalanica02@gmail.com>
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#include <Uefi.h>
#include <Protocol/RockchipDsiPanel.h>
#include <Library/GpioLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>

STATIC UINT8  mCsotDsiInitSequence[] = {
  0x0a, 0x31, 0x58, 0x11, 0x00, 0x00, 0x89, 0x30, 0x80, 0x0A, 0x00, 0x06, 0x40, 0x00, 0x28, 0x06, 0x40, 0x06, 0x40, 0x02, 0x00, 0x04, 0x21, 0x00, 0x20, 0x05, 0xD0, 0x00, 0x16, 0x00, 0x0C, 0x02, 0x77, 0x00, 0xDA, 0x18, 0x00, 0x10, 0xE0, 0x03, 0x0C, 0x20, 0x00, 0x06, 0x0B, 0x0B, 0x33, 0x0E, 0x1C, 0x2A, 0x38, 0x46, 0x54, 0x62, 0x69, 0x70, 0x77, 0x79, 0x7B, 0x7D, 0x7E, 0x01, 0x02, 0x01, 0x00, 0x09, 0x40, 0x09, 0xBE, 0x19, 0xFC, 0x19, 0xFA, 0x19, 0xF8, 0x1A, 0x38, 0x1A, 0x78, 0x1A, 0xB6, 0x2A, 0xF6, 0x2B, 0x34, 0x2B, 0x74, 0x3B, 0x74, 0x6B, 0x74,
  0x39, 0x00, 0x06, 0xB9, 0x83, 0x12, 0x1A, 0x55, 0x00,
  0x39, 0x00, 0x03, 0x51, 0x08, 0x00,
  0x39, 0x00, 0x02, 0x53, 0x24,
  0x39, 0x00, 0x1D, 0xB1, 0x1C, 0x6B, 0x6B, 0x27, 0xE7, 0x00, 0x1B, 0x12, 0x20, 0x20, 0x2D, 0x2D, 0x1F, 0x33, 0x31, 0x40, 0xCD, 0xFF, 0x1A, 0x05, 0x15, 0x98, 0x00, 0x88, 0xF9, 0xFF, 0xFF, 0xCF,
  0x39, 0x00, 0x12, 0xB2, 0x00, 0x6A, 0x40, 0x00, 0x00, 0x14, 0x6E, 0x40, 0x73, 0x02, 0x80, 0x21, 0x21, 0x00, 0x00, 0x10, 0x27,
  0x39, 0x00, 0x2D, 0xB4, 0x64, 0x00, 0x08, 0x7F, 0x08, 0x7F, 0x00, 0x62, 0x01, 0x72, 0x01, 0x72, 0x00, 0x60, 0x00, 0x00, 0x0A, 0x08, 0x00, 0x29, 0x05, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x14, 0x00, 0x00, 0x0F, 0x0F, 0x2D, 0x2D,
  0x39, 0x00, 0x04, 0xB6, 0x8F, 0x8F, 0x03,
  0x39, 0x00, 0x03, 0xBC, 0x06, 0x02,
  0x39, 0x00, 0x07, 0xC0, 0x34, 0x34, 0x44, 0x00, 0x08, 0xD8,
  0x39, 0x00, 0x06, 0xC9, 0x00, 0x1E, 0x80, 0xA5, 0x01,
  0x39, 0x00, 0x07, 0xCB, 0x00, 0x13, 0x38, 0x00, 0x0B, 0x27,
  0x39, 0x00, 0x02, 0xCC, 0x02,
  0x39, 0x00, 0x02, 0xD1, 0x07,
  0x39, 0x00, 0x29, 0xD3, 0x00, 0xC0, 0x08, 0x08, 0x08, 0x04, 0x04, 0x04, 0x14, 0x02, 0x07, 0x07, 0x07, 0x31, 0x13, 0x12, 0x12, 0x12, 0x03, 0x03, 0x03, 0x32, 0x10, 0x11, 0x00, 0x11, 0x32, 0x10, 0x03, 0x00, 0x03, 0x32, 0x10, 0x03, 0x00, 0x03, 0x00, 0x00, 0xFF, 0x00,
  0x39, 0x00, 0x31, 0xD5, 0x19, 0x19, 0x18, 0x18, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07, 0x00, 0x00, 0x01, 0x01, 0x18, 0x18, 0x40, 0x40, 0x20, 0x20, 0x18, 0x18, 0x18, 0x18, 0x40, 0x40, 0x18, 0x18, 0x2F, 0x2F, 0x31, 0x31, 0x2F, 0x2F, 0x31, 0x31, 0x18, 0x18, 0x41, 0x41, 0x41, 0x41,
  0x39, 0x00, 0x31, 0xD6, 0x40, 0x40, 0x18, 0x18, 0x05, 0x05, 0x04, 0x04, 0x03, 0x03, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, 0x07, 0x07, 0x06, 0x06, 0x18, 0x18, 0x19, 0x19, 0x20, 0x20, 0x18, 0x18, 0x18, 0x18, 0x40, 0x40, 0x18, 0x18, 0x2F, 0x2F, 0x31, 0x31, 0x2F, 0x2F, 0x31, 0x31, 0x18, 0x18, 0x41, 0x41, 0x41, 0x41,
  0x39, 0x00, 0x40, 0xE1, 0x11, 0x00, 0x00, 0x89, 0x30, 0x80, 0x0A, 0x00, 0x06, 0x40, 0x00, 0x28, 0x06, 0x40, 0x06, 0x40, 0x02, 0x00, 0x04, 0x21, 0x00, 0x20, 0x05, 0xD0, 0x00, 0x16, 0x00, 0x0C, 0x02, 0x77, 0x00, 0xDA, 0x18, 0x00, 0x10, 0xE0, 0x03, 0x0C, 0x20, 0x00, 0x06, 0x0B, 0x0B, 0x33, 0x0E, 0x1C, 0x2A, 0x38, 0x46, 0x54, 0x62, 0x69, 0x70, 0x77, 0x79, 0x7B, 0x7D, 0x7E, 0x01, 0x02, 0x01, 0x00, 0x09,
  0x39, 0x00, 0x0C, 0xE7, 0x06, 0x14, 0x14, 0x1A, 0x23, 0x38, 0x00, 0x23, 0x5D, 0x02, 0x02,
  0x39, 0x00, 0x02, 0xBD, 0x01,
  0x39, 0x00, 0x04, 0xB1, 0x01, 0x23, 0x00,
  0x39, 0x00, 0x25, 0xD8, 0x20, 0x00, 0x02, 0x22, 0x00, 0x00, 0x20, 0x00, 0x02, 0x22, 0x00, 0x00, 0x20, 0x00, 0x02, 0x22, 0x00, 0x00, 0x20, 0x00, 0x02, 0x22, 0x00, 0x00, 0x20, 0x00, 0x02, 0x22, 0x00, 0x00, 0x20, 0x00, 0x02, 0x22, 0x00, 0x00,
  0x39, 0x00, 0x1A, 0xE1, 0x40, 0x09, 0xBE, 0x19, 0xFC, 0x19, 0xFA, 0x19, 0xF8, 0x1A, 0x38, 0x1A, 0x78, 0x1A, 0xB6, 0x2A, 0xF6, 0x2B, 0x34, 0x2B, 0x74, 0x3B, 0x74, 0x6B, 0xF4,
  0x39, 0x00, 0x0D, 0xE7, 0x02, 0x00, 0x40, 0x01, 0x84, 0x13, 0xBE, 0x14, 0x48, 0x00, 0x04, 0x26,
  0x39, 0x00, 0x08, 0xCB, 0x1F, 0x55, 0x03, 0x28, 0x0D, 0x08, 0x0A,
  0x39, 0x00, 0x02, 0xBD, 0x02,
  0x39, 0x00, 0x0D, 0xD8, 0xAF, 0xFF, 0xFA, 0xFA, 0xBF, 0xEA, 0xAF, 0xFF, 0xFA, 0xFA, 0xBF, 0xEA,
  0x39, 0x00, 0x23, 0xE7, 0x01, 0x05, 0x01, 0x03, 0x01, 0x03, 0x04, 0x02, 0x02, 0x24, 0x00, 0x24, 0x81, 0x02, 0x40, 0x00, 0x29, 0x60, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x39, 0x00, 0x02, 0xBD, 0x03,
  0x39, 0x00, 0x19, 0xD8, 0xAA, 0xAA, 0xAA, 0xAB, 0xBF, 0xEA, 0xAA, 0xAA, 0xAA, 0xAB, 0xBF, 0xEA, 0xAF, 0xFF, 0xFA, 0xFA, 0xBF, 0xEA, 0xAF, 0xFF, 0xFA, 0xFA, 0xBF, 0xEA,
  0x39, 0x00, 0x03, 0xE1, 0x01, 0x3F,
  0x39, 0x00, 0x02, 0xBD, 0x00,
  0x39, 0x00, 0x2F, 0xE0, 0x00, 0x13, 0x30, 0x36, 0x40, 0x78, 0x8B, 0x94, 0x95, 0x97, 0x94, 0x94, 0x91, 0x8F, 0x8F, 0x8B, 0x8A, 0x8C, 0x8E, 0xA6, 0xB7, 0x4D, 0x7F, 0x00, 0x13, 0x30, 0x36, 0x40, 0x78, 0x8B, 0x94, 0x95, 0x97, 0x94, 0x94, 0x91, 0x8F, 0x8F, 0x8B, 0x8A, 0x8C, 0x8E, 0xA6, 0xB7, 0x4D, 0x7F,
  0x39, 0x00, 0x05, 0xBA, 0x70, 0x03, 0xA8, 0x92,
  0x39, 0x00, 0x25, 0xD8, 0xEA, 0xAA, 0xAA, 0xAE, 0xAA, 0xAF, 0xEA, 0xAA, 0xAA, 0xAE, 0xAA, 0xAF, 0xE0, 0x00, 0x0A, 0x2E, 0x80, 0x2F, 0xE0, 0x00, 0x0A, 0x2E, 0x80, 0x2F, 0xE0, 0x00, 0x0A, 0x2E, 0x80, 0x2F, 0xE0, 0x00, 0x0A, 0x2E, 0x80, 0x2F,
  0x39, 0x00, 0x02, 0xBD, 0x00,
  0x39, 0x00, 0x02, 0xC1, 0x01,
  0x39, 0x00, 0x02, 0xBD, 0x01,
  0x39, 0x00, 0x3B, 0xC1, 0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x1F, 0x23, 0x27, 0x2B, 0x2F, 0x33, 0x37, 0x3B, 0x3F, 0x43, 0x47, 0x4B, 0x52, 0x5A, 0x62, 0x69, 0x71, 0x79, 0x81, 0x89, 0x91, 0x98, 0xA1, 0xA9, 0xB1, 0xB9, 0xC1, 0xCA, 0xD2, 0xDA, 0xE3, 0xEA, 0xF4, 0xF8, 0xF9, 0xFB, 0xFD, 0xFF, 0x16, 0xA4, 0x44, 0x16, 0x90, 0xE7, 0xF9, 0x71, 0xA0, 0xF3, 0x1F, 0x40,
  0x39, 0x00, 0x02, 0xBD, 0x02,
  0x39, 0x00, 0x3B, 0xC1, 0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24, 0x28, 0x2D, 0x31, 0x35, 0x39, 0x3D, 0x41, 0x45, 0x49, 0x4D, 0x55, 0x5D, 0x65, 0x6D, 0x75, 0x7D, 0x85, 0x8D, 0x94, 0x9C, 0xA4, 0xAC, 0xB4, 0xBC, 0xC4, 0xCC, 0xD4, 0xDC, 0xE4, 0xEC, 0xF4, 0xF8, 0xFA, 0xFC, 0xFE, 0xFF, 0x06, 0xAA, 0xFC, 0x5B, 0xFF, 0xFF, 0xA4, 0xF9, 0x86, 0xF9, 0x55, 0x40,
  0x39, 0x00, 0x02, 0xBD, 0x03,
  0x39, 0x00, 0x3B, 0xC1, 0x00, 0x04, 0x07, 0x0B, 0x0F, 0x13, 0x17, 0x1B, 0x1F, 0x23, 0x27, 0x2C, 0x30, 0x33, 0x38, 0x3C, 0x40, 0x44, 0x48, 0x4C, 0x53, 0x5B, 0x63, 0x6B, 0x72, 0x7A, 0x82, 0x89, 0x91, 0x99, 0xA1, 0xA9, 0xB1, 0xB9, 0xC1, 0xC9, 0xD1, 0xDA, 0xE2, 0xEA, 0xF3, 0xF6, 0xF9, 0xFA, 0xFE, 0xFF, 0x0F, 0x9A, 0xFC, 0x31, 0x40, 0xE4, 0xFB, 0xE9, 0xA3, 0xD9, 0x77, 0x00,
  0x39, 0x00, 0x02, 0xBD, 0x02,
  0x39, 0x00, 0x02, 0xBF, 0x72,
  0x39, 0x00, 0x02, 0xBD, 0x00,
  0x39, 0x00, 0x08, 0xBF, 0xFD, 0x00, 0x80, 0x9C, 0x10, 0x00, 0x80,
  0x39, 0x00, 0x02, 0xE9, 0xDE,
  0x39, 0x00, 0x04, 0xB1, 0xCC, 0x03, 0x00,
  0x39, 0x00, 0x02, 0xE9, 0x3F,
  0x39, 0x00, 0x07, 0xD0, 0x07, 0xC0, 0x08, 0x03, 0x11, 0x00,
  0x39, 0x00, 0x03, 0xB0, 0x00, 0x00,
  0x39, 0x00, 0x02, 0xE9, 0xCF,
  0x39, 0x00, 0x02, 0xBA, 0x03,
  0x39, 0x00, 0x02, 0xE9, 0x3F,
  // 0x39, 0x00, 0x04, 0xB9, 0x83, 0x12, 0x1A,
  // 0x39, 0x00, 0x02, 0xC7, 0x00,
  // 0x39, 0x00, 0x02, 0xCF, 0xFF,

  0x39, 0x00, 0x01, 0x11,
  0x39, 0x00, 0x01, 0x29,
};

EFI_STATUS
EFIAPI
CsotDsiPanelPrepare (
  IN ROCKCHIP_DSI_PANEL_PROTOCOL  *This
  )
{
  /* vcc_lcd_en_pin */
  GpioPinWrite (4, GPIO_PIN_PA3, TRUE);
  GpioPinSetDirection (4, GPIO_PIN_PA3, GPIO_PIN_OUTPUT);
  MicroSecondDelay (5 * 1000);

  /* avdd_lcd_gpio */
  GpioPinWrite (3, GPIO_PIN_PA7, TRUE);
  GpioPinSetDirection (3, GPIO_PIN_PA7, GPIO_PIN_OUTPUT);
  MicroSecondDelay (5 * 1000);

  /* avee_lcd_gpio */
  GpioPinWrite (3, GPIO_PIN_PA6, TRUE);
  GpioPinSetDirection (3, GPIO_PIN_PA6, GPIO_PIN_OUTPUT);
  MicroSecondDelay (5 * 1000);

  MicroSecondDelay (120 * 1000);

  /* lcd_rst_gpio (inverted) */
  GpioPinWrite (3, GPIO_PIN_PC6, FALSE);
  GpioPinSetDirection (3, GPIO_PIN_PC6, GPIO_PIN_OUTPUT);
  MicroSecondDelay (120 * 1000);

  /* lcd_rst_gpio (inverted) */
  GpioPinWrite (3, GPIO_PIN_PC6, TRUE);
  MicroSecondDelay (120 * 1000);

  return EFI_SUCCESS;
}

STATIC ROCKCHIP_DSI_PANEL_PROTOCOL  mCsotDsiPanel = {
  .DsiId       = 0,
  .DsiLaneRate = 676000,
  .DsiLanes    = 4,
  .DsiFlags    = MIPI_DSI_MODE_VIDEO |
                 MIPI_DSI_MODE_VIDEO_BURST |
                 MIPI_DSI_MODE_LPM |
                 MIPI_DSI_MODE_EOT_PACKET,
  .DsiFormat    = MIPI_DSI_FMT_RGB888,
  .DscEnable    = TRUE,
  .SliceWidth   = 1600,
  .SliceHeight  = 40,
  .VersionMajor = 1,
  .VersionMinor = 1,

  .InitSequence       = mCsotDsiInitSequence,
  .InitSequenceLength = ARRAY_SIZE (mCsotDsiInitSequence),

  .NativeMode         = {
    .OscFreq     = 275000,
    .HActive     = 1600,
    .HFrontPorch = 60,
    .HSync       = 20,
    .HBackPorch  = 40,
    .HSyncActive = 0,
    .VActive     = 2560,
    .VFrontPorch = 112,
    .VSync       = 4,
    .VBackPorch  = 18,
    .VSyncActive = 0,
  },

  .Prepare            = CsotDsiPanelPrepare,
};

EFI_STATUS
EFIAPI
AttachCsotDsiPanel (
  VOID
  )
{
  EFI_HANDLE  Handle = NULL;

  return gBS->InstallMultipleProtocolInterfaces (
                &Handle,
                &gRockchipDsiPanelProtocolGuid,
                &mCsotDsiPanel,
                NULL
                );
}
