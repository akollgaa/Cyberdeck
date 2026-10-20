/*
 * ILI9341 8-bit MCU parallel interface implementation.
 */
#include "ili9341.h"

void ILI9341_Init(const ILI9341_HandleTypeDef *display)
{
  if (!ILI9341_DEBUG_CHECK_DISPLAY(display))
  {
    return;
  }

  /* Hardware reset. */
  HAL_GPIO_WritePin(display->reset_port, display->reset_pin, GPIO_PIN_RESET);
  HAL_Delay(5U);
  HAL_GPIO_WritePin(display->reset_port, display->reset_pin, GPIO_PIN_SET);
  HAL_Delay(120U);

  /* Software reset and required power-up sequence. */
  ILI9341_SoftwareReset(display);
  ILI9341_WriteCommand(display, ILI9341_CMD_SLPOUT);
  HAL_Delay(120U);

  /* Select 16-bit RGB565 pixel format (65K colors). */
  ILI9341_WriteCommand(display, ILI9341_CMD_COLMOD);
  ILI9341_WriteData(display, ILI9341_PIXEL_FORMAT_RGB565);

  ILI9341_SetMode(display, ILI9341_MODE_NORMAL, 0U);
  ILI9341_SetMode(display, ILI9341_MODE_DISPLAY_ON, 0U);
}

void ILI9341_SoftwareReset(const ILI9341_HandleTypeDef *display)
{
  if (!ILI9341_DEBUG_CHECK_DISPLAY(display))
  {
    return;
  }

  ILI9341_WriteCommand(display, ILI9341_CMD_SWRESET);
  HAL_Delay(120U);
}

void ILI9341_InvertDisplay(const ILI9341_HandleTypeDef *display,
                           uint8_t enable)
{
  if (!ILI9341_DEBUG_CHECK_DISPLAY(display))
  {
    return;
  }

  ILI9341_SetMode(display,
                  enable ? ILI9341_MODE_INVERSION_ON
                         : ILI9341_MODE_INVERSION_OFF,
                  0U);
}

#if ILI9341_DEBUG

uint8_t ILI9341_DebugCheckDisplay(const ILI9341_HandleTypeDef *display)
{
  return (display != NULL) ? 1U : 0U;
}

uint8_t ILI9341_DebugCheckPoint(const ILI9341_HandleTypeDef *display,
                                uint16_t x,
                                uint16_t y)
{
  if (!ILI9341_DebugCheckDisplay(display))
  {
    return 0U;
  }

  return ((x < ILI9341_DISPLAY_WIDTH) &&
          (y < ILI9341_DISPLAY_HEIGHT)) ? 1U : 0U;
}

uint8_t ILI9341_DebugCheckLine(const ILI9341_HandleTypeDef *display,
                               uint16_t x0,
                               uint16_t y0,
                               uint16_t x1,
                               uint16_t y1)
{
  if (!ILI9341_DebugCheckDisplay(display))
  {
    return 0U;
  }

  return ((x0 <= x1) &&
          (y0 <= y1) &&
          (x1 < ILI9341_DISPLAY_WIDTH) &&
          (y1 < ILI9341_DISPLAY_HEIGHT)) ? 1U : 0U;
}

uint8_t ILI9341_DebugCheckRect(const ILI9341_HandleTypeDef *display,
                               uint16_t x,
                               uint16_t y,
                               uint16_t width,
                               uint16_t height)
{
  uint32_t right;
  uint32_t bottom;

  if (!ILI9341_DebugCheckDisplay(display) || (width == 0U) || (height == 0U))
  {
    return 0U;
  }

  right = (uint32_t)x + (uint32_t)width;
  bottom = (uint32_t)y + (uint32_t)height;

  return ((right <= ILI9341_DISPLAY_WIDTH) &&
          (bottom <= ILI9341_DISPLAY_HEIGHT)) ? 1U : 0U;
}

#endif /* ILI9341_DEBUG */
