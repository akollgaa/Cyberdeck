/*
 * ILI9341 8-bit MCU parallel interface.
 *
 * The GPIO pins are supplied by the application because the display wiring is
 * board-specific.  The data bus is assumed to be D0..D7, with D0 as the
 * least-significant bit of each byte.
 */
#ifndef ILI9341_H
#define ILI9341_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* Set to 1 to enable debug argument validation in the future implementation. */
#ifndef ILI9341_DEBUG
#define ILI9341_DEBUG 0U
#endif

#define ILI9341_DISPLAY_WIDTH   240U
#define ILI9341_DISPLAY_HEIGHT  320U

/* ILI9341 command codes. */
#define ILI9341_CMD_SWRESET  0x01U /* Software Reset. */
#define ILI9341_CMD_SLPIN    0x10U /* Sleep In / Enter Sleep Mode. */
#define ILI9341_CMD_SLPOUT   0x11U /* Sleep Out. */
#define ILI9341_CMD_PTLON    0x12U /* Partial Mode On. */
#define ILI9341_CMD_NORON    0x13U /* Normal Display Mode On. */
#define ILI9341_CMD_DINVOFF  0x20U /* Display Inversion Off. */
#define ILI9341_CMD_DINVON   0x21U /* Display Inversion On. */
#define ILI9341_CMD_DISPOFF  0x28U /* Display Off. */
#define ILI9341_CMD_DISPON   0x29U /* Display On. */
#define ILI9341_CMD_CASET    0x2AU /* Column Address Set. */
#define ILI9341_CMD_PASET    0x2BU /* Page Address Set. */
#define ILI9341_CMD_RAMWR    0x2CU /* Memory Write. */
#define ILI9341_CMD_ETMOD    0xB7U /* Entry Mode Set. */
#define ILI9341_CMD_PWCTRL1  0xC0U /* Power Control 1. */
#define ILI9341_CMD_PWCTRL2  0xC1U /* Power Control 2. */
#define ILI9341_CMD_VMCTRL1  0xC5U /* VCOM Control 1. */
#define ILI9341_CMD_VMCTRL2  0xC7U /* VCOM Control 2. */
#define ILI9341_CMD_MADCTL   0x36U /* Memory Access Control. */
#define ILI9341_CMD_COLMOD   0x3AU /* Pixel Format Set. */
#define ILI9341_CMD_WRDISBV  0x51U /* Write Display Brightness. */
#define ILI9341_CMD_WRCTRLD  0x53U /* Write Control Display. */

/* COLMOD value for 16-bit/pixel RGB565 transfers (65K colors). */
#define ILI9341_PIXEL_FORMAT_RGB565 0x55U

/* MADCTL bit definitions. */
#define ILI9341_MADCTL_MY   0x80U
#define ILI9341_MADCTL_MX   0x40U
#define ILI9341_MADCTL_MV   0x20U
#define ILI9341_MADCTL_ML   0x10U
#define ILI9341_MADCTL_BGR  0x08U
#define ILI9341_MADCTL_MH   0x04U

/* WRCTRLD bits. */
#define ILI9341_WRCTRLD_BC  0x20U /* Backlight control enable. */
#define ILI9341_WRCTRLD_DD  0x08U /* Display dimming enable. */
#define ILI9341_WRCTRLD_BL  0x01U /* Backlight on. */

typedef struct
{
  GPIO_TypeDef *data_port[8];
  uint16_t data_pin[8];

  GPIO_TypeDef *wr_port;
  uint16_t wr_pin;
  GPIO_TypeDef *rd_port;
  uint16_t rd_pin;
  GPIO_TypeDef *dc_port;
  uint16_t dc_pin;
  GPIO_TypeDef *cs_port;
  uint16_t cs_pin;
  GPIO_TypeDef *reset_port;
  uint16_t reset_pin;
} ILI9341_HandleTypeDef;

