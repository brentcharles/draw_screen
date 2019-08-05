# draw_screen
Draw a horizontal line on a screen.

The screen is represented by an array of uint8_t (bytes) where each bit represents a pixel.
The pixels are lit up by writing a 1 to the specified bits which are in the screen.

The draw_screen() algorithm is embedded print_screen_test.c which is the unit test wrapper around
the draw_screen algorithm used to draw the horizontal line on the screen.

# draw_screen() interface

Function Prototype: void draw_screen(uint8_t *screen, int width, int depth, int x1, int x2, int row)

Arguments : screen  - Pointer to an array of bytes to represent a screen
            width   - Width in bytes of the screen (Numbering starts from 1)
            depth   - Depth (i.e. height) in bytes of the screen (Numbering starts from 1)
            x1      - First pixel index in specified row to be drawn (Numbering starts from 0)
            x2      - Last pixel index in specified row to be drawn (Numbering starts from 0)
            row     - Row to draw the line (Numbering starts from 0)
