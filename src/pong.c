#include "pong.h"

#include "graphics.h"
#include "io.h"
#include "math.h"

#define PADDLE_WIDTH (WIDTH / 8)
#define PADDLE_HEIGHT (PADDLE_WIDTH / 4)
#define PADDLE_POS_Y (HEIGHT - PADDLE_WIDTH * 2)
#define PADDLE_SPEED (WIDTH / 140)
#define BALL_SPEED (WIDTH / 200)
#define BALL_RADIUS (12)

struct PongGame
{
    uint32_t ball_x;
    uint32_t ball_y;
    uint8_t ball_dir; // LSB = 0 means left, LSB + 1 = 0 means up
    uint32_t last_ball_x;
    uint32_t last_ball_y;
    uint32_t paddle_x;
    uint32_t last_paddle_x;
    uint8_t ball_colour;
    uint8_t game_lost;
    uint8_t game_initialized;
    uint32_t x_speed;
};

static struct PongGame game = {};

static uint32_t next = 324;

static uint32_t lcg_rand()
{
    next = (1103515245 * next + 12345) % 2147483648;
    return next;
}

void init_pong()
{
    draw_rect(0, 0, WIDTH, HEIGHT, LIGHT_BLUE);
    game.ball_x = WIDTH / 2 - BALL_RADIUS;
    game.ball_y = BALL_RADIUS;
    game.ball_dir = 0x03;
    game.last_ball_x = BALL_RADIUS;
    game.last_ball_y = BALL_RADIUS;
    game.paddle_x = WIDTH / 2 - PADDLE_WIDTH / 2;
    game.last_paddle_x = game.paddle_x;
    game.ball_colour = LIGHT_GREEN;
    game.game_lost = 0;
    game.game_initialized = 42;
    game.x_speed = BALL_SPEED;
}

static void move(uint32_t* x, uint32_t* y, uint32_t speed)
{
    if ((game.ball_dir & 0x01) != 0)
    {
        // moving right
        *x += speed;
    }
    else
    {
        // moving left
        *x -= speed;
    }

    if ((game.ball_dir & 0x02) != 0)
    {
        // moving down
        *y += speed;
    }
    else
    {
        // moving up
        *y -= speed;
    }
}

