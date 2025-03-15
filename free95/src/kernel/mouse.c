#include "descriptorTables.h"
#include "isr.h"
#include "string.h"
#include "vga.h"
#include "mouse.h"
#include <stdint.h>
#include <stdbool.h>
#include "string.h"

int g_mouse_x_pos = 0, g_mouse_y_pos = 0;
MOUSE_STATUS g_status;

int mouse_getlc()
{
    return g_status.left_button;
}

int mouse_getrc()
{
    return g_status.right_button;
}

int mGetX() {
    return g_mouse_x_pos;
}

int mGetY() {
    return g_mouse_y_pos;
}

void mouse_wait(bool type) {
    uint32_t time_out = 100000;
    if (type == false) {
        // suspend until status is 1
        while (time_out--) {
            if ((inb(PS2_CMD_PORT) & 1) == 1) {
                return;
            }
        }
        return;
    } else {
        while (time_out--) {
            if ((inb(PS2_CMD_PORT) & 2) == 0) {
                return;
            }
        }
    }
}
extern void mouse_write(uint8_t data);
extern uint8_t mouse_read();


// this mouse read/write casued wack behavure in qemu
// void mouse_write(uint8_t data) {
//     // sending write command
//     mouse_wait(true);
//     outb(PS2_CMD_PORT, 0xD4);
//     mouse_wait(true);
//     // finally write data to port
//     outb(MOUSE_DATA_PORT, data);
// }

// uint8_t mouse_read() {
//     mouse_wait(false);
//     return inb(MOUSE_DATA_PORT);
// }

void get_mouse_status(uint8_t status_byte, MOUSE_STATUS *status) {
    memset(status, 0, sizeof(MOUSE_STATUS));
    if (status_byte & 0x01)
        status->left_button = 1;
    if (status_byte & 0x02)
        status->right_button = 1;
    if (status_byte & 0x04)
        status->middle_button = 1;
    if (status_byte & 0x08)
        status->always_1 = 1;
    if (status_byte & 0x10)
        status->x_sign = 1;
    if (status_byte & 0x20)
        status->y_sign = 1;
    if (status_byte & 0x40)
        status->x_overflow = 1;
    if (status_byte & 0x80)
        status->y_overflow = 1;
}

void print_mouse_info()
{
    /* shit? */
}

void mouse_handler() {
    static uint8_t mouse_cycle = 0;
    static uint8_t mouse_byte[3];

    switch (mouse_cycle) {
        case 0:
            mouse_byte[0] = mouse_read();
            get_mouse_status(mouse_byte[0], &g_status);
            if (!g_status.always_1) {
                mouse_cycle = 0; // Reset even if data is invalid
                return;  // Ignore invalid data
            }
            mouse_cycle++;
            break;
        case 1:
            mouse_byte[1] = mouse_read();
            mouse_cycle++;
            break;
        case 2:
            mouse_byte[2] = mouse_read();
            if (g_status.x_overflow || g_status.y_overflow) {
                mouse_cycle = 0; // Reset even if we discard this update
                return;  // Discard this update due to overflow
            }

            int8_t dx = (int8_t)mouse_byte[1];  // X movement
            int8_t dy = (int8_t)mouse_byte[2];  // Y movement

            // Ensure movement is within valid bounds before applying it
            if (g_mouse_x_pos + dx < 0) 
                g_mouse_x_pos = 0;
            else if (g_mouse_x_pos + dx > SCREEN_WIDTH - 1) 
                g_mouse_x_pos = SCREEN_WIDTH - 1;
            else 
                g_mouse_x_pos += dx;

            if (g_mouse_y_pos + dy < 0)
                g_mouse_y_pos = 0;
            else if (g_mouse_y_pos + dy > SCREEN_HEIGHT - 1) 
                g_mouse_y_pos = SCREEN_HEIGHT - 1;
            else 
                g_mouse_y_pos += dy;

            mouse_cycle = 0;
            break;
    }
}

/**
 * available rates 10, 20, 40, 60, 80, 100, 200
 */
void set_mouse_rate(uint8_t rate) {
    uint8_t status;

    mouse_wait(true);
    outb(MOUSE_DATA_PORT, MOUSE_CMD_SAMPLE_RATE);
    status = mouse_read();
    if(status != MOUSE_ACKNOWLEDGE) {
        return;
    }
    mouse_wait(true);
    outb(MOUSE_DATA_PORT, rate);
    status = mouse_read();
    if(status != MOUSE_ACKNOWLEDGE) {
        return;
    }
}

void mouse_init() {
    uint8_t status;

    g_mouse_x_pos = 5;
    g_mouse_y_pos = 2;

    // enable mouse device
    mouse_wait(true);
    outb(PS2_CMD_PORT, 0xA8);

    // print mouse id
    outb(MOUSE_DATA_PORT, MOUSE_CMD_MOUSE_ID);
    status = mouse_read();
    if (status != 0x00) {
        return;  // Abort initialization if mouse ID is not 0x00 (standard PS/2 mouse)
    }

    set_mouse_rate(10);

    //outb(MOUSE_DATA_PORT, MOUSE_CMD_RESOLUTION);
    //outb(MOUSE_DATA_PORT, 0);

    // enable the interrupt
    mouse_wait(true);
    outb(PS2_CMD_PORT, 0x20);
    mouse_wait(false);
    // get and set second bit
    status = (inb(MOUSE_DATA_PORT) | 2);
    // write status to port
    mouse_wait(true);
    outb(PS2_CMD_PORT, MOUSE_DATA_PORT);
    mouse_wait(true);
    outb(MOUSE_DATA_PORT, status);

    // set mouse to use default settings
    mouse_write(MOUSE_CMD_SET_DEFAULTS);
    status = mouse_read();
    if(status != MOUSE_ACKNOWLEDGE) {
        return;
    }

    // enable packet streaming to receive
    mouse_write(MOUSE_CMD_ENABLE_PACKET_STREAMING);
    status = mouse_read();
    if(status != MOUSE_ACKNOWLEDGE) {
        return;
    }

    // set mouse handler
    register_interrupt_handler(IRQ12, mouse_handler);
}
