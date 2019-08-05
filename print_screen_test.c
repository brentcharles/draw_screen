/* print_screen_test.c */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define STATUS_SUCCESS  0
#define STATUS_FAILURE  1

// Uint testing helper defines
#define FAIL(test_num) printf("\nFailure in test %d\n", test_num)

typedef int (*test_func_t)(void);

static int g_tests_run = 0;

// Draw screen algorithm
void draw_screen(uint8_t *screen, int width, int depth, int x1, int x2, int row);

// Private draw screen algorithm helper functions
static bool is_partial_byte(int cur_index, int end_index);
static void draw_next_bit(uint8_t *screen, int next_bit_index);
static void init_screen(uint8_t *screen, int width, int depth);
static void print_screen(uint8_t *screen, int width, int depth);

// Unit testing helper functions
static int run_all_tests(void);
static int verify_test(test_func_t test);
static int screen_check(uint8_t *screen, uint8_t *expected_screen, int num_bytes, int test_num);

// Unit tests
static int test_blank_screen(void);
static int test_first_pixel(void);
static int test_first_two_pixels(void);
static int test_first_byte(void);
static int test_first_two_bytes(void);
static int test_offset_spanning_two_bytes(void);
static int test_full_row(void);
static int test_x2_exceeds_width(void);
static int test_x1_exceeds_x2(void);


int main(int argc, char **argv) {
    int result = run_all_tests();

    if (result == STATUS_SUCCESS) {
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }

    printf("Tests run: %d\n", g_tests_run);

    return result != STATUS_SUCCESS;
}

// Algorithm to draw the screen (i.e. line) based on given (x1, row) through (x2, row) coordinate
void draw_screen(uint8_t *screen, int width, int depth, int x1, int x2, int row)
{
    // Make sure screen is initialized
    if (screen == NULL) {
        return;
    }

    init_screen(screen, width, depth);

    // Enforce a valid request
    if ((x1 > x2) || (x2 >= (width * 8))) {
        return;
    }

    int bit_index  = x1;
    int byte_index = (width * row) + (bit_index / 8);

    while (bit_index <= x2) {
        int bit_offset = bit_index % 8;

        if (is_partial_byte(bit_index, x2)) {
            draw_next_bit(&screen[byte_index], bit_offset);
            byte_index = (bit_offset == 7) ? byte_index + 1 : byte_index;
            bit_index++;
        } else {
            screen[byte_index++] = 0xFF;
            bit_index += 8;
        }
    }

    /*
    //   Below is a slightly simpler, brute force algorithm
    //   It is not as efficient if full bytes shall be written however

    for (i = x1; i <= x2; i++) {
        int bit_offset = i % 8;
        draw_next_bit(&screen[byte_index], bit_offset);
       
        byte_index = (bit_offset == 7) ? byte_index + 1 : byte_index;
    }
    */
}

static bool is_partial_byte(int cur_index, int end_index)
{
    return (((cur_index % 8) != 0) || ((end_index - cur_index) < 8));
}

static void init_screen(uint8_t *screen, int width, int depth)
{
	int i;

	for (i = 0; i < (width * depth); i++) { 
        screen[i] = 0;
    }
}

static void print_screen(uint8_t *screen, int width, int depth)
{
	int i;

	for (i = 0; i < (width * depth); i++) { 
        printf("screen[%d] is: %x\n", i, screen[i]);
    }
}

static void draw_next_bit(uint8_t *screen, int next_bit_index)
{
    *screen |= (0x80 >> next_bit_index);
}

static int run_all_tests(void) {
    int result = STATUS_SUCCESS;

    result |= verify_test(test_blank_screen);
    result |= verify_test(test_first_pixel);
    result |= verify_test(test_first_two_pixels);
    result |= verify_test(test_first_byte);
    result |= verify_test(test_first_two_bytes);
    result |= verify_test(test_offset_spanning_two_bytes);
    result |= verify_test(test_full_row);
    result |= verify_test(test_x2_exceeds_width);
    result |= verify_test(test_x1_exceeds_x2);

    return result;
}

static int verify_test(test_func_t test)
{
    g_tests_run++;

    return (test());
}

static int screen_check(uint8_t *screen, uint8_t *expected_screen, int num_bytes, int test_num)
{
    int result = STATUS_SUCCESS;
   
    int i;

    for (i = 0; i < num_bytes; i++) {
        if (screen[i] != expected_screen[i]) {
            result = STATUS_FAILURE;
            FAIL(test_num);
            break;
        }
    }
    
    return result;
}

static int test_blank_screen(void)
{
    int width = 3, depth = 3;
    uint8_t screen[width*depth];

    init_screen(screen, width, depth);

    uint8_t expected_screen[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    
    return (screen_check(screen, expected_screen, width*depth, 1));
}

static int test_first_pixel(void)
{ 
    int width = 2, depth = 2;
    uint8_t screen[width*depth];

    draw_screen(screen, width, depth, 0, 0, 0);

    uint8_t expected_screen[4] = { 0x80, 0, 0, 0 };
   
    return (screen_check(screen, expected_screen, width*depth, 2));
}

static int test_first_two_pixels(void)
{ 
    int width = 2, depth = 2;
    uint8_t screen[width*depth];
    draw_screen(screen, width, depth, 0, 1, 0);

    uint8_t expected_screen[4] = { 0xC0, 0, 0, 0 };
    
    return (screen_check(screen, expected_screen, width*depth, 3));
}

static int test_first_byte(void)
{ 
    int width = 2, depth = 2;
    uint8_t screen[width*depth];
    draw_screen(screen, width, depth, 0, 7, 0);

    uint8_t expected_screen[4] = { 0xFF, 0, 0, 0 };
    
    return (screen_check(screen, expected_screen, width*depth, 4));
}

static int test_first_two_bytes(void)
{ 
    int width = 2, depth = 2;
    uint8_t screen[width*depth];
    draw_screen(screen, width, depth, 0, 15, 0);

    uint8_t expected_screen[4] = { 0xFF, 0xFF, 0, 0 };
    
    return (screen_check(screen, expected_screen, width*depth, 5));
}

static int test_offset_spanning_two_bytes(void)
{ 
    int width = 3, depth = 1;
    uint8_t screen[width*depth];
    draw_screen(screen, width, depth, 3, 21, 0);

    uint8_t expected_screen[3] = { 0x1F, 0xFF, 0xFC };
    
    return (screen_check(screen, expected_screen, width*depth, 6));
}

static int test_full_row(void)
{ 
    int width = 2, depth = 2;
    uint8_t screen[width*depth];
    draw_screen(screen, width, depth, 0, 15, 1);

    uint8_t expected_screen[4] = { 0, 0, 0xFF, 0xFF };
    
    return (screen_check(screen, expected_screen, width*depth, 7));
}

static int test_x2_exceeds_width(void)
{ 
    int width = 2, depth = 2;
    uint8_t screen[width*depth];
    draw_screen(screen, width, depth, 0, 16, 1);

    uint8_t expected_screen[4] = { 0, 0, 0, 0 };
    
    return (screen_check(screen, expected_screen, width*depth, 8));
}

static int test_x1_exceeds_x2(void)
{ 
    int width = 4, depth = 1;
    uint8_t screen[width*depth];
    draw_screen(screen, width, depth, 15, 14, 0);

    print_screen(screen,width,depth);
    uint8_t expected_screen[4] = { 0, 0, 0, 0 };
    
    return (screen_check(screen, expected_screen, width*depth, 9));
}


