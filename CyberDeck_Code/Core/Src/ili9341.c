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

  /* Power control 1: set the GVDD voltage. */
  ILI9341_WriteCommand(display, ILI9341_CMD_PWCTRL1);
  ILI9341_WriteData(display, 0x23U);

  /* Power control 2: set the step-up factor for the operating voltage. */
  ILI9341_WriteCommand(display, ILI9341_CMD_PWCTRL2);
  ILI9341_WriteData(display, 0x10U);

  /* VCOM control 1: set VCOMH and VCOML. */
  ILI9341_WriteCommand(display, ILI9341_CMD_VMCTRL1);
  ILI9341_WriteData(display, 0x3EU); // Values are based on other common examples
  ILI9341_WriteData(display, 0x28U);

  /* VCOM control 2: set the VCOM offset voltage. */
  ILI9341_WriteCommand(display, ILI9341_CMD_VMCTRL2);
  ILI9341_WriteData(display, 0x86U);

  /* Entry mode: enable the standard 65K-color display mode. */
  ILI9341_WriteCommand(display, ILI9341_CMD_ETMOD);
  ILI9341_WriteData(display, 0x07U);

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

void ILI9341_SetBacklight(const ILI9341_HandleTypeDef *display,
                          uint8_t brightness)
{
  if (!ILI9341_DEBUG_CHECK_DISPLAY(display))
  {
    return;
  }

  ILI9341_WriteCommand(display, ILI9341_CMD_WRDISBV);
  ILI9341_WriteData(display, brightness);
}

void ILI9341_DrawPixel(const ILI9341_HandleTypeDef *display,
                       uint16_t x,
                       uint16_t y,
                       uint16_t color)
{
  if (!ILI9341_DEBUG_CHECK_POINT(display, x, y))
  {
    return;
  }

  ILI9341_SetColumnAddress(display, x, x);
  ILI9341_SetPageAddress(display, y, y);
  ILI9341_WriteCommand(display, ILI9341_CMD_RAMWR);

  HAL_GPIO_WritePin(display->cs_port, display->cs_pin, GPIO_PIN_RESET);
  ILI9341_Write8Bit(display, (uint8_t)(color >> 8), 1U);
  ILI9341_Write8Bit(display, (uint8_t)color, 1U);
  HAL_GPIO_WritePin(display->cs_port, display->cs_pin, GPIO_PIN_SET);
}

void ILI9341_DrawHorizontalLine(const ILI9341_HandleTypeDef *display,
                                uint16_t x0,
                                uint16_t y0,
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t color)
{
  uint32_t pixel_count;

  if ((y0 != y1) || (x0 > x1) ||
      !ILI9341_DEBUG_CHECK_LINE(display, x0, y0, x1, y1))
  {
    return;
  }

  ILI9341_SetColumnAddress(display, x0, x1);
  ILI9341_SetPageAddress(display, y0, y0);
  ILI9341_WriteCommand(display, ILI9341_CMD_RAMWR);

  pixel_count = (uint32_t)x1 - (uint32_t)x0 + 1U;
  HAL_GPIO_WritePin(display->cs_port, display->cs_pin, GPIO_PIN_RESET);
  for (uint32_t pixel = 0U; pixel < pixel_count; pixel++)
  {
    ILI9341_Write8Bit(display, (uint8_t)(color >> 8), 1U);
    ILI9341_Write8Bit(display, (uint8_t)color, 1U);
  }
  HAL_GPIO_WritePin(display->cs_port, display->cs_pin, GPIO_PIN_SET);
}