typedef enum
{
  ILI9341_MODE_SLEEP = 0,
  ILI9341_MODE_WAKE,
  ILI9341_MODE_DISPLAY_OFF,
  ILI9341_MODE_DISPLAY_ON,
  ILI9341_MODE_INVERSION_OFF,
  ILI9341_MODE_INVERSION_ON,
  ILI9341_MODE_NORMAL,
  ILI9341_MODE_BACKLIGHT
} ILI9341_ModeTypeDef;

/**
 * @brief Initializes the ILI9341 using the supplied 8-bit parallel interface.
 *        The implementation will configure RGB565/16-bit color mode and
 *        enable normal display operation.
 * @param display Pointer to the display hardware configuration.
 * @retval None
 */
void ILI9341_Init(const ILI9341_HandleTypeDef *display);

/**
 * @brief Draws one RGB565 pixel at the specified display coordinate.
 * @param display Pointer to the display hardware configuration.
 * @param x Horizontal pixel coordinate.
 * @param y Vertical pixel coordinate.
 * @param color 16-bit RGB565 pixel color.
 * @retval None
 */
void ILI9341_DrawPixel(const ILI9341_HandleTypeDef *display,
                       uint16_t x,
                       uint16_t y,
                       uint16_t color);

/**
 * @brief Draws an RGB565 line between two display coordinates.
 * @param display Pointer to the display hardware configuration.
 * @param x0 Horizontal coordinate of the line start.
 * @param y0 Vertical coordinate of the line start.
 * @param x1 Horizontal coordinate of the line end.
 * @param y1 Vertical coordinate of the line end.
 * @param color 16-bit RGB565 line color.
 * @retval None
 */
void ILI9341_DrawLine(const ILI9341_HandleTypeDef *display,
                      uint16_t x0,
                      uint16_t y0,
                      uint16_t x1,
                      uint16_t y1,
                      uint16_t color);

/**
 * @brief Draws a horizontal RGB565 line using a direct write loop.
 * @param display Pointer to the display hardware configuration.
 * @param x0 Horizontal coordinate of the line start; must be <= x1.
 * @param y0 Vertical coordinate of the line; must equal y1.
 * @param x1 Horizontal coordinate of the line end.
 * @param y1 Vertical coordinate of the line; must equal y0.
 * @param color 16-bit RGB565 line color.
 * @retval None
 */
void ILI9341_DrawHorizontalLine(const ILI9341_HandleTypeDef *display,
                                uint16_t x0,
                                uint16_t y0,
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t color);

/**
 * @brief Draws a vertical RGB565 line using a direct write loop.
 * @param display Pointer to the display hardware configuration.
 * @param x0 Horizontal coordinate of the line; must equal x1.
 * @param y0 Vertical coordinate of the line start; must be <= y1.
 * @param x1 Horizontal coordinate of the line; must equal x0.
 * @param y1 Vertical coordinate of the line end.
 * @param color 16-bit RGB565 line color.
 * @retval None
 */
void ILI9341_DrawVerticalLine(const ILI9341_HandleTypeDef *display,
                              uint16_t x0,
                              uint16_t y0,
                              uint16_t x1,
                              uint16_t y1,
                              uint16_t color);

/**
 * @brief Draws a filled RGB565 rectangle.
 * @param display Pointer to the display hardware configuration.
 * @param x Horizontal coordinate of the rectangle's top-left corner.
 * @param y Vertical coordinate of the rectangle's top-left corner.
 * @param width Rectangle width in pixels.
 * @param height Rectangle height in pixels.
 * @param color 16-bit RGB565 fill color.
 * @retval None
 */
void ILI9341_FillRect(const ILI9341_HandleTypeDef *display,
                      uint16_t x,
                      uint16_t y,
                      uint16_t width,
                      uint16_t height,
                      uint16_t color);

/**
 * @brief Fills the entire display using the supplied RGB565 color.
 * @param display Pointer to the display hardware configuration.
 * @param color 16-bit RGB565 fill color.
 * @retval None
 */
void ILI9341_FillScreen(const ILI9341_HandleTypeDef *display, uint16_t color);

