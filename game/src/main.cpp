#include "raylib.h"
#include "raymath.h"
#include "raygui.h"

constexpr float SCREEN_WIDTH = 1200.0f;
constexpr float SCREEN_HEIGHT = 800.0f;
constexpr Vector2 CENTER{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };

// Ball can move half the screen width per-second
constexpr float BALL_SPEED = SCREEN_WIDTH * 0.5f;
constexpr float BALL_SIZE = 40.0f;

// Paddles can move half the screen height per-second
constexpr float PADDLE_SPEED = SCREEN_HEIGHT * 0.5f;
constexpr float PADDLE_WIDTH = 40.0f;
constexpr float PADDLE_HEIGHT = 80.0f;

Rectangle RecFromPoint(Vector2 position, float width, float height)
{
    Rectangle rec;
    rec.x = position.x - width * 0.5f;
    rec.y = position.y - height * 0.5f;
    rec.width = width;
    rec.height = height;
    return rec;
}

Rectangle BallRec(Vector2 position)
{
    Rectangle rec = RecFromPoint(position, BALL_SIZE, BALL_SIZE);
    return rec;
}

Rectangle PaddleRec(Vector2 position)
{
    Rectangle rec = RecFromPoint(position, PADDLE_WIDTH, PADDLE_HEIGHT);
    return rec;
}

void ResetBall(Vector2& position, Vector2& direction)
{
    position = CENTER;
    direction = Vector2Rotate(Vector2UnitX, GetRandomValue(0, 360) * DEG2RAD);
}

int main()
{
    Vector2 ball_position;
    Vector2 ball_direction;
    ResetBall(ball_position, ball_direction);

    Vector2 paddle1_position, paddle2_position;
    paddle1_position.x = SCREEN_WIDTH * 0.05f;
    paddle2_position.x = SCREEN_WIDTH * 0.95f;
    paddle1_position.y = paddle2_position.y = CENTER.y;

    int test_score = 0;

	int player1_score = 0;
	int player2_score = 0;
	float restart_timer = 0.0f;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong");
    InitAudioDevice();
    SetTargetFPS(60);

    Sound coin = LoadSound("./assets/audio/sound_coin.mp3");

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Move paddle with key input
        if (IsKeyDown(KEY_W))
            paddle1_position.y -= PADDLE_SPEED * dt;
        if (IsKeyDown(KEY_S))
            paddle1_position.y += PADDLE_SPEED * dt;

        // Here is where i WIll change paddle 2 input to arrow keys
		if (IsKeyDown(KEY_UP))
			paddle2_position.y -= PADDLE_SPEED * dt;
		if (IsKeyDown(KEY_DOWN))
			paddle2_position.y += PADDLE_SPEED * dt;

        float phh = PADDLE_HEIGHT * 0.5f;
        paddle1_position.y = Clamp(paddle1_position.y, phh, SCREEN_HEIGHT - phh);
        paddle2_position.y = Clamp(paddle2_position.y, phh, SCREEN_HEIGHT - phh);

        // Change the ball's direction on-collision
        Vector2 ball_position_next = ball_position + ball_direction * BALL_SPEED * dt;
        Rectangle ball_rec = BallRec(ball_position_next);
        Rectangle paddle1_rec = PaddleRec(paddle1_position);
        Rectangle paddle2_rec = PaddleRec(paddle2_position);

        // TODO -- increment the scoring player's score after they've touched the ball and the ball goes too far right/left
        test_score++;
        if (ball_rec.x <= 0.0f || ball_rec.x + ball_rec.width >= SCREEN_WIDTH)
        {
            ball_direction.x *= -1.0f;
        }
        if (ball_rec.y <= 0.0f || ball_rec.y + ball_rec.height >= SCREEN_HEIGHT)
        {
            ball_direction.y *= -1.0f;
        }
        if (CheckCollisionRecs(ball_rec, paddle1_rec) || CheckCollisionRecs(ball_rec, paddle2_rec))
        {
            ball_direction.x *= -1.0f;
            PlaySound(coin);
        }


        // Here I will add the scoring logic for player 1 and player 2

		if (player1_score == 5 || player2_score == 5)
		{
			restart_timer += dt; // I added a timer to restart the game after a player wins
			if (restart_timer >= 3.0f) 
			{
				player1_score = 0;
				player2_score = 0;
				ResetBall(ball_position, ball_direction);
				restart_timer = 0.0f;
			}
		}
        else
        {
            if (ball_rec.x <= 0.0f)
            {
                player2_score++;
				test_score = 0; 
                ResetBall(ball_position, ball_direction);
            }
            else if (ball_rec.x + ball_rec.width >= SCREEN_WIDTH)
            {
                player1_score++;
                test_score = 0;
                ResetBall(ball_position, ball_direction);
            }
        }



        // Update ball position after collision resolution, then render
        ball_position = ball_position + ball_direction * BALL_SPEED * dt;

		// Putting this code here allows the screen to show who wins before restarting the game

        BeginDrawing();
        ClearBackground(BLACK);

        if (player1_score == 5)
        {
            // Player 1 wins
            DrawText("Player 1 Wins!", SCREEN_WIDTH * 0.5f - MeasureText("Player 1 Wins!", 40) * 0.5f, SCREEN_HEIGHT * 0.5f - 20, 40, GREEN);


        }
        else if (player2_score == 5)
        {
            // Player 2 wins
            DrawText("Player 2 Wins!", SCREEN_WIDTH * 0.5f - MeasureText("Player 2 Wins!", 40) * 0.5f, SCREEN_HEIGHT * 0.5f - 20, 40, RED);
        }
        else {

            DrawRectangleRec(BallRec(ball_position), WHITE);
            DrawRectangleRec(PaddleRec(paddle1_position), WHITE);
            DrawRectangleRec(PaddleRec(paddle2_position), WHITE);

            // Text format requires you to put a '%i' wherever you want an integer, then add said integer after the comma
            const char* test_score_text = TextFormat("Test Score: %i ", test_score);
            const char* player1_score_text = TextFormat("Player 1: %i", player1_score); // here we are adding player 1 text
            const char* player2_score_text = TextFormat("Player 2: %i", player2_score); // here we are adding player 2 text

            // We can measure our text for more exact positioning. This puts our score in the center of our screen!
            DrawText(test_score_text, SCREEN_WIDTH * 0.5f - MeasureText(test_score_text, 20) * 0.5f, 50, 20, BLUE);
            DrawText(player1_score_text, 50, 50, 20, GREEN); // here we are drawing player 1 score on the left side of the screen
            DrawText(player2_score_text, SCREEN_WIDTH - MeasureText(player2_score_text, 20) - 50, 50, 20, RED); // here we are drawing player 2 score on the right side of the screen
        }
        EndDrawing();
    }

    UnloadSound(coin);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