void ILI9341_DrawVerticalLine(const ILI9341_HandleTypeDef *display,
                              uint16_t x0,
                              uint16_t y0,
                              uint16_t x1,
                              uint16_t y1,
                              uint16_t color)
{
  uint32_t pixel_count;

  if ((x0 != x1) || (y0 > y1) ||
      !ILI9341_DEBUG_CHECK_LINE(display, x0, y0, x1, y1))
  {
    return;
  }

  ILI9341_SetColumnAddress(display, x0, x0);
  ILI9341_SetPageAddress(display, y0, y1);
  ILI9341_WriteCommand(display, ILI9341_CMD_RAMWR);

  pixel_count = (uint32_t)y1 - (uint32_t)y0 + 1U;
  HAL_GPIO_WritePin(display->cs_port, display->cs_pin, GPIO_PIN_RESET);
  for (uint32_t pixel = 0U; pixel < pixel_count; pixel++)
  {
    ILI9341_Write8Bit(display, (uint8_t)(color >> 8), 1U);
    ILI9341_Write8Bit(display, (uint8_t)color, 1U);
  }
  HAL_GPIO_WritePin(display->cs_port, display->cs_pin, GPIO_PIN_SET);
}

void ILI9341_DrawLine(const ILI9341_HandleTypeDef *display,
                      uint16_t x0,
                      uint16_t y0,
                      uint16_t x1,
                      uint16_t y1,
                      uint16_t color)
{
  int32_t current_x;
  int32_t current_y;
  int32_t delta_x;
  int32_t delta_y;
  int32_t step_x;
  int32_t step_y;
  int32_t error;
  int32_t doubled_error;

  if (!ILI9341_DEBUG_CHECK_LINE(display, x0, y0, x1, y1))
  {
    return;
  }

  if (y0 == y1)
  {
    ILI9341_DrawHorizontalLine(display, x0, y0, x1, y1, color);
    return;
  }

  if (x0 == x1)
  {
    ILI9341_DrawVerticalLine(display, x0, y0, x1, y1, color);
    return;
  }

  current_x = (int32_t)x0;
  current_y = (int32_t)y0;
  delta_x = (x0 < x1) ? ((int32_t)x1 - (int32_t)x0)
                       : ((int32_t)x0 - (int32_t)x1);
  delta_y = (y0 < y1) ? ((int32_t)y1 - (int32_t)y0)
                       : ((int32_t)y0 - (int32_t)y1);
  step_x = (x0 < x1) ? 1 : -1;
  step_y = (y0 < y1) ? 1 : -1;
  error = delta_x - delta_y;

  while (1)
  {
    ILI9341_DrawPixel(display, (uint16_t)current_x,
                      (uint16_t)current_y, color);

    if ((current_x == (int32_t)x1) && (current_y == (int32_t)y1))
    {
      break;
    }

    doubled_error = 2 * error;
    if (doubled_error > -delta_y)
    {
      error -= delta_y;
      current_x += step_x;
    }
    if (doubled_error < delta_x)
    {
      error += delta_x;
      current_y += step_y;
    }
  }
}

void ILI9341_FillRect(const ILI9341_HandleTypeDef *display,
                      uint16_t x,
                      uint16_t y,
                      uint16_t width,
                      uint16_t height,
                      uint16_t color)
{
  uint32_t pixel_count;

  if (!ILI9341_DEBUG_CHECK_RECT(display, x, y, width, height))
  {
    return;
  }

  ILI9341_SetColumnAddress(display, x,
                           (uint16_t)((uint32_t)x + width - 1U));
  ILI9341_SetPageAddress(display, y,
                         (uint16_t)((uint32_t)y + height - 1U));
  ILI9341_WriteCommand(display, ILI9341_CMD_RAMWR);

  pixel_count = (uint32_t)width * (uint32_t)height;
  HAL_GPIO_WritePin(display->cs_port, display->cs_pin, GPIO_PIN_RESET);
  for (uint32_t pixel = 0U; pixel < pixel_count; pixel++)
  {
    ILI9341_Write8Bit(display, (uint8_t)(color >> 8), 1U);
    ILI9341_Write8Bit(display, (uint8_t)color, 1U);
  }
  HAL_GPIO_WritePin(display->cs_port, display->cs_pin, GPIO_PIN_SET);
}

void ILI9341_FillScreen(const ILI9341_HandleTypeDef *display, uint16_t color)
{
  if (!ILI9341_DEBUG_CHECK_DISPLAY(display))
  {
    return;
  }

  ILI9341_FillRect(display, 0U, 0U,
                   ILI9341_DISPLAY_WIDTH,
                   ILI9341_DISPLAY_HEIGHT,
                   color);
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
