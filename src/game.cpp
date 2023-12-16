#include "game.hpp"

//----------------------------------------------------------------------------//

bool _game_camera_init(Camera** cam);
void _game_camera_quit(Camera* cam);
void _game_camera_update(Camera* cam, float dt, GLFWwindow* window);
void _game_camera_cursor_moved(Camera* cam, float x, float y);
void _game_camera_scroll(Camera* cam, float amt);

//----------------------------------------------------------------------------//

void _game_cursor_pos_callback(GLFWwindow* window, double x, double y);
void _game_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void _game_scroll_callback(GLFWwindow* window, double x, double y);

//----------------------------------------------------------------------------//

float _game_exp_scale_factor(float speed, float dt);

//----------------------------------------------------------------------------//

static void _game_message_log(const char* message, const char* file, int32 line);
#define MSG_LOG(m) _game_message_log(m, __FILE__, __LINE__)

static void _game_error_log(const char* message, const char* file, int32 line);
#define ERROR_LOG(m) _game_error_log(m, __FILE__, __LINE__)

//----------------------------------------------------------------------------//

bool game_init(GameState** state)
{
	*state = (GameState*)malloc(sizeof(GameState));
	GameState* s = *state;

	if(!s)
	{
		ERROR_LOG("failed to allocate GameState struct");
		return false;
	}

	if(!draw_init(&s->drawState))
	{
		ERROR_LOG("failed to intialize rendering");
		return false;
	}

	if(!_game_camera_init(&s->cam))
	{
		ERROR_LOG("failed to initialize camera");
		return false;
	}

	//glfwSetInputMode(s->drawState->instance->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetWindowUserPointer(s->drawState->instance->window, s);
	glfwSetCursorPosCallback(s->drawState->instance->window, _game_cursor_pos_callback);
	glfwSetKeyCallback(s->drawState->instance->window, _game_key_callback);
	glfwSetScrollCallback(s->drawState->instance->window, _game_scroll_callback);

	return true;
}

void game_quit(GameState* s)
{
	_game_camera_quit(s->cam);
	draw_quit(s->drawState);
	free(s);
}

//----------------------------------------------------------------------------//

void game_main_loop(GameState* s)
{
	float lastTime = (float)glfwGetTime();

	while(!glfwWindowShouldClose(s->drawState->instance->window))
	{
		float curTime = (float)glfwGetTime();
		float dt = curTime - lastTime;
		lastTime = curTime;

		_game_camera_update(s->cam, dt, s->drawState->instance->window);

		DrawParams drawParams;
		drawParams.cam = s->cam;
		drawParams.numParticles = 1;
		draw_render(s->drawState, drawParams);
	
		glfwPollEvents();
	}
}

//----------------------------------------------------------------------------//

bool _game_camera_init(Camera** camera)
{
	*camera = (Camera*)malloc(sizeof(Camera));
	Camera* cam = *camera;

	if(!cam)
	{
		ERROR_LOG("failed to allocate Camera struct");
		return false;
	}

	cam->up = {0.0f, 0.0f, 1.0f};
	cam->center = cam->targetCenter = {0.0f, 0.0f, 0.0f};

	cam->dist = cam->targetDist = cam->maxDist = 16000.0f;
	cam->angle = cam->targetAngle = 45.0f;

	cam->fov = 45.0f;
	cam->nearPlane = 0.1f;

	return true;
}

void _game_camera_quit(Camera* cam)
{
	free(cam);
}

void _game_camera_update(Camera* cam, float dt, GLFWwindow* window)
{
	float camSpeed = 1.0f * dt * cam->dist;
	float angleSpeed = 45.0f * dt;

	qm::vec4 forward4 = qm::rotate(cam->up, cam->angle) * qm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	qm::vec4 side4 = qm::rotate(cam->up, cam->angle) * qm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

	qm::vec3 forward = qm::vec3(forward4.x, forward4.y, forward4.z);
	qm::vec3 side = qm::vec3(side4.x, side4.y, side4.z);

	qm::vec3 camVel(0.0f, 0.0f, 0.0f);
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camVel = camVel - forward;
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camVel = camVel + forward;
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camVel = camVel - side;
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camVel = camVel + side;

	if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cam->targetAngle -= angleSpeed;
	if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cam->targetAngle += angleSpeed;

	cam->targetCenter = cam->targetCenter + camSpeed * qm::normalize(camVel);

	cam->center = cam->center + (cam->targetCenter - cam->center) * _game_exp_scale_factor(0.985f, dt);
	cam->dist += (cam->targetDist - cam->dist) * _game_exp_scale_factor(0.99f, dt);
	cam->angle += (cam->targetAngle - cam->angle) * _game_exp_scale_factor(0.99f, dt);

	qm::vec4 toPos = qm::rotate(side, -45.0f) * qm::vec4(forward, 1.0f);
	cam->pos = cam->center + cam->dist * qm::normalize(qm::vec3(toPos.x, toPos.y, toPos.z));
}

void _game_camera_cursor_moved(Camera* cam, float x, float y)
{

}

void _game_camera_scroll(Camera* cam, float amt)
{
	cam->targetDist -= 0.1f * cam->targetDist * amt;
	cam->targetDist = roundf(cam->targetDist * 100.0f) / 100.0f;

	if(cam->targetDist < 1.0f)
		cam->targetDist = 1.0f;
	
	if(cam->targetDist > cam->maxDist)
		cam->targetDist = cam->maxDist;
}

//----------------------------------------------------------------------------//

void _game_cursor_pos_callback(GLFWwindow* window, double x, double y)
{
	GameState* s = (GameState*)glfwGetWindowUserPointer(window);

	_game_camera_cursor_moved(s->cam, (float)x, (float)y);
}

void _game_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void _game_scroll_callback(GLFWwindow* window, double x, double y)
{
	GameState* s = (GameState*)glfwGetWindowUserPointer(window);

	if(y != 0.0)
		_game_camera_scroll(s->cam, (float)y);
}

//----------------------------------------------------------------------------//

float _game_exp_scale_factor(float speed, float dt)
{
	return 1.0f - powf(speed, 1000.0f * dt);
}

//----------------------------------------------------------------------------//

static void _game_message_log(const char* message, const char* file, int32 line)
{
	printf("GAME MESSAGE in %s at line %i - \"%s\"\n\n", file, line, message);
}

static void _game_error_log(const char* message, const char* file, int32 line)
{
	printf("GAME ERROR in %s at line %i - \"%s\"\n\n", file, line, message);
}