/**
 * @brief Enables or disables display color inversion.
 * @param display Pointer to the display hardware configuration.
 * @param enable Nonzero enables inversion; zero disables inversion.
 * @retval None
 */
void ILI9341_InvertDisplay(const ILI9341_HandleTypeDef *display,
                           uint8_t enable);

/**
 * @brief Performs an ILI9341 software reset.
 * @param display Pointer to the display hardware configuration.
 * @retval None
 */
void ILI9341_SoftwareReset(const ILI9341_HandleTypeDef *display);

#if ILI9341_DEBUG

/**
 * @brief Validates that a display handle is not null.
 * @param display Pointer to the display hardware configuration.
 * @retval Nonzero if valid; zero if display is null.
 */
uint8_t ILI9341_DebugCheckDisplay(const ILI9341_HandleTypeDef *display);

/**
 * @brief Validates a pixel coordinate against the 240x320 display bounds.
 * @param display Pointer to the display hardware configuration.
 * @param x Horizontal coordinate; valid range is 0 through 239.
 * @param y Vertical coordinate; valid range is 0 through 319.
 * @retval Nonzero if valid; zero otherwise.
 */
uint8_t ILI9341_DebugCheckPoint(const ILI9341_HandleTypeDef *display,
                                uint16_t x,
                                uint16_t y);

/**
 * @brief Validates line endpoints and their ordering.
 * @param display Pointer to the display hardware configuration.
 * @param x0 Horizontal coordinate of the line start; must be <= x1.
 * @param y0 Vertical coordinate of the line start; must be <= y1.
 * @param x1 Horizontal coordinate of the line end.
 * @param y1 Vertical coordinate of the line end.
 * @retval Nonzero if valid; zero otherwise.
 */
uint8_t ILI9341_DebugCheckLine(const ILI9341_HandleTypeDef *display,
                               uint16_t x0,
                               uint16_t y0,
                               uint16_t x1,
                               uint16_t y1);

/**
 * @brief Validates a rectangle's origin, dimensions, and display bounds.
 * @param display Pointer to the display hardware configuration.
 * @param x Horizontal coordinate of the top-left corner.
 * @param y Vertical coordinate of the top-left corner.
 * @param width Rectangle width; must be nonzero and fit on screen.
 * @param height Rectangle height; must be nonzero and fit on screen.
 * @retval Nonzero if valid; zero otherwise.
 */
uint8_t ILI9341_DebugCheckRect(const ILI9341_HandleTypeDef *display,
                                uint16_t x,
                                uint16_t y,
                                uint16_t width,
                                uint16_t height);

#define ILI9341_DEBUG_CHECK_DISPLAY(display) \
  ILI9341_DebugCheckDisplay((display))
#define ILI9341_DEBUG_CHECK_POINT(display, x, y) \
  ILI9341_DebugCheckPoint((display), (x), (y))
#define ILI9341_DEBUG_CHECK_LINE(display, x0, y0, x1, y1) \
  ILI9341_DebugCheckLine((display), (x0), (y0), (x1), (y1))
#define ILI9341_DEBUG_CHECK_RECT(display, x, y, width, height) \
  ILI9341_DebugCheckRect((display), (x), (y), (width), (height))

#else

#define ILI9341_DEBUG_CHECK_DISPLAY(display)       (1U)
#define ILI9341_DEBUG_CHECK_POINT(display, x, y)   (1U)
#define ILI9341_DEBUG_CHECK_LINE(display, x0, y0, x1, y1) (1U)
#define ILI9341_DEBUG_CHECK_RECT(display, x, y, width, height) (1U)

#endif /* ILI9341_DEBUG */

/**
 * @brief Sets the backlight brightness level. 
 * @param display Pointer to the display hardware configuration.
 * @param brightness Backlight brightness level (0-255).
 * @retval None
 */
void ILI9341_SetBacklight(const ILI9341_HandleTypeDef *display,
                          uint8_t brightness);

