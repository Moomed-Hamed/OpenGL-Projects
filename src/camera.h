#include "renderer.h"

#define DIR_FORWARD	0
#define DIR_BACKWARD	1
#define DIR_LEFT	2
#define DIR_RIGHT	3
#define DIR_UP	4
#define DIR_DOWN	5

struct Camera
{
	vec3 position;
	vec3 front, right, up;
	float yaw, pitch;

	vec3 horizontal_offset, vertical_offset; // for panning
	float theta, height; // for rotating
};

void camera_pan(Camera* camera, float dx, float dy, float dtime)
{
	static float horizontal_amount = 0;
	static float vertical_amount   = 0;

	horizontal_amount -= dx * dtime;
	vertical_amount   += dy * dtime;

	camera->horizontal_offset = camera->right * horizontal_amount;
	camera->vertical_offset   = camera->up    * vertical_amount;
}

void camera_rotate(Camera* camera, float dx, float dy, float dtime)
{
	camera->theta  += dx * dtime * .1;
	camera->height += dy * dtime * .4;

	if (camera->theta > TWOPI) camera->theta = 0;
	if (camera->theta < 0    ) camera->theta = TWOPI;
	if (camera->height < 3   ) camera->height = 3;
	if (camera->height > 10  ) camera->height = 10;
}

void camera_update_dir(Camera* camera, float dx, float dy, float sensitivity = 0.003)
{
	camera->yaw   += (dx * sensitivity) / TWOPI;
	camera->pitch += (dy * sensitivity) / TWOPI;

	if (camera->pitch >  PI / 2.01) camera->pitch = PI / 2.01;
	if (camera->pitch < -PI / 2.01) camera->pitch = -PI / 2.01;

	camera->front.y = sin(camera->pitch);
	camera->front.x = cos(camera->pitch) * cos(camera->yaw);
	camera->front.z = cos(camera->pitch) * sin(camera->yaw);

	camera->front = normalize(camera->front);
	camera->right = normalize(cross(camera->front, vec3(0, 1, 0)));
	camera->up    = normalize(cross(camera->right, camera->front));
}

void camera_update_dir(Camera* camera, vec3 front)
{
	camera->front = normalize(front);
	camera->right = normalize(cross(camera->front, vec3(0, 1, 0)));
	camera->up = normalize(cross(camera->right, camera->front));
}

void camera_update_pos(Camera* camera, int direction, float distance)
{
	if (direction == DIR_FORWARD ) camera->position += camera->front * distance;
	if (direction == DIR_LEFT    ) camera->position -= camera->right * distance;
	if (direction == DIR_RIGHT   ) camera->position += camera->right * distance;
	if (direction == DIR_BACKWARD) camera->position -= camera->front * distance;
	if (direction == DIR_UP      ) camera->position += camera->up    * distance;
	if (direction == DIR_DOWN    ) camera->position -= camera->up    * distance;
}

void camera_update(Camera* camera)
{
	float radius = 12;
	camera->position = vec3(radius * sin(camera->theta) + 8, camera->height, radius * cos(camera->theta) + 8);
	camera->position += camera->vertical_offset + camera->horizontal_offset;
	camera_update_dir(camera, (vec3(8, 1, 8) + camera->vertical_offset + camera->horizontal_offset) - camera->position);
}