void pong_tick(int8_t paddle_dir)
{
    if (game.game_initialized != 42)
    {
        return;
    }

    // move ball
    move(&game.ball_x, &game.ball_y, game.x_speed);

    // clamp inside bounds
    game.ball_x = maximum(game.ball_x, BALL_RADIUS);
    game.ball_x = minimum(game.ball_x, WIDTH - BALL_RADIUS);
    game.ball_y = maximum(game.ball_y, BALL_RADIUS);
    game.ball_y = minimum(game.ball_y, HEIGHT - BALL_RADIUS - 1);

    // check collisions

    uint8_t collision_happened = 0;

    // left wall
    if (game.ball_x <= BALL_RADIUS)
    {
        // make move right
        game.ball_dir |= 0x01;
        collision_happened = 1;
    }

    // right wall
    if (game.ball_x >= WIDTH - BALL_RADIUS)
    {
        // make move left
        game.ball_dir &= ~0x01;
        collision_happened = 1;
    }

    // roof
    if (game.ball_y <= BALL_RADIUS)
    {
        // make move down
        game.ball_dir |= 0x02;
        collision_happened = 1;
    }

    // floor
    if (game.ball_y >= HEIGHT - BALL_RADIUS)
    {
        // make move up
        game.ball_dir &= ~0x02;
        collision_happened = 1;

        // communicate my disappointment
        game.game_lost = 1;
    }

    // paddle
    uint8_t intersecting_top = game.ball_x >= game.paddle_x && game.ball_x <= game.paddle_x + PADDLE_WIDTH && absolute(
        (int32_t)game.ball_y - (PADDLE_POS_Y)) <= BALL_RADIUS;

    uint8_t intersecting_left = game.ball_y >= PADDLE_POS_Y && game.ball_y <= PADDLE_POS_Y + PADDLE_HEIGHT && absolute(
        (int32_t)game.ball_x - game.paddle_x) <= BALL_RADIUS;
    uint8_t intersecting_right = game.ball_y >= PADDLE_POS_Y && game.ball_y <= PADDLE_POS_Y + PADDLE_HEIGHT && absolute(
        (int32_t)game.ball_x - (game.paddle_x + PADDLE_WIDTH)) <= BALL_RADIUS;

    uint8_t intersecting_top_left = distance((int32_t)game.ball_x - (int32_t)game.paddle_x,
                                             (int32_t)game.ball_y - (PADDLE_POS_Y)) <= BALL_RADIUS;
    uint8_t intersecting_top_right = distance((int32_t)game.ball_x - (int32_t)(game.paddle_x + PADDLE_WIDTH),
                                              (int32_t)game.ball_y - (PADDLE_POS_Y)) <= BALL_RADIUS;

    if (intersecting_top || intersecting_left || intersecting_right || intersecting_top_left || intersecting_top_right)
    {
        // make move up
        game.ball_dir &= ~0x02;

        if (intersecting_left || intersecting_top_left)
        {
            // make move left
            game.ball_dir &= ~0x01;
        }

        if (intersecting_right || intersecting_top_right)
        {
            // make move right
            game.ball_dir |= 0x01;
        }

        // move ball to outside, implementing by whichever correction requires the least
        int32_t move_distance_to_left = game.paddle_x - (game.ball_x + BALL_RADIUS);
        int32_t move_distance_to_right = (game.ball_x - BALL_RADIUS) - (game.paddle_x + PADDLE_WIDTH);
        int32_t move_distance_horizontal = move_distance_to_left;
        if (absolute(move_distance_to_left) > absolute(move_distance_to_right))
        {
            move_distance_horizontal = move_distance_to_right;
        }

        int32_t move_distance_to_top = PADDLE_POS_Y - (game.ball_y + BALL_RADIUS);

        if (absolute(move_distance_to_top) < absolute(move_distance_horizontal))
        {
            game.ball_y += move_distance_to_top;
        }
        else
        {
            game.ball_x += move_distance_horizontal;
        }

        // update speed
        game.x_speed = (uint32_t)(BALL_SPEED + (int32_t)lcg_rand() % 3 - 1);

        collision_happened = 1;
    }

    // change ball colour
    if (collision_happened)
    {
        game.ball_colour++;
        if (game.ball_colour >= WHITE) game.ball_colour = LIGHT_GREEN;
    }

    // wait for screen to blank before we start drawing
    while (inb(0x3DA) & 0x08)
    {
    }
    while (!(inb(0x3DA) & 0x08))
    {
    }

    // draw sad face if lost
    if (game.game_lost)
    {
        draw_rect(WIDTH * 2 / 5, HEIGHT / 4, WIDTH / 10, HEIGHT / 3, RED);
        draw_rect(WIDTH * 3 / 5, HEIGHT / 4, WIDTH / 10, HEIGHT / 3, RED);
        draw_rect(WIDTH * 2 / 5, HEIGHT / 4 + HEIGHT / 3 + HEIGHT / 8, WIDTH * 2 / 5 - HEIGHT / 8, HEIGHT / 8, RED);
        draw_rect(WIDTH * 2 / 5 - HEIGHT / 8, HEIGHT / 4 + HEIGHT / 3 + 2 * HEIGHT / 8, HEIGHT / 8, HEIGHT / 8, RED);
        draw_rect(WIDTH * 4 / 5 - HEIGHT / 8, HEIGHT / 4 + HEIGHT / 3 + 2 * HEIGHT / 8, HEIGHT / 8, HEIGHT / 8, RED);
    }

    // draw ball
    draw_circle(game.ball_x, game.ball_y, BALL_RADIUS, game.ball_colour);

    // remove trailing pixels
    for (int i = game.last_ball_x - BALL_RADIUS; i <= game.last_ball_x + BALL_RADIUS; i++)
    {
        for (int j = game.last_ball_y - BALL_RADIUS; j <= game.last_ball_y + BALL_RADIUS; j++)
        {
            int dist_from_current = distance(i - (int)game.ball_x, j - (int)game.ball_y);
            int dist_from_last = distance(i - (int)game.last_ball_x, j - (int)game.last_ball_y);

            if (dist_from_current > BALL_RADIUS && dist_from_last <= BALL_RADIUS)
            {
                put_pixel(i, j, LIGHT_BLUE);
            }
        }
    }

    // update last ball
    game.last_ball_x = game.ball_x;
    game.last_ball_y = game.ball_y;

    // move paddle
    if (paddle_dir == 1 && game.paddle_x < WIDTH - PADDLE_WIDTH)
    {
        game.paddle_x += PADDLE_SPEED;
    }
    if (paddle_dir == 2 && game.paddle_x > 0)
    {
        game.paddle_x -= PADDLE_SPEED;
    }

    // draw paddle
    uint32_t paddle_deviation = absolute((int32_t)game.paddle_x - (int32_t)game.last_paddle_x);

    if (game.paddle_x > game.last_paddle_x)
    {
        draw_rect(game.last_paddle_x, PADDLE_POS_Y, paddle_deviation, PADDLE_HEIGHT, LIGHT_BLUE);
    }
    else if (game.paddle_x < game.last_paddle_x)
    {
        draw_rect(game.paddle_x + PADDLE_WIDTH, PADDLE_POS_Y, paddle_deviation, PADDLE_HEIGHT, LIGHT_BLUE);
    }
    draw_rect(game.paddle_x, PADDLE_POS_Y, PADDLE_WIDTH, PADDLE_HEIGHT, GREEN);

    game.last_paddle_x = game.paddle_x;
}