static inline void ILI9341_Write8Bit(const ILI9341_HandleTypeDef *display,
                                     uint8_t value,
                                     uint8_t data_mode)
{
  HAL_GPIO_WritePin(display->dc_port, display->dc_pin,
                    data_mode ? GPIO_PIN_SET : GPIO_PIN_RESET);

  for (uint32_t bit = 0U; bit < 8U; bit++)
  {
    HAL_GPIO_WritePin(display->data_port[bit], display->data_pin[bit],
                      (value & (1U << bit)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }

  HAL_GPIO_WritePin(display->wr_port, display->wr_pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(display->wr_port, display->wr_pin, GPIO_PIN_SET);
}

static inline void ILI9341_WriteCommand(const ILI9341_HandleTypeDef *display,
                                        uint8_t command)
{
  HAL_GPIO_WritePin(display->cs_port, display->cs_pin, GPIO_PIN_RESET);
  ILI9341_Write8Bit(display, command, 0U);
  HAL_GPIO_WritePin(display->cs_port, display->cs_pin, GPIO_PIN_SET);
}

static inline void ILI9341_WriteData(const ILI9341_HandleTypeDef *display,
                                     uint8_t value)
{
  HAL_GPIO_WritePin(display->cs_port, display->cs_pin, GPIO_PIN_RESET);
  ILI9341_Write8Bit(display, value, 1U);
  HAL_GPIO_WritePin(display->cs_port, display->cs_pin, GPIO_PIN_SET);
}

static inline void ILI9341_SetColumnAddress(const ILI9341_HandleTypeDef *display,
                                            uint16_t column_start,
                                            uint16_t column_end)
{
  ILI9341_WriteCommand(display, ILI9341_CMD_CASET);
  ILI9341_WriteData(display, (uint8_t)(column_start >> 8));
  ILI9341_WriteData(display, (uint8_t)column_start);
  ILI9341_WriteData(display, (uint8_t)(column_end >> 8));
  ILI9341_WriteData(display, (uint8_t)column_end);
}

static inline void ILI9341_SetPageAddress(const ILI9341_HandleTypeDef *display,
                                          uint16_t row_start,
                                          uint16_t row_end)
{
  ILI9341_WriteCommand(display, ILI9341_CMD_PASET);
  ILI9341_WriteData(display, (uint8_t)(row_start >> 8));
  ILI9341_WriteData(display, (uint8_t)row_start);
  ILI9341_WriteData(display, (uint8_t)(row_end >> 8));
  ILI9341_WriteData(display, (uint8_t)row_end);
}

static inline void ILI9341_SetMode(const ILI9341_HandleTypeDef *display,
                                   ILI9341_ModeTypeDef mode,
                                   uint8_t value)
{
  switch (mode)
  {
    case ILI9341_MODE_SLEEP:          ILI9341_WriteCommand(display, ILI9341_CMD_SLPIN); break;
    case ILI9341_MODE_WAKE:           ILI9341_WriteCommand(display, ILI9341_CMD_SLPOUT); break;
    case ILI9341_MODE_DISPLAY_OFF:    ILI9341_WriteCommand(display, ILI9341_CMD_DISPOFF); break;
    case ILI9341_MODE_DISPLAY_ON:     ILI9341_WriteCommand(display, ILI9341_CMD_DISPON); break;
    case ILI9341_MODE_INVERSION_OFF:  ILI9341_WriteCommand(display, ILI9341_CMD_DINVOFF); break;
    case ILI9341_MODE_INVERSION_ON:   ILI9341_WriteCommand(display, ILI9341_CMD_DINVON); break;
    case ILI9341_MODE_NORMAL:         ILI9341_WriteCommand(display, ILI9341_CMD_NORON); break;
    case ILI9341_MODE_BACKLIGHT:
      ILI9341_WriteCommand(display, ILI9341_CMD_WRCTRLD);
      ILI9341_WriteData(display, (uint8_t)(ILI9341_WRCTRLD_BC |
                                           (value ? ILI9341_WRCTRLD_BL : 0U)));
      break;
    default: break;
  }
}

#endif /* ILI9341_H